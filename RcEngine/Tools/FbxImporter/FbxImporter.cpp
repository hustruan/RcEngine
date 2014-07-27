#include "FbxImporter.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/VertexDeclaration.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileStream.h>
#include <IO/PathUtil.h>
#include <set>
#include <fstream>
#include <limits> 
#include <stack>

#include "ExportLog.h"

using namespace std;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(mFBXSdkManager->GetIOSettings()))
#endif

ConsoleOutListener g_ConsoleOutListener;
DebugSpewListener  g_DebugSpewListener;
ExportSettings     g_ExportSettings;

#define MAXBONES_PER_VERTEX 4

namespace {

uint32_t CalculateVertexSize(uint32_t vertexFlag)
{
	uint32_t size = 0;

	size += (vertexFlag & Vertex::ePosition) ? 12 : 0;
	size += (vertexFlag & Vertex::eBlendWeight) ? 16 : 0;
	size += (vertexFlag & Vertex::eBlendIndices) ? 16 : 0;
	size += (vertexFlag & Vertex::eNormal) ? 12 : 0;
	size += (vertexFlag & Vertex::eTexcoord0) ? 8 : 0;
	size += (vertexFlag & Vertex::eTexcoord1) ? 8 : 0;
	size += (vertexFlag & Vertex::eTangent) ? 12 : 0;
	size += (vertexFlag & Vertex::eBinormal) ? 12 : 0;

	return size;
}

void GetVertexDeclaration(uint32_t vertexFlag, std::vector<VertexElement>& elements, uint32_t& vertexSize)
{
	size_t offset = 0;

	if (vertexFlag & Vertex::ePosition)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Position, 0));
		offset += 12;
	}

	if (vertexFlag & Vertex::eBlendWeight)
	{
		elements.push_back(VertexElement(offset, VEF_Float4, VEU_BlendWeight, 0));
		offset += 16;
	}

	if (vertexFlag & Vertex::eBlendIndices)
	{
		elements.push_back(VertexElement(offset, VEF_UInt4, VEU_BlendIndices, 0));
		offset += 16;
	}

	if (vertexFlag & Vertex::eNormal)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Normal, 0));
		offset += 12;
	}

	if (vertexFlag & Vertex::eTexcoord0)
	{
		elements.push_back(VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 0));
		offset += 8;
	}

	if (vertexFlag & Vertex::eTexcoord1)
	{
		elements.push_back(VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 1));
		offset += 8;
	}

	if (vertexFlag & Vertex::eTangent)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Tangent, 0));
		offset += 12;
	}

	if (vertexFlag & Vertex::eBinormal)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Binormal, 0));
		offset += 12;
	}

	vertexSize = offset;
}

void CorrectName(String& matName)
{
	std::replace(matName.begin(), matName.end(), ':', '_');
}

inline float4 float4FromFBX(const FbxVector4& fbx)
{
	return float4(float(fbx[0]), float(fbx[1]), float(fbx[2]), float(fbx[3]));
}

inline float3 float3FromFBX(const FbxVector4& fbx)
{
	return float3(float(fbx[0]), float(fbx[1]), float(fbx[2]));
}

inline float2 float2FromFBX(const FbxVector2& fbx)
{
	return float2(float(fbx[0]), float(fbx[1]));
}

inline Quaternionf quatFromFBX(const FbxQuaternion& quat) 
{
	return Quaternionf((float)quat[3], (float)quat[0], (float)quat[1], (float)quat[2]);
}

float4x4 matrixFromFBX(const FbxAMatrix& fbxMatrix)
{
	FbxVector4 translation = fbxMatrix.GetT();
	FbxVector4 scale = fbxMatrix.GetS();
	FbxQuaternion rotation = fbxMatrix.GetQ();

	return CreateTransformMatrix(float3((float)scale[0], (float)scale[1], (float)scale[2]), 
							     Quaternionf((float)rotation[3], (float)rotation[0], (float)rotation[1], (float)rotation[2]),
								 float3((float)translation[0], (float)translation[1], (float)translation[2]));
}

float3 ReadMaterialColor(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty)
{
	float3 Color;

	if( FBXColorProperty.IsValid() )
	{
		FbxDouble3 FBXColor = FBXColorProperty.Get();
		Color = float3( 
			static_cast<float>(FBXColor[0]),
			static_cast<float>(FBXColor[1]),
			static_cast<float>(FBXColor[2]) );

		if( FBXFactorProperty.IsValid() )
		{
			double FBXFactor = FBXFactorProperty.Get();
			Color = Color * static_cast<float>(FBXFactor);
		}
	}
	return Color;
}

bool GetMaterialTexture(const FbxSurfaceMaterial *pMaterial, const char *pPropertyName, String* filename)
{
	FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);

	if (!lProperty.IsValid())
		return false;

	if (int lTextureCount = lProperty.GetSrcObjectCount(FbxFileTexture::ClassId))
	{
		FbxTexture* lTexture = FbxCast<FbxTexture>(lProperty.GetSrcObject(FbxTexture::ClassId, 0));  
		if (lTexture)
		{
			if( FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture))
			{
				*filename = lFileTexture->GetFileName();
			}		
		}
	}

	return true;
}

FbxNode* GetBoneRoot(FbxNode* boneNode)
{
	assert(boneNode &&
		boneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);

	String name = boneNode->GetName();
	FbxNode* parentNode = boneNode->GetParent();
	FbxNodeAttribute* pParentNodeAttribute = parentNode->GetNodeAttribute();

	while(parentNode &&  pParentNodeAttribute && pParentNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		boneNode = parentNode;
		parentNode = parentNode->GetParent();
		pParentNodeAttribute = parentNode->GetNodeAttribute();
	}

	return boneNode;
}

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

/**
 * Get the global position of the node for the current pose. If the specified node
 * is not part of the pose or no pose is specified, get its global position at the current time.
 */
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = NULL, FbxAMatrix* pParentGlobalPosition = NULL)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

bool HasSkin(FbxMesh* pMesh)
{
	const int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int iSkinIndex = 0; iSkinIndex < lSkinCount; ++iSkinIndex)
	{
		if( ((FbxSkin*)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin))->GetClusterCount() > 0 )
		{
			return true;
		}
	}

	return false;
}

float3 BakePosition(const float3& pos, const float4x4& globalTransform)
{
	float4 baked = float4(pos[0], pos[1], pos[2], 1.0f) * globalTransform;
	return float3(baked[0], baked[1], baked[2]);
}

float3 BakeDirection(const float3& dir, const float4x4& globalTransform)
{
	float4 baked = float4(dir[0], dir[1], dir[2], 0.0f) * globalTransform;
	return float3(baked[0], baked[1], baked[2]);
}

} // Namespace

//---------------------------------------------------------------------------------------------
void FBXTransformer::Initialize( FbxScene* pScene )
{
	// Doesn't work as expected. Still need to transform vertices.
	//FbxAxisSystem::DirectX.ConvertScene(mFBXScene); 

	FbxAxisSystem sceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();

	bool mayaUp = sceneAxisSystem == FbxAxisSystem::MayaYUp;

	int upSign;
	FbxAxisSystem::EUpVector eUp = sceneAxisSystem.GetUpVector(upSign);
	assert(upSign == 1);

	if (eUp == FbxAxisSystem::eZAxis)
	{
		//ExportLog::LogMsg( 4, "Converting from Z-up axis system to Y-up axis system." );
		mMaxConversion = true;
	}
	else
	{
		mMaxConversion = false;
	}

	//FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
	//if( SceneSystemUnit.GetScaleFactor() != 1.0 )
	//{
	//	//The unit in this example is centimeter.
	//	FbxSystemUnit::cm.ConvertScene( pScene );
	//}
}

void FBXTransformer::TransformPosition( float3* pDestPosition, const float3* pSrcPosition ) const
{
	float3 SrcVector;
	if( pSrcPosition == pDestPosition )
	{
		SrcVector = *pSrcPosition;
		pSrcPosition = &SrcVector;
	}

	if( mMaxConversion )
	{
		pDestPosition->X() = pSrcPosition->X();
		pDestPosition->Y() = pSrcPosition->Z();
		pDestPosition->Z() = pSrcPosition->Y();
	}
	else
	{
		pDestPosition->X() = pSrcPosition->X();
		pDestPosition->Y() = pSrcPosition->Y(); 
		pDestPosition->Z() = -pSrcPosition->Z();
	}
}

void FBXTransformer::TransformDirection( float3* pDestDirection, const float3* pSrcDirection ) const
{
	float3 SrcVector;
	if( pSrcDirection == pDestDirection )
	{
		SrcVector = *pSrcDirection;
		pSrcDirection = &SrcVector;
	}

	if( mMaxConversion )
	{
		pDestDirection->X() = pSrcDirection->X();
		pDestDirection->Y() = pSrcDirection->Z();
		pDestDirection->Z() = pSrcDirection->Y();
	}
	else
	{
		pDestDirection->X() = pSrcDirection->X();
		pDestDirection->Y() = pSrcDirection->Y();
		pDestDirection->Z() = -pSrcDirection->Z();
	}
}

void FBXTransformer::TransformMatrix( float4x4* pDestMatrix, const float4x4* pSrcMatrix ) const
{
	float4x4 SrcMatrix;
	if( pSrcMatrix == pDestMatrix )
	{
		memcpy( &SrcMatrix, pSrcMatrix, sizeof( float4x4 ) );
		pSrcMatrix = &SrcMatrix;
	}
	memcpy( pDestMatrix, pSrcMatrix, sizeof( float4x4 ) );

	// What we're doing here is premultiplying by a left hand -> right hand matrix,
	// and then postmultiplying by a right hand -> left hand matrix.
	// The end result of those multiplications is that the third row and the third
	// column are negated (so element _33 is left alone).  So instead of actually
	// carrying out the multiplication, we just negate the 6 matrix elements.

	pDestMatrix->M13 = -pSrcMatrix->M13;
	pDestMatrix->M23 = -pSrcMatrix->M23;
	pDestMatrix->M43 = -pSrcMatrix->M43;

	pDestMatrix->M31 = -pSrcMatrix->M31;
	pDestMatrix->M32 = -pSrcMatrix->M32;
	pDestMatrix->M34 = -pSrcMatrix->M34;
}

//---------------------------------------------------------------------------------------------
FbxProcesser::FbxProcesser(void)
	: mFBXScene(nullptr), 
	  mFBXSdkManager(nullptr),
	  mQuietMode(false),
	  mMergeScene(false)
{
}

FbxProcesser::~FbxProcesser(void)
{
}

void FbxProcesser::Initialize()
{
	if (mFBXSdkManager)
		mFBXSdkManager->Destroy();

	mFBXSdkManager = FbxManager::Create();
	if( !mFBXSdkManager )
	{
		ExportLog::LogError("Error: Unable to create FBX Manager!");
		exit(1);
	}
	else
		ExportLog::LogMsg(0, "Autodesk FBX SDK version %s", mFBXSdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(mFBXSdkManager, IOSROOT);
	mFBXSdkManager->SetIOSettings(ios);

	mFBXScene = FbxScene::Create(mFBXSdkManager , "");  
	if( !mFBXScene )
	{
		ExportLog::LogError("Error: Unable to create FBX scene!");
		exit(1);
	}
}

//////////////////////////////////////////////////////////////////////////
void BoneWeights::AddBoneWeight( int nBoneIndex, float fBoneWeight )
{
	if( fBoneWeight <= 0.0f )
		return;

	bool added = false;
	for (size_t i = 0; i < mBoneWeights.size(); ++i)
	{
		if (fBoneWeight > mBoneWeights[i].second)
		{
			mBoneWeights.insert(mBoneWeights.begin() + i, std::make_pair(nBoneIndex,fBoneWeight));
			added = true;
			break;
		}
	}

	if (!added)
	{
		mBoneWeights.push_back(std::make_pair(nBoneIndex,fBoneWeight));
	}
}

void BoneWeights::Validate()
{
	// make sure not exceed max bone per vertex
	int size = mBoneWeights.size();
	if (size > MAXBONES_PER_VERTEX)
	{
		mBoneWeights.resize(MAXBONES_PER_VERTEX);
	}
	else
	{
		std::pair<int, float> dummy(0, 0.0f);
		mBoneWeights.resize(MAXBONES_PER_VERTEX, dummy);
	}
}

void BoneWeights::Normalize()
{
	Validate();

	float sum = 0.0f;
	std::for_each(mBoneWeights.begin(), mBoneWeights.end(), [&sum](const std::pair<int, float>& value) 
	{
		sum += value.second;
	} );

	float scale = 1.0f / sum;
	std::for_each(mBoneWeights.begin(), mBoneWeights.end(), [scale](std::pair<int, float>& value)
	{
		value.second *= scale;
	});
}

//////////////////////////////////////////////////////////////////////////
bool FbxProcesser::LoadScene( const String& filename )
{
	if (!mFBXSdkManager)
	{
		ExportLog::LogError("Create FBX Manager first!");
		return false;
	}

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	bool bResult;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	ExportLog::LogMsg(1, "FBX file format version for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(mFBXSdkManager,"");

	// Initialize the importer by providing a filename.
	bResult = lImporter->Initialize(filename.c_str(), -1, mFBXSdkManager->GetIOSettings());
	if( !bResult )
	{
		ExportLog::LogError("Call to FbxExporter::LoadScene() failed.");
		//ExportLog::LogError("Error returned: %s\n\n", lImporter->GetLastErrorString());
		return false;
	}

	// Import the scene.
	bResult = lImporter->Import(mFBXScene);
	if( !bResult )
	{
		ExportLog::LogError( "Could not load FBX file \"%s\".", filename.c_str() );
		return false;
	}

	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
	ExportLog::LogMsg(1, "FBX file format version for file '%s' is %d.%d.%d", filename.c_str(), lFileMajor, lFileMinor, lFileRevision);

	mFBXTransformer.Initialize(mFBXScene);

	// Destroy the importer.
	lImporter->Destroy();

	mOutputPath = PathUtil::GetPath(filename);
	mSceneName = PathUtil::GetFileName(filename);

	return bResult;
}

void FbxProcesser::CollectSceneNodes( FbxNode* pNode )
{
	if( !pNode ) return;

	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		switch(pNodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eSkeleton:
			mFBXBoneNodes[pNode] = false;
			break;
		case FbxNodeAttribute::eMesh:
			mFBXMeshNodes.insert(pNode);
			break;
		case FbxNodeAttribute::eCamera:
			mFBXCameraNodes.insert(pNode);
			break;
		case FbxNodeAttribute::eLight:
			mFBXLightNodes.insert(pNode);
			break;
		case FbxNodeAttribute::eSubDiv:
		default:
			break;
		};
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
		CollectSceneNodes(pNode->GetChild(i));
}

void FbxProcesser::BuildSkeleton()
{
	// Figure out bone nodes referenced by mesh
	for (FbxNode* pMeshNode : mFBXMeshNodes)
		ComputeSkeletonBindPose(pMeshNode);

	auto it = mFBXMeshSkeletonMap.begin();
	while (it != mFBXMeshSkeletonMap.end())
	{
		auto& kv = *it++;

		const vector<FbxNode*>& meshBones = kv.second.BoneNodes;
		// 可能是绑定到骨骼上的武器
		if (kv.second.BoneNodes.size() == 1)
			mFBXMeshSkeletonMap.erase(kv.first);
	
		shared_ptr<Skeleton> meshSkeleton( new Skeleton() );

		for (FbxNode* boneNodeFBX : meshBones)
		{
			Bone* newBone;

			auto it = std::find(meshBones.begin(), meshBones.end(), boneNodeFBX->GetParent());	
			if (it == meshBones.end())
			{
				newBone = meshSkeleton->AddBone(boneNodeFBX->GetName(), nullptr);
			}
			else
			{
				uint32_t parentIndex = std::distance(meshBones.begin(), it);
				Bone* parentBone = meshSkeleton->GetBone(parentIndex);
				assert(parentBone);
				
				newBone = meshSkeleton->AddBone(boneNodeFBX->GetName(), parentBone);
			}

			ComputeBoneLocalPose(boneNodeFBX, newBone);
		}

		kv.second.Skeleton = meshSkeleton;
	}
}

void FbxProcesser::ComputeBoneLocalPose( FbxNode* pBoneFBX, Bone* pBone )
{
	auto it = mBoneBindPose.find(pBoneFBX);
	if (it != mBoneBindPose.end())
	{
		float4x4 boneBindPose = matrixFromFBX(it->second);
		mFBXTransformer.TransformMatrix(&boneBindPose, &boneBindPose);

		if (pBone->GetParent())
		{
			float4x4 paretTrans = pBone->GetParent()->GetWorldTransform();
			boneBindPose = boneBindPose * paretTrans.Inverse();
		}

		float3 pos, scale;
		Quaternionf rot;
		MatrixDecompose(scale, rot, pos, boneBindPose);

		pBone->SetTransform(pos, rot, scale);
	}
	else
	{
		/**
		 * Calculate bind pose matrix for bones that don't have a FbxCluster, have no influence on mesh points
		 */ 

		//FbxNode* pBoneNode = mBoneMap[bone->GetName()];

		//Bone* parent = static_cast<Bone*>(bone->GetParent());
		//if (!parent)
		//{
		//	bindPoseMap[bone] = pBoneNode->EvaluateGlobalTransform();
		//}
		//else
		//{
		//	bindPoseMap[bone] = bindPoseMap[parent] * pBoneNode->EvaluateLocalTransform() * GetGeometry(pBoneNode);
		//}

		//float4x4 boneBindPose = matrixFromFBX(bindPoseMap[bone]);
		//mFBXTransformer.TransformMatrix(&boneBindPose, &boneBindPose);

		//if (bone->GetParent())
		//{
		//	float4x4 paretTrans = bone->GetParent()->GetWorldTransform();
		//	boneBindPose = boneBindPose * paretTrans.Inverse();
		//}

		//float3 pos, scale;
		//Quaternionf rot;
		//MatrixDecompose(scale, rot, pos, boneBindPose);

		//bone->SetTransform(pos, rot, scale);
	}
}

void FbxProcesser::ComputeSkeletonBindPose( FbxNode* pMeshNode )
{
	if (pMeshNode == nullptr || pMeshNode->GetMesh() == nullptr)
		return;

	FbxMesh* pMesh = pMeshNode->GetMesh();
	if (!pMesh->IsTriangleMesh())
	{
		FbxGeometryConverter GeometryConverter(mFBXSdkManager);
		if( !GeometryConverter.Triangulate(pMeshNode->GetNodeAttribute(), true) )
		{
			ExportLog::LogError("FbxMesh: %s triangulation failed!", pMeshNode->GetName());
			return;
		}
		pMesh = pMeshNode->GetMesh();
	}

	if( HasSkin(pMesh) )
	{
		FbxAMatrix lReferenceGlobalInitPosition;
		FbxAMatrix lClusterGlobalInitPosition;

		FbxAMatrix lReferenceGeometry;
		FbxAMatrix lGlobalPosition;

		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lGlobalPosition = pMesh->GetNode()->EvaluateGlobalTransform();

		for( int i = 0; i < pMesh->GetDeformerCount(); ++i )
		{
			FbxDeformer* pFBXDeformer = pMesh->GetDeformer(i);

			if( !pFBXDeformer )
				continue;

			if( pFBXDeformer->GetDeformerType() == FbxDeformer::eSkin )
			{
				FbxSkin* pFBXSkin = (FbxSkin *)(pFBXDeformer);

				if( !pFBXSkin )
					continue;

				FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize; //Default link mode

				int nClusterCount = pFBXSkin->GetClusterCount();
				for( int i = 0; i < nClusterCount; ++i )
				{
					FbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);
					if( !pFBXCluster )
						continue;

					linkMode = pFBXCluster->GetLinkMode();
					FbxNode* pLinkNode = pFBXCluster->GetLink();

					if( !pLinkNode )
						continue;

					// Find which skeleton this mesh is skin to
					if (mFBXBoneNodes.find(pLinkNode) == mFBXBoneNodes.end())
					{
						ExportLog::LogWarning("Export Animation Error: Bone %s not exits!\n", pLinkNode->GetName());
						assert(false);
						continue;
					}

					/**
						* See http://forums.autodesk.com/t5/FBX-SDK/Getting-the-local-transformation-matrix-for-the-vertices-of-a/td-p/4190364
						*     http://stackoverflow.com/questions/13566608/loading-skinning-information-from-fbx
						*     http://forums.autodesk.com/t5/FBX-SDK/Skeleton-Bind-Pose-for-bones-not-linked-to-clusters/m-p/4173622/highlight/true#M3264
						*/
				
					// Multiply lReferenceGlobalInitPosition by Geometric Transformation
					pFBXCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
					lReferenceGlobalInitPosition *= lReferenceGeometry;

					// Get the link initial global position
					pFBXCluster->GetTransformLinkMatrix( lClusterGlobalInitPosition );

					// InvBindPose, Matrix(Global->Joint)
					FbxAMatrix lInvBindPose = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

					// BindPose, Matrix(Joint->Global)
					FbxAMatrix lBindPose = lGlobalPosition * lReferenceGlobalInitPosition.Inverse() * lClusterGlobalInitPosition;
	
					mBoneBindPose[pLinkNode] = lBindPose;
					
					// Make sure all parents exit in front of this bone in vector
					std::stack<FbxNode*> nodeStack;
					
					// Mark bone and all parent as referenced
					while(mFBXBoneNodes.count(pLinkNode))
					{
						mFBXBoneNodes[pLinkNode] = true; 
						nodeStack.push(pLinkNode);
						pLinkNode = pLinkNode->GetParent();
					}

					while (nodeStack.size())
					{
						FbxNode* pLinkNode = nodeStack.top();
						nodeStack.pop();

						vector<FbxNode*>& meshBoneNodes = mFBXMeshSkeletonMap[pMesh].BoneNodes;
						if (std::find(meshBoneNodes.begin(), meshBoneNodes.end(), pLinkNode) == meshBoneNodes.end())
							meshBoneNodes.push_back(pLinkNode);
					}
				} 
			}
		}
	}
}

void FbxProcesser::ProcessScene( )
{
	CollectMaterials();
	CollectSceneNodes(mFBXScene->GetRootNode());
	if (g_ExportSettings.ExportSkeleton && mFBXBoneNodes.size())
		BuildSkeleton();

	// Process each mesh node
	for (FbxNode* pMeshNode : mFBXMeshNodes)
		ProcessMesh(pMeshNode);

	if (g_ExportSettings.ExportAnimation && mFBXBoneNodes.size())
		CollectAnimations();

	if (g_ExportSettings.MergeScene)
		MergeSceneMeshs();

	MergeMeshParts();
}

void FbxProcesser::ProcessMesh( FbxNode* pNode )
{
	FbxMesh* pMesh = pNode->GetMesh();
	if (!pMesh) return;

	ExportLog::LogMsg(0, "Process mesh: %s\n", pNode->GetName());

	// material count this mesh will use
	int numMaterial = pNode->GetMaterialCount();

	// just use layer 0.
	FbxLayer* layer = pMesh->GetLayer(0);

	int tangentRefMode = layer->GetTangents() ? layer->GetTangents()->GetReferenceMode() : -1;
	int binormalRefMode = layer->GetBinormals() ? layer->GetBinormals()->GetReferenceMode() : -1;

	// group the polygons by material
	vector< vector<int> > polysByMaterial;
	polysByMaterial.resize((std::max)(1,numMaterial));
    const FbxLayerElementMaterial* materials = layer->GetMaterials();

	bool useDefaultMat = false;
	if (materials) 
	{
		FbxLayerElement::EMappingMode mm = materials->GetMappingMode();

		static const char *mm_str[] = { "eNONE", "eBY_CONTROL_POINT", "eBY_POLYGON_VERTEX", "eBY_POLYGON", "eBY_EDGE", "eALL_SAME" };
		
		if( !(mm == FbxLayerElement::eByPolygon || mm == FbxLayerElement::eAllSame) )
		{
			// only support eByPolygon and eAllSame
			ExportLog::LogError("Unsupported mapping mode: %s", ((mm <= FbxLayerElement::eAllSame) ? mm_str[mm] : "unknown"));
			return;
		}
			
		const FbxLayerElementArrayTemplate<int>& polyMatIndices = materials->GetIndexArray();

		if (mm == FbxLayerElement::eByPolygon) 
		{
			// each poly has its own material index
			for (int i = 0; i < polyMatIndices.GetCount(); ++i)
			{
				int materialIndex = polyMatIndices.GetAt(i);  
				polysByMaterial[materialIndex].push_back(i);
			}	
		} 
		else if (mm == FbxLayerElement::eAllSame) 
		{
			// all the polys share the same material
			for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
			{
				polysByMaterial[0].push_back(i);
			}
		}
	} 
	else 
	{
		// no materials found, so use default material
		useDefaultMat = true;
		for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
			polysByMaterial[0].push_back(i);
	}

	// get the uv sets
	vector<String> uvSets;
	for (int i = 0; i < layer->GetUVSetCount(); ++i)
		uvSets.push_back(layer->GetUVSets()[i]->GetName());
	
	// build mesh data
	shared_ptr<MeshData> mesh = std::make_shared<MeshData>();

	// read name
	mesh->Name = pNode->GetName();
	CorrectName(mesh->Name);

	bool lHasSkin = mFBXMeshSkeletonMap.count(pMesh) > 0;
	std::vector<BoneWeights> meshBoneWeights;
	if (lHasSkin)
	{
		meshBoneWeights.resize(pMesh->GetControlPointsCount());
		// Deform the vertex array with the skin deformer.
		mesh->Skeleton = ProcessBoneWeights(pMesh, meshBoneWeights);	
	}
 
	float4x4 totalMatrix = matrixFromFBX( pNode->EvaluateGlobalTransform() * GetGeometry(pNode) );
	float4x4 totalMatrixNormal = totalMatrix.Inverse();
	totalMatrixNormal = totalMatrixNormal.Transpose();
	
	for (size_t mi = 0; mi < polysByMaterial.size(); ++mi)
	{
		shared_ptr<MeshPartData> meshPart( new MeshPartData() );
	
		// mesh part name
		meshPart->Name = pNode->GetName();
		if (polysByMaterial.size() > 1)
			meshPart->Name += std::to_string(mi);

		// material name
		meshPart->MaterialName = useDefaultMat ? "DefaultMaterial" : pNode->GetMaterial(mi)->GetName();
		CorrectName(meshPart->MaterialName);

		uint32_t vertexFlag = 0;
		
		if (tangentRefMode != -1)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);
			
			if (material)
			{
				String filename;
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sBump, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0| Vertex::eTangent;
				}
				
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sNormalMap, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0| Vertex::eTangent;
				}
			}		
		}

		if (!useDefaultMat)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);

			if (material)
			{
				String filename;
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sDiffuse, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0;
				}
			}		
		}

		if (lHasSkin)
		{
			vertexFlag |= Vertex::eBlendIndices;
			vertexFlag |= Vertex::eBlendWeight;
		}

		std::set<Vertex> exitingVertices;

		for (size_t pi = 0; pi < polysByMaterial[mi].size(); ++pi)
		{
			int polyIndex = polysByMaterial[mi][pi];
			assert(pMesh->GetPolygonSize(polyIndex) == 3);

			for (size_t vi = 0; vi < 3; ++vi)
			{
				Vertex vertex;
				
				// get position
				vertexFlag |= Vertex::ePosition;
				int ctrlPointIndex = pMesh->GetPolygonVertex(polyIndex , vi); 

				FbxVector4 fbxPosition = pMesh->GetControlPointAt(ctrlPointIndex);
				vertex.Position = BakePosition(float3FromFBX(fbxPosition), totalMatrix);	
				mFBXTransformer.TransformPosition(&vertex.Position, &vertex.Position);
				
				meshPart->Bound.Merge(vertex.Position);

				if (vertexFlag & Vertex::eBlendWeight)
				{
					auto& boneWeights = meshBoneWeights[ctrlPointIndex];
					boneWeights.Normalize();

					for (auto& kv : boneWeights.GetBoneWeights())
					{
						vertex.BlendIndices.push_back(kv.first);
						vertex.BlendWeights.push_back(kv.second);
					}
				}

				// get normal
				FbxVector4 fbxNormal;
				if( pMesh->GetPolygonVertexNormal(polyIndex, vi, fbxNormal) )
				{
					vertex.Normal = BakeDirection(float3FromFBX(fbxNormal), totalMatrixNormal);	
					mFBXTransformer.TransformDirection(&vertex.Normal, &vertex.Normal);

					vertex.Normal.Normalize();
					vertexFlag |= Vertex::eNormal;
				}
						
				for (size_t uv_idx = 0; uv_idx  < uvSets.size(); ++uv_idx)
				{
					FbxVector2 uv;
					bool hadUV;
					if( pMesh->GetPolygonVertexUV(polyIndex, vi, uvSets[uv_idx].c_str(), uv, hadUV) )
					{
						// flip the y-coordinate to match DirectX
						uv[1] = 1 - uv[1];

						if (uv_idx == 0) 
						{
							vertex.Tex0 = float2FromFBX(uv);
							vertexFlag |= Vertex::eTexcoord0;
						}
						else if (uv_idx == 1) 
						{
							vertex.Tex1 = float2FromFBX(uv);
							vertexFlag |= Vertex::eTexcoord1;
						} 
						else 
						{
							std::cerr << "Too many uv-sets!\n" ;
						}
					}						
				}

				if (vertexFlag & Vertex::eTangent)
				{
					FbxVector4 fbxTangent, fbxBinormal;

					if (tangentRefMode == FbxLayerElement::eDirect) 
						fbxTangent = layer->GetTangents()->GetDirectArray()[ctrlPointIndex + vi];
					else
					{
						int idx = layer->GetTangents()->GetIndexArray()[ctrlPointIndex + vi];
						fbxTangent = layer->GetTangents()->GetDirectArray()[idx];
					}
					vertexFlag |= Vertex::eTangent;

					if (binormalRefMode == FbxLayerElement::eDirect) 
						fbxBinormal = layer->GetBinormals()->GetDirectArray()[ctrlPointIndex + vi];
					else 
					{
						int idx = layer->GetBinormals()->GetIndexArray()[ctrlPointIndex + vi];
						fbxBinormal = layer->GetBinormals()->GetDirectArray()[idx];
					}
					vertexFlag |= Vertex::eBinormal;


					vertex.Tangent = BakeDirection(float3FromFBX(fbxTangent), totalMatrixNormal);
					vertex.Binormal = BakeDirection(float3FromFBX(fbxBinormal), totalMatrixNormal);

					mFBXTransformer.TransformDirection(&vertex.Tangent, &vertex.Tangent);
					mFBXTransformer.TransformDirection(&vertex.Binormal, &vertex.Binormal);

					vertex.Tangent.Normalize();
					vertex.Binormal.Normalize();		
				}

				size_t index;

				std::set<Vertex>::iterator exitingIter = exitingVertices.find(vertex);
				if (exitingIter == exitingVertices.end())
				{
					vertex.Index = meshPart->Vertices.size();
					index = vertex.Index;
					vertex.Flags = vertexFlag;
					exitingVertices.insert(vertex);
					meshPart->Vertices.push_back(vertex);
				}
				else
					index = exitingIter->Index;

				meshPart->Indices.push_back(index);
			}
		}

		if (meshPart->Vertices.size())
		{
			meshPart->VertexFlags = meshPart->Vertices.front().Flags;

			mesh->Bound.Merge(meshPart->Bound);
			mesh->MeshParts.push_back(meshPart);
		}
	}

	mSceneMeshes.push_back(mesh);
}

shared_ptr<Skeleton> FbxProcesser::ProcessBoneWeights( FbxMesh* pMesh, std::vector<BoneWeights>& meshBoneWeights )
{
	shared_ptr<Skeleton> meshSkeleton = mFBXMeshSkeletonMap[pMesh].Skeleton;

	for( int i = 0; i < pMesh->GetDeformerCount(); ++i )
	{
		FbxDeformer* pFBXDeformer = pMesh->GetDeformer(i);

		if( !pFBXDeformer )
			continue;

		if( pFBXDeformer->GetDeformerType() == FbxDeformer::eSkin )
		{
			FbxSkin* pFBXSkin = (FbxSkin *)(pFBXDeformer);

			if( !pFBXSkin )
				continue;

			FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize; //Default link mode

			int nClusterCount = pFBXSkin->GetClusterCount();
			for( int i = 0; i < nClusterCount; ++i )
			{
				FbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);
				if( !pFBXCluster )
					continue;

				linkMode = pFBXCluster->GetLinkMode();
				FbxNode* pLinkNode = pFBXCluster->GetLink();

				if( !pLinkNode )
					continue;
				
				Bone* skeletonBone = meshSkeleton->GetBone(pLinkNode->GetName());
				assert(skeletonBone);

				int* indices = pFBXCluster->GetControlPointIndices();
				double* weights = pFBXCluster->GetControlPointWeights();

				for( int j = 0; j < pFBXCluster->GetControlPointIndicesCount(); ++j )
				{
					int ctlPointIndex = indices[j];
					meshBoneWeights[ctlPointIndex].AddBoneWeight(skeletonBone->GetBoneIndex(), (float)weights[j]);
				}
			}
		}
	}

	return meshSkeleton;
}

void FbxProcesser::ProcessAnimation( const String& clipName, FbxNode* pNode, double fFrameRate, double fStart, double fStop )
{
	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxNode* skeletonRoot = GetBoneRoot(pNode);
		
		if( mFBXBoneNodes.count(pNode) && mFBXBoneNodes[pNode])
		{
			AnimationClipData& clip = mAnimationClips[clipName];

			AnimationClipData::AnimationTrack track;
			track.Name = pNode->GetName();

			double fTime = 0;
			while( fTime <= fStop )
			{
				FbxTime takeTime;
				takeTime.SetSecondDouble(fTime);

				FbxAMatrix offset = GetGeometry(pNode);

				FbxAMatrix matAbsoluteTransform = GetGlobalPosition(pNode, takeTime);	
				FbxAMatrix matParentAbsoluteTransform = GetGlobalPosition(pNode->GetParent(), takeTime);
				FbxAMatrix matLocalTrasform = matParentAbsoluteTransform.Inverse() * matAbsoluteTransform;

				AnimationClipData::KeyFrame keyframe;

				float4x4 localTrasform = matrixFromFBX(matLocalTrasform);
				mFBXTransformer.TransformMatrix(&localTrasform, &localTrasform);
				MatrixDecompose(keyframe.Scale, keyframe.Rotation, keyframe.Translation, localTrasform);

				keyframe.Time = (float)fTime;
				track.KeyFrames.push_back(keyframe);

				fTime += 1.0f/fFrameRate;
			}

			clip.Duration = (float)(fTime-1.0f/fFrameRate);
			clip.mAnimationTracks.push_back(track);
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
		ProcessAnimation(clipName, pNode->GetChild(i), fFrameRate, fStart, fStop);
}

void FbxProcesser::CollectMaterials( )
{ 
	ExportLog::LogMsg(2, "Collect Materials.");

	for( int i = 0; i < mFBXScene->GetMaterialCount(); ++i )
	{
		FbxSurfaceMaterial* pSurfaceMaterial = mFBXScene->GetMaterial(i);

		MaterialData matData;
		matData.Name = pSurfaceMaterial->GetName();
		std::replace(matData.Name.begin(), matData.Name.end(), ':', '_');

		float fSpecularPower = 1.0f;
		float fTransparency = 1.0f;

		FbxProperty FBXEmissiveProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		FbxProperty FBXDiffuseProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		FbxProperty FBXSpecularProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		FbxProperty FBXSpecularFactorProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
		FbxProperty FBXShininessProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
		FbxProperty FBXNormalMapProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		FbxProperty FBXTransparentProperty = pSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
		
		auto bBXEmissiveProperty = FBXEmissiveProperty.IsValid(); 
		auto bBXDiffuseProperty = FBXDiffuseProperty.IsValid(); 
		auto bBXSpecularProperty = FBXEmissiveProperty.IsValid(); 
		auto bFBXSpecularFactorProperty = FBXEmissiveProperty.IsValid(); 
		auto bBXShininessProperty = FBXEmissiveProperty.IsValid(); 
		auto bBXNormalMapProperty = FBXEmissiveProperty.IsValid(); 
		auto bFBXTransparentProperty = FBXEmissiveProperty.IsValid(); 

		FbxSurfaceLambert* pLambert = FbxCast<FbxSurfaceLambert>(pSurfaceMaterial);
		FbxSurfacePhong* pPhong = FbxCast<FbxSurfacePhong>(pSurfaceMaterial);

		if( pLambert )
		{
			matData.Ambient = ReadMaterialColor(pLambert->Ambient, pLambert->AmbientFactor);
			matData.Emissive = ReadMaterialColor(pLambert->Emissive, pLambert->EmissiveFactor);
			matData.Diffuse = ReadMaterialColor(pLambert->Diffuse, pLambert->DiffuseFactor);

			FbxPropertyT<FbxDouble> FBXTransparencyProperty = pLambert->TransparencyFactor;
			if( FBXTransparencyProperty.IsValid() )
				fTransparency = static_cast<float>( FBXTransparencyProperty.Get() );
		}

		if( pPhong )
		{
			if( pPhong->Specular.IsValid() )
			{
				FbxDouble3 FBXColor = pPhong->Specular.Get();
				matData.Specular = float3((float)FBXColor[0], (float)FBXColor[1], (float)FBXColor[2]);
			}

			//matData.Specular = ReadMaterialColor(pPhong->Specular, pPhong->SpecularFactor);

			FbxPropertyT<FbxDouble> FBXSpecularPowerProperty = pPhong->Shininess;
			if( FBXSpecularPowerProperty.IsValid() )
				matData.Power = static_cast<float>( FBXSpecularPowerProperty.Get() );
		}

		// Textures
		for(int textureLayerIndex = 0 ; textureLayerIndex < FbxLayerElement::sTypeTextureCount ; ++textureLayerIndex)
		{  
			FbxProperty pProperty = pSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureLayerIndex]);  
			if(pProperty.IsValid())  
			{  
				int textureCount = pProperty.GetSrcObjectCount(FbxTexture::ClassId);  

				for(int tex = 0 ; tex < textureCount ; ++tex)  
				{  
					FbxTexture* pTexture = FbxCast<FbxTexture>(pProperty.GetSrcObject(FbxTexture::ClassId,tex));  
					if(pTexture)  
					{  
						FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(pTexture);
						if (pFileTexture)
						{
							 String filepath =  pFileTexture->GetFileName();
							 matData.Textures[FbxLayerElement::sTextureChannelNames[textureLayerIndex]] = filepath;
							 /*std::cout << FbxLayerElement::sTextureChannelNames[textureLayerIndex] << ": " << filepath << std::endl;*/
						}
					}  
				}  
			}  
		}  
		
		mMaterials.push_back(matData);
	}
}

void FbxProcesser::CollectAnimations( )
{
	ExportLog::LogMsg(0, "Collect Animations.");

	FbxNode* pRootNode = mFBXScene->GetRootNode();

	if (!pRootNode) return;

	double frameRate = FbxTime::GetFrameRate(mFBXScene->GetGlobalSettings().GetTimeMode());

	//FbxTime mFrameTime;
	//mFrameTime.SetTime(0, 0, 0, 1, 0, mFBXScene->GetGlobalSettings().GetTimeMode());

	for (int i = 0; i < mFBXScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(i);
		
		int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();

		FbxString takeName = lAnimStack->GetName();

		if (takeName != FbxString("Default"))
		{
			FbxTakeInfo* lCurrentTakeInfo = mFBXScene->GetTakeInfo(takeName);

			//mFBXScene->ActiveAnimStackName.Set(takeName);
			//mFBXSdkManager->GetAnimationEvaluator()->SetContext(lAnimStack);
			mFBXScene->SetCurrentAnimationStack(lAnimStack);

			FbxTime KStart, KStop;

			if (lCurrentTakeInfo)
			{
				KStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
				KStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
			}
			else
			{
				// Take the time line value
				FbxTimeSpan lTimeLineTimeSpan;
				mFBXScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

				KStart = lTimeLineTimeSpan.GetStart();
				KStop  = lTimeLineTimeSpan.GetStop();
			}

			double fStart = KStart.GetSecondDouble();
			double fStop = KStop.GetSecondDouble();

			if( fStart < fStop )
			{
				ExportLog::LogMsg(0, "Process Animation Stack %s.\n", lAnimStack->GetName());
				ProcessAnimation(lAnimStack->GetName(), pRootNode, frameRate, fStart, fStop);
			}
		}
	}
}

void FbxProcesser::MergeSceneMeshs()
{
	if (mSceneMeshes.size() > 1)
	{
		if (mFBXMeshSkeletonMap.size() && g_ExportSettings.ExportSkeleton)
		{
			ExportLog::LogWarning("Found mesh with skeleton, can't merge!");
			return;
		}

		// Make other mesh as a mesh part of first mesh
		for (size_t mi = 1; mi < mSceneMeshes.size(); ++mi)
		{
			MeshData& mesh  = *(mSceneMeshes[mi]);

			mSceneMeshes[0]->Bound.Merge(mesh.Bound);

			for (auto& part : mesh.MeshParts)
				mSceneMeshes[0]->MeshParts.push_back(part);
		}

		mSceneMeshes.resize(1);	
	}	
}

void FbxProcesser::MergeMeshParts()
{
	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *mSceneMeshes[mi];

		if (mesh.MeshParts.empty()) continue;

		// Sort mesh part based on vertex format and material
		sort(mesh.MeshParts.begin(), mesh.MeshParts.end(),
			[&](const shared_ptr<MeshPartData>& part1, const shared_ptr<MeshPartData>& part2) {
			
				if (part1->VertexFlags < part2->VertexFlags)
					return true;
				else if (part1->VertexFlags > part2->VertexFlags)
					return false;
				else
					return part1->MaterialName < part2->MaterialName;
		});	

		if (g_ExportSettings.MergeWithSameMaterial)
		{
			// Merge mesh parts with same material
			auto destMergeIt = mesh.MeshParts.begin();
			while (destMergeIt != mesh.MeshParts.end())
			{
				shared_ptr<MeshPartData> destMergePart = *destMergeIt;

				auto srcMergeIt = destMergeIt + 1;
				while (srcMergeIt != mesh.MeshParts.end())
				{
					shared_ptr<MeshPartData> srcMergePart = *srcMergeIt;

					bool canMerge = (srcMergePart->VertexFlags == destMergePart->VertexFlags);
					canMerge &= (srcMergePart->MaterialName == destMergePart->MaterialName);

					if (srcMergePart->Vertices.size() > UINT16_MAX || destMergePart->Vertices.size() > UINT16_MAX)
					{
						canMerge &= (srcMergePart->Vertices.size() + destMergePart->Vertices.size() < UINT32_MAX);
					}
					else
					{
						canMerge &= (srcMergePart->Vertices.size() + destMergePart->Vertices.size() < UINT16_MAX);
					}

					if (canMerge)
					{
						size_t baseIndex = destMergePart->Vertices.size();

						destMergePart->Vertices.reserve(destMergePart->Vertices.size() + srcMergePart->Vertices.size());
						destMergePart->Indices.reserve(destMergePart->Indices.size() + srcMergePart->Indices.size());
						for (Vertex& vertex : srcMergePart->Vertices)
						{
							vertex.Index += baseIndex;
							destMergePart->Vertices.push_back(vertex);
						}

						for (const uint32_t& index : srcMergePart->Indices)
							destMergePart->Indices.push_back(baseIndex + index);

						destMergePart->Bound.Merge(srcMergePart->Bound);

						srcMergeIt = mesh.MeshParts.erase(srcMergeIt);
					}
					else 
						++srcMergeIt;
				}

				++destMergeIt;
			}
		}

		// Allocation enough buffer to hold all VB&IB,  std::vector reallocation is very expensive in this case
		mesh.Vertices.reserve(10);
		mesh.Indices.reserve(10);

		// Combine mesh part's VS and IB into buffers
		auto destMergeIt = mesh.MeshParts.begin();
		while (destMergeIt != mesh.MeshParts.end())
		{
			shared_ptr<MeshPartData> destMergePart = *destMergeIt;

			mesh.Vertices.resize(mesh.Vertices.size() + 1);		
			mesh.Vertices.back().reserve(2000);

			uint32_t dstVertexBufferIndex = mesh.Vertices.size() - 1;
			uint32_t dstIndexBufferIndex;

			auto srcMergeIt = destMergeIt;
			while (srcMergeIt != mesh.MeshParts.end())
			{
				shared_ptr<MeshPartData> srcMergePart = *srcMergeIt;
				printf("Combine MeshPart: %s\n", srcMergePart->Name.c_str());
				if (srcMergePart->VertexFlags == destMergePart->VertexFlags || srcMergeIt == destMergeIt)
				{
					srcMergePart->BaseVertex = mesh.Vertices[dstVertexBufferIndex].size();

					for (Vertex& vertex : srcMergePart->Vertices)
					{
						//vertex.Index += baseVertex;
						mesh.Vertices[dstVertexBufferIndex].push_back(vertex);
					}

					IndexBufferType indexType = (srcMergePart->Vertices.size() < UINT16_MAX) ? IBT_Bit16 : IBT_Bit32;

					dstIndexBufferIndex = UINT32_MAX;
					for (size_t i = 0; i < mesh.Indices.size(); ++i)
					{
						if (mesh.IndexTypes[i] == indexType)
						{
							dstIndexBufferIndex = i;
							break;
						}
					}

					if (dstIndexBufferIndex == UINT32_MAX)
					{
						mesh.Indices.resize(mesh.Indices.size() + 1);	
						mesh.Indices.back().reserve(2000);
						mesh.IndexTypes.resize(mesh.IndexTypes.size() + 1, indexType);

						dstIndexBufferIndex = mesh.Indices.size() - 1;
					}

					srcMergePart->StartIndex = mesh.Indices[dstIndexBufferIndex].size();
					srcMergePart->IndexCount =  srcMergePart->Indices.size();
					for (const uint32_t& index : srcMergePart->Indices)
						mesh.Indices[dstIndexBufferIndex].push_back(index);	

					srcMergePart->VertexBufferIndex = dstVertexBufferIndex;
					srcMergePart->IndexBufferIndex = dstIndexBufferIndex;
					
					// Advance iteration
					++srcMergeIt;
				}
				else
					break;
			}

			destMergeIt = srcMergeIt;
		}
	}
}

void FbxProcesser::ExportMaterial()
{
	XMLDoc materialxml;

	XMLNodePtr rootNode = materialxml.AllocateNode(XML_Node_Element, "materials");
	materialxml.RootNode(rootNode);

	rootNode->AppendAttribute(materialxml.AllocateAttributeUInt("count", mMaterials.size()));

	XMLNodePtr matNode, tmpNode;
	XMLAttributePtr attrib;
	for (size_t i = 0; i < mMaterials.size(); ++i)
	{
		matNode = materialxml.AllocateNode(XML_Node_Element, "material");
		matNode->AppendAttribute(materialxml.AllocateAttributeString("material", mMaterials[i].Name));

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "ambient");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Ambient[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Ambient[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Ambient[2]));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "diffuse");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Diffuse[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Diffuse[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Diffuse[2]));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "specular");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Specular[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Specular[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Specular[2]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("power", mMaterials[i].Power));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "emissive");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Emissive[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Emissive[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Emissive[2]));
		matNode->AppendNode(tmpNode);

		for (auto iter = mMaterials[i].Textures.begin(); iter != mMaterials[i].Textures.end(); ++iter)
		{
			tmpNode = materialxml.AllocateNode(XML_Node_Element, iter->first);
			tmpNode->AppendAttribute(materialxml.AllocateAttributeString("name", iter->second));
			matNode->AppendNode(tmpNode);
		}	

		rootNode->AppendNode(matNode);
	}

	std::ofstream xmlFile(mOutputPath + mSceneName + ".materials.xml");
	materialxml.Print(xmlFile);
	xmlFile.close();
}

//void FbxProcesser::BuildAndSaveXML( )
//{
//	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
//	{
//		MeshData& mesh  = *(mSceneMeshes[mi]);
//
//		if(!mQuietMode)
//		{
//			FBXSDK_printf("output xml mesh: %s.\n", mesh.Name.c_str());
//		}
//
//		XMLDoc meshxml;
//
//		XMLNodePtr meshNode = meshxml.AllocateNode(XML_Node_Element, "mesh");
//		meshxml.RootNode(meshNode);
//
//		// mesh name
//		meshNode->AppendAttribute(meshxml.AllocateAttributeString("name", mesh.Name));
//
//		// mesh bounding
//		XMLNodePtr meshBoundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");
//
//		XMLNodePtr meshBoundMinNode = meshxml.AllocateNode(XML_Node_Element, "min");
//		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", mesh.Bound.Min.X()));
//		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", mesh.Bound.Min.Y()));
//		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", mesh.Bound.Min.Z()));
//		meshBoundNode->AppendNode(meshBoundMinNode);
//
//		XMLNodePtr meshBoundMaxNode = meshxml.AllocateNode(XML_Node_Element, "max");
//		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", mesh.Bound.Max.X()));
//		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", mesh.Bound.Max.Y()));
//		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", mesh.Bound.Max.Z()));
//		meshBoundNode->AppendNode(meshBoundMaxNode);
//
//		meshNode->AppendNode(meshBoundNode);
//
//		// mesh skeleton
//		if (g_ExportSettings.ExportSkeleton && mesh.Skeleton)
//		{
//			XMLNodePtr meshSkeletonNode = meshxml.AllocateNode(XML_Node_Element, "skeleton");
//			meshSkeletonNode->AppendAttribute(meshxml.AllocateAttributeUInt("boneCount", mesh.Skeleton->GetNumBones()));
//			for (size_t iBone = 0; iBone < mesh.Skeleton->GetNumBones(); ++iBone)
//			{
//				Bone* bone = mesh.Skeleton->GetBone(iBone);
//				Bone* parentBone = static_cast<Bone*>(bone->GetParent());
//
//				XMLNodePtr boneNode = meshxml.AllocateNode(XML_Node_Element, "bone");
//				boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", bone->GetName()));
//				boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", parentBone ? parentBone->GetName() : ""));
//				meshSkeletonNode->AppendNode(boneNode);
//
//				float3 pos = bone->GetPosition();
//				float3 scale = bone->GetScale();
//				Quaternionf rot = bone->GetRotation();
//
//				XMLNodePtr posNode = meshxml.AllocateNode(XML_Node_Element, "bindPosition");
//				posNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", pos.X()));
//				posNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", pos.Y()));
//				posNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", pos.Z()));
//				boneNode->AppendNode(posNode);
//
//				XMLNodePtr rotNode = meshxml.AllocateNode(XML_Node_Element, "bindRotation");
//				rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("w", rot.W()));
//				rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", rot.X()));
//				rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", rot.Y()));
//				rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", rot.Z()));
//				boneNode->AppendNode(rotNode);
//
//				XMLNodePtr scaleNode = meshxml.AllocateNode(XML_Node_Element, "bindScale");
//				scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", scale.X()));
//				scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", scale.Y()));
//				scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", scale.Z()));
//				boneNode->AppendNode(scaleNode);
//			}
//			meshNode->AppendNode(meshSkeletonNode);
//		}
//
//		for (size_t mpi = 0; mpi < mesh.MeshParts.size(); ++mpi)
//		{
//			const shared_ptr<MeshPartData>& meshPart = mesh.MeshParts[mpi];
//
//			XMLNodePtr meshPartNode = meshxml.AllocateNode(XML_Node_Element, "meshPart");
//			XMLAttributePtr meshPartNameAttr = meshxml.AllocateAttributeString("name", meshPart->Name);
//			XMLAttributePtr meshPartMatAttr = meshxml.AllocateAttributeString("material", meshPart->MaterialName);
//			meshPartNode->AppendAttribute(meshPartNameAttr);
//			meshPartNode->AppendAttribute(meshPartMatAttr);
//			// add to mesh
//			meshNode->AppendNode(meshPartNode);
//
//			//  bounding
//			XMLNodePtr boundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");
//
//			XMLNodePtr boundMinNode = meshxml.AllocateNode(XML_Node_Element, "min");
//			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", meshPart->Bound.Min.X()));
//			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", meshPart->Bound.Min.Y()));
//			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", meshPart->Bound.Min.Z()));
//			boundNode->AppendNode(boundMinNode);
//
//			XMLNodePtr boundMaxNode = meshxml.AllocateNode(XML_Node_Element, "max");
//			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", meshPart->Bound.Max.X()));
//			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", meshPart->Bound.Max.Y()));
//			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", meshPart->Bound.Max.Z()));
//			boundNode->AppendNode(boundMaxNode);
//
//			meshPartNode->AppendNode(boundNode);
//
//			XMLNodePtr verticesNode = meshxml.AllocateNode(XML_Node_Element, "vertices");
//			XMLAttributePtr verticesCountAttr = meshxml.AllocateAttributeUInt("verticesCount", meshPart->Vertices.size());
//			XMLAttributePtr vertexSizeAttr = meshxml.AllocateAttributeUInt("vertexSize", CalculateVertexSize(meshPart->Vertices[0].Flags));
//			verticesNode->AppendAttribute(verticesCountAttr);
//			verticesNode->AppendAttribute(vertexSizeAttr);		
//			// add to mesh part
//			meshPartNode->AppendNode(verticesNode);
//
//			for(const Vertex& vertex : meshPart->Vertices)
//			{
//				XMLNodePtr vertexNode = meshxml.AllocateNode(XML_Node_Element, "vertex");
//				verticesNode->AppendNode(vertexNode);
//
//				uint32_t vertexFlag = vertex.Flags;
//				if (vertexFlag & Vertex::ePosition)
//				{
//					XMLNodePtr positionNode = meshxml.AllocateNode(XML_Node_Element, "position");
//					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Position.X()));
//					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Position.Y()));
//					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Position.Z()));
//					vertexNode->AppendNode(positionNode);
//				}
//
//				if (vertexFlag & Vertex::eNormal)
//				{
//					XMLNodePtr normalNode = meshxml.AllocateNode(XML_Node_Element, "normal");
//					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Normal.X()));
//					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Normal.Y()));
//					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Normal.Z()));
//					vertexNode->AppendNode(normalNode);
//				}
//
//				if (vertexFlag & Vertex::eTexcoord0)
//				{
//					XMLNodePtr texcoordNode = meshxml.AllocateNode(XML_Node_Element, "texcoord");
//					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("u", vertex.Tex0.X()));
//					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("v", vertex.Tex0.Y()));
//					vertexNode->AppendNode(texcoordNode);
//				}
//
//				if (vertexFlag & Vertex::eTexcoord1)
//				{
//					XMLNodePtr texcoordNode = meshxml.AllocateNode(XML_Node_Element, "texcoord");
//					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("u", vertex.Tex1.X()));
//					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("v", vertex.Tex1.Y()));
//					vertexNode->AppendNode(texcoordNode);
//				}
//
//				if (vertexFlag & Vertex::eTangent)
//				{
//					XMLNodePtr tangentNode = meshxml.AllocateNode(XML_Node_Element, "tangent");
//					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Tangent.X()));
//					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Tangent.Y()));
//					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Tangent.Z()));
//					vertexNode->AppendNode(tangentNode);
//				}
//
//				if (vertexFlag & Vertex::eBinormal)
//				{
//					XMLNodePtr binormalNode = meshxml.AllocateNode(XML_Node_Element, "binormal");
//					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Binormal.X()));
//					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Binormal.Y()));
//					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Binormal.Z()));
//					vertexNode->AppendNode(binormalNode);
//				}
//
//				if (vertexFlag & Vertex::eBlendWeight)
//				{
//					for (size_t ib = 0; ib < 4; ++ib)
//					{
//						XMLNodePtr boneNode = meshxml.AllocateNode(XML_Node_Element, "bone");
//						boneNode->AppendAttribute(meshxml.AllocateAttributeUInt("index", vertex.BlendIndices[ib]));
//						boneNode->AppendAttribute(meshxml.AllocateAttributeFloat("weight", vertex.BlendWeights[ib]));
//						vertexNode->AppendNode(boneNode);
//					}
//				}
//			}
//
//
//			size_t triangleCount = meshPart->Indices.size() / 3;
//			XMLNodePtr trianglesNode = meshxml.AllocateNode(XML_Node_Element, "triangles");
//			trianglesNode->AppendAttribute(meshxml.AllocateAttributeUInt("triangleCount", triangleCount));
//
//			// add to mesh part
//			meshPartNode->AppendNode(trianglesNode);
//
//			for (size_t tri = 0; tri < triangleCount; ++tri)
//			{
//				XMLNodePtr triangleNode = meshxml.AllocateNode(XML_Node_Element, "triangle");
//				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("a", meshPart->Indices[3*tri]));
//				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("b", meshPart->Indices[3*tri+1]));
//				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("c", meshPart->Indices[3*tri+2]));
//				trianglesNode->AppendNode(triangleNode);
//			}			
//		}
//
//		if (g_ExportSettings.ExportAnimation && mesh.Skeleton)
//		{
//			XMLNodePtr animationNode = meshxml.AllocateNode(XML_Node_Element, "animation");
//
//			
//			animationNode->AppendAttribute(meshxml.AllocateAttributeUInt("clipCount", mAnimations.size()));
//			meshNode->AppendNode(animationNode);
//
//			for (auto iter = mAnimations.begin(); iter != mAnimations.end(); ++iter)
//			{
//				AnimationClipData& clipData = iter->second;
//
//				XMLNodePtr clipNode = meshxml.AllocateNode(XML_Node_Element, "clip");
//				clipNode->AppendAttribute(meshxml.AllocateAttributeString("name", iter->first));
//				clipNode->AppendAttribute(meshxml.AllocateAttributeUInt("track", clipData.mAnimationTracks.size()));
//				animationNode->AppendNode(clipNode);
//
//				for (size_t iTrack = 0; iTrack < clipData.mAnimationTracks.size(); ++iTrack)
//				{
//					AnimationClipData::AnimationTrack& track =  clipData.mAnimationTracks[iTrack];
//
//					XMLNodePtr trackNode = meshxml.AllocateNode(XML_Node_Element, "track");
//					trackNode->AppendAttribute(meshxml.AllocateAttributeString("bone", track.Name));
//					trackNode->AppendAttribute(meshxml.AllocateAttributeUInt("keys", track.KeyFrames.size()));
//					clipNode->AppendNode(trackNode);
//
//					for (size_t iKey = 0; iKey < track.KeyFrames.size(); ++iKey)
//					{
//						XMLNodePtr keyNode = meshxml.AllocateNode(XML_Node_Element, "keyframe");
//						keyNode->AppendAttribute(meshxml.AllocateAttributeFloat("time", track.KeyFrames[iKey].Time));
//						trackNode->AppendNode(keyNode);
//
//						float3 pos = track.KeyFrames[iKey].Translation;
//						float3 scale = track.KeyFrames[iKey].Scale;
//						Quaternionf rot = track.KeyFrames[iKey].Rotation;
//
//						XMLNodePtr posNode = meshxml.AllocateNode(XML_Node_Element, "position");
//						posNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", pos.X()));
//						posNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", pos.Y()));
//						posNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", pos.Z()));
//						keyNode->AppendNode(posNode);
//
//						XMLNodePtr rotNode = meshxml.AllocateNode(XML_Node_Element, "rotation");
//						rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("w", rot.W()));
//						rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", rot.X()));
//						rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", rot.Y()));
//						rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", rot.Z()));
//						keyNode->AppendNode(rotNode);
//
//						XMLNodePtr scaleNode = meshxml.AllocateNode(XML_Node_Element, "scale");
//						scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", scale.X()));
//						scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", scale.Y()));
//						scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", scale.Z()));
//						keyNode->AppendNode(scaleNode);
//					}
//				}
//			}
//		}
//
//		std::ofstream xmlFile(mOutputPath + mSceneName + ".mesh.xml");
//		meshxml.Print(xmlFile);
//		xmlFile.close();
//	}	
//
//	ExportMaterial();
//}

void FbxProcesser::BuildAndSaveBinary( )
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		FileStream stream;
		stream.Open(mOutputPath + mesh.Name + ".mesh", FILE_WRITE);
		ExportLog::LogMsg(0, "Build mesh: %s\n", mesh.Name.c_str());

		// Write mesh id
		stream.WriteUInt(MeshId);

		// write mesh name
		stream.WriteString(mesh.Name);

		// write mesh bounding box
		stream.Write(&mesh.Bound.Min, sizeof(float3));
		stream.Write(&mesh.Bound.Max, sizeof(float3));

		// write mesh part count
		stream.WriteUInt( mesh.MeshParts.size() );
		stream.WriteUInt( (g_ExportSettings.ExportSkeleton && mesh.Skeleton) ? mesh.Skeleton->GetNumBones() : 0 );
		stream.WriteUInt( mesh.Vertices.size() );
		stream.WriteUInt( mesh.Indices.size() );
			
		// Write mesh part 
		for (size_t mpi = 0; mpi < mesh.MeshParts.size(); ++mpi)
		{
			const shared_ptr<MeshPartData>& meshPart = mesh.MeshParts[mpi];

			// write sub mesh name
			stream.WriteString(meshPart->Name);	

			// write material name
			stream.WriteString(meshPart->MaterialName + ".material.xml");

			// write sub mesh bounding sphere
			stream.Write(&meshPart->Bound.Min, sizeof(float3));
			stream.Write(&meshPart->Bound.Max, sizeof(float3));

			stream.WriteUInt(meshPart->VertexBufferIndex);
			stream.WriteUInt(meshPart->IndexBufferIndex);

			// write vertex count and vertex size
			stream.WriteUInt(meshPart->StartIndex);
			stream.WriteUInt(meshPart->IndexCount);
			stream.WriteInt(meshPart->BaseVertex);
		}

		// Write skeleton
		if (g_ExportSettings.ExportSkeleton && mesh.Skeleton)
		{
			// write bone count
			for (size_t iBone = 0; iBone < mesh.Skeleton->GetNumBones(); ++iBone)
			{
				Bone* bone = mesh.Skeleton->GetBone(iBone);
				Bone* parentBone = static_cast<Bone*>(bone->GetParent());

				float3 pos = bone->GetPosition();
				float3 scale = bone->GetScale();
				Quaternionf rot = bone->GetRotation();

				stream.WriteString(bone->GetName());
				stream.WriteInt(parentBone ? parentBone->GetBoneIndex() : -1);

				stream.Write(&pos, sizeof(float3));
				stream.Write(&rot, sizeof(Quaternionf));
				stream.Write(&scale, sizeof(float3));
			}
		}

		// Write vertex and index buffer
		for (size_t i = 0; i < mesh.Vertices.size(); ++i)
		{
			uint32_t vertexSize;
			std::vector<VertexElement> vertexElements;
			GetVertexDeclaration(mesh.Vertices[i].front().Flags, vertexElements, vertexSize);

			stream.WriteUInt(mesh.Vertices[i].size()); // Vertex Count
			stream.WriteUInt(vertexElements.size());   // Vertex Size
			for (const VertexElement& ve : vertexElements)
			{
				stream.WriteUInt(ve.Offset);
				stream.WriteUInt(ve.Type);
				stream.WriteUInt(ve.Usage);
				stream.WriteUShort(ve.UsageIndex);
			}

			for (const Vertex& vertex : mesh.Vertices[i])
			{
				uint32_t vertexFlag = vertex.Flags;

				if (vertexFlag & Vertex::ePosition)
					stream.Write(&vertex.Position, sizeof(float3));

				if (vertexFlag & Vertex::eBlendWeight)
				{
					assert(vertex.BlendWeights.size() == 4);
					stream.Write(&vertex.BlendWeights[0], sizeof(float) * 4);
				}

				if (vertexFlag & Vertex::eBlendIndices)
				{
					assert(vertex.BlendIndices.size() == 4);
					stream.WriteUInt(vertex.BlendIndices[0]);
					stream.WriteUInt(vertex.BlendIndices[1]);
					stream.WriteUInt(vertex.BlendIndices[2]);
					stream.WriteUInt(vertex.BlendIndices[3]);
				}

				if (vertexFlag & Vertex::eNormal)
					stream.Write(&vertex.Normal, sizeof(float3));

				if (vertexFlag & Vertex::eTexcoord0)
					stream.Write(&vertex.Tex0, sizeof(float2));

				if (vertexFlag & Vertex::eTexcoord1)
					stream.Write(&vertex.Tex1, sizeof(float2));

				if (vertexFlag & Vertex::eTangent)
					stream.Write(&vertex.Tangent, sizeof(float3));

				if (vertexFlag & Vertex::eBinormal)
					stream.Write(&vertex.Binormal, sizeof(float3));		
			}
		}

		for (size_t i = 0; i < mesh.Indices.size(); ++i)
		{
			if (mesh.IndexTypes[i] == IBT_Bit16)
			{
				stream.WriteUInt(mesh.Indices[i].size());
				stream.WriteUInt(IBT_Bit16);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t j = 0; j < mesh.Indices[i].size() / 3; ++j)
					{
						stream.WriteUShort(mesh.Indices[i][3*j+0]);
						stream.WriteUShort(mesh.Indices[i][3*j+2]);
						stream.WriteUShort(mesh.Indices[i][3*j+1]);
					}
				}
				else
				{
					for (size_t j = 0; j < mesh.Indices[i].size(); ++j)
						stream.WriteUShort(mesh.Indices[i][j]);
				}
			}
			else
			{
				stream.WriteUInt(mesh.Indices[i].size());
				stream.WriteUInt(IBT_Bit32);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t j = 0; j < mesh.Indices[i].size() / 3; ++j)
					{
						stream.WriteUInt(mesh.Indices[i][3*j+0]);
						stream.WriteUInt(mesh.Indices[i][3*j+2]);
						stream.WriteUInt(mesh.Indices[i][3*j+1]);
					}
				}
				else
				{
					stream.Write(&mesh.Indices[i], sizeof(uint32_t) * mesh.Indices[i].size());
				}
			}
		}

		stream.Close();
	}

	// Write animation clips
	if (g_ExportSettings.ExportAnimation)
	{
		for (const auto& kv : mAnimationClips)
		{
			String clipName = kv.first + ".anim";
			const AnimationClipData& clip = kv.second;

			FileStream clipStream;
			clipStream.Open(mOutputPath + clipName, FILE_WRITE);

			clipStream.WriteFloat(clip.Duration);
			clipStream.WriteUInt(clip.mAnimationTracks.size());

			for (const AnimationClipData::AnimationTrack& track : clip.mAnimationTracks)
			{
				// write track name
				clipStream.WriteString(track.Name);
				// write track key frame count
				clipStream.WriteUInt(track.KeyFrames.size());

				for (const AnimationClipData::KeyFrame& key : track.KeyFrames)
				{
					// write key time
					clipStream.WriteFloat(key.Time);
					clipStream.Write(&key.Translation, sizeof(float3));
					clipStream.Write(&key.Rotation, sizeof(Quaternionf));
					clipStream.Write(&key.Scale, sizeof(float3));
				}
			}

			clipStream.Close();
		}
	}
}

void FbxProcesser::BuildAndSaveMaterial()
{
	char buffer[256];

	for (size_t i = 0; i < mMaterials.size(); ++i)
	{
		XMLDoc materialXML;

		XMLNodePtr rootNode = materialXML.AllocateNode(XML_Node_Element, "material");
		materialXML.RootNode(rootNode);

		rootNode->AppendAttribute(materialXML.AllocateAttributeString("name", mMaterials[i].Name));


		XMLNodePtr paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "WorldView"));
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "WorldViewMatrix"));
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "float4x4"));
		rootNode->AppendNode(paramNode);

		paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "Proj"));
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "ProjectionMatrix"));
		paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "float4x4"));
		rootNode->AppendNode(paramNode);

		uint32_t texUnit = 0;

		XMLNodePtr renderQueueNode = materialXML.AllocateNode(XML_Node_Element, "Queue");
		XMLNodePtr effectNode = materialXML.AllocateNode(XML_Node_Element, "Effect");
		
		if (mMaterials[i].Textures.count("DiffuseColor"))
		{
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "DiffuseMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("sampler", "LinearSampler"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "DiffuseMaterialMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("stage", "PixelShader"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeUInt("texUnit", texUnit++));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "texture2d"));

			String filename = PathUtil::GetFileName(mMaterials[i].Textures["DiffuseColor"]) + ".dds";
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", filename));

			rootNode->AppendNode(paramNode);
			
			// Add effect flag
			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_DiffuseMap"));
			effectNode->AppendNode(flagNode);
		}
		else
		{
			std::sprintf(buffer, "%f %f %f", mMaterials[i].Diffuse[0], mMaterials[i].Diffuse[1], mMaterials[i].Diffuse[2]);
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "DiffuseColor"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "DiffuseMaterialColor"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "float3"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", buffer));
			rootNode->AppendNode(paramNode);
		}

		if (mMaterials[i].Textures.count("SpecularColor"))
		{
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "SpecularMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("sampler", "LinearSampler"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "SpecularMaterialMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("stage", "PixelShader"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeUInt("texUnit", texUnit++));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "texture2d"));

			String filename = PathUtil::GetFileName(mMaterials[i].Textures["SpecularColor"]) + ".dds";
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", filename));

			rootNode->AppendNode(paramNode);

			// Add effect flag
			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_SpecularMap"));
			effectNode->AppendNode(flagNode);
		}
		else
		{
			std::sprintf(buffer, "%f %f %f", mMaterials[i].Specular[0], mMaterials[i].Specular[1], mMaterials[i].Specular[2]);
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "SpecularColor"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "SpecularMaterialColor"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "float3"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", buffer));
			rootNode->AppendNode(paramNode);
		}

		if (mMaterials[i].Textures.count("ShininessExponent"))
		{
			assert( mMaterials[i].Textures.count("SpecularColor") > 0 );
			
			//Specular Sharpness/Power/Roughness
			// Bake Gloss map into the alpha channel of your specular map 

			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_GlossyMap"));
			effectNode->AppendNode(flagNode);
		}
		else
		{
			std::sprintf(buffer, "%f %f %f", mMaterials[i].Specular[0], mMaterials[i].Specular[1], mMaterials[i].Specular[2]);
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "Shininess"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "SpecularMaterialPower"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "float"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", std::to_string(mMaterials[i].Power)));
			rootNode->AppendNode(paramNode);
		}

		if (mMaterials[i].Textures.count("NormalMap"))
		{
			paramNode = materialXML.AllocateNode(XML_Node_Element, "Parameter");
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("name", "NormalMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("sampler", "LinearSampler"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("semantic", "NormalMaterialMap"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("stage", "PixelShader"));
			paramNode->AppendAttribute(materialXML.AllocateAttributeUInt("texUnit", texUnit++));
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("type", "texture2d"));

			String filename = PathUtil::GetFileName(mMaterials[i].Textures["NormalMap"]) + ".dds";
			paramNode->AppendAttribute(materialXML.AllocateAttributeString("value", filename));

			rootNode->AppendNode(paramNode);

			// Add effect flag
			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_NormalMap"));
			effectNode->AppendNode(flagNode);
		}

		if (mMaterials[i].Textures.count("TransparentColor"))
		{
			assert( mMaterials[i].Textures.count("DiffuseColor") > 0 );

			// Bake transparent into the alpha channel of your diffuse map 

			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_AlphaTest"));
			effectNode->AppendNode(flagNode);

			effectNode->AppendAttribute(materialXML.AllocateAttributeString("name", "AlphaTest.effect.xml"));
		}
		else
		{
			effectNode->AppendAttribute(materialXML.AllocateAttributeString("name", "Model.effect.xml"));
		}

		if (mFBXMeshSkeletonMap.size())
		{
			XMLNodePtr flagNode = materialXML.AllocateNode(XML_Node_Element, "Flag");
			flagNode->AppendAttribute(materialXML.AllocateAttributeString("name", "_Skinning"));
			effectNode->AppendNode(flagNode);
		}

		rootNode->AppendNode(effectNode);

		renderQueueNode->AppendAttribute(materialXML.AllocateAttributeString("name", "Opaque"));
		rootNode->AppendNode(renderQueueNode);
		
		String outputPath = mOutputPath + mMaterials[i].Name +  ".material.xml";
		
		std::ofstream xmlFile(outputPath);
		materialXML.Print(xmlFile);
		xmlFile.close();
	}
}

void FbxProcesser::RunCommand( const vector<String>& arguments )
{

}
int main()
{
	ExportLog::AddListener( &g_ConsoleOutListener );
#if _MSC_VER >= 1500
	if( IsDebuggerPresent() )
		ExportLog::AddListener( &g_DebugSpewListener );
#endif

	ExportLog::SetLogLevel( 1 );
	ExportLog::EnableLogging( TRUE );

	//g_ExportSettings.ExportSkeleton = false;
	//g_ExportSettings.MergeScene = true;
	g_ExportSettings.MergeWithSameMaterial = true;
	g_ExportSettings.MergeScene = true;
	//g_ExportSettings.SwapWindOrder = false;

	FbxProcesser fbxProcesser;
	fbxProcesser.Initialize();

	if (fbxProcesser.LoadScene("../../Media/Mesh/AvatarAnimPack/run.fbx"))
	{
		fbxProcesser.mSceneName = "Ahri";
		fbxProcesser.mAnimationName = "Dance";

		fbxProcesser.ProcessScene();
		//fbxProcesser.BuildAndSaveXML();
 		fbxProcesser.BuildAndSaveBinary();
		fbxProcesser.BuildAndSaveMaterial();
		fbxProcesser.ExportMaterial();
	}

	return 0;
}

