#pragma once

#include <fbxsdk.h>
#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Skeleton.h>
#include <Graphics/VertexDeclaration.h>
#include <Math/MathUtil.h>
#include <Math/ColorRGBA.h>
#include <IO/FileStream.h>

using namespace RcEngine;

// Global Export Settings
struct ExportSettings
{
	bool ExportSkeleton;
	bool ExportAnimation; 
	bool MergeScene;
	bool MergeWithSameMaterial; // Merge sub mesh with same material
	bool SwapWindOrder;

	ExportSettings()
		: SwapWindOrder(true),
		  ExportSkeleton(true),
		  ExportAnimation(true),
		  MergeScene(false),
		  MergeWithSameMaterial(false)
	{}
};

extern ExportSettings g_ExportSettings;

// Coordinate System Transformer, Right-Handed -> Left Handed.
class FBXTransformer
{
public:
	FBXTransformer() : mMaxConversion(false) {}

	void Initialize( FbxScene* pScene );

	void TransformMatrix( float4x4* pDestMatrix, const float4x4* pSrcMatrix ) const;
	void TransformPosition( float3* pDestPosition, const float3* pSrcPosition ) const;
	void TransformDirection( float3* pDestDirection, const float3* pSrcDirection ) const;

protected:
	bool  mMaxConversion; // Convert Z-Up to Y-Up
};

// Material Template
struct MaterialData
{
	String Name;
	float3 Ambient;
	float3 Diffuse;
	float3 Specular;
	float3 Emissive;
	float Power;

	unordered_map<String, String> Textures;
};

class BoneWeights
{
public:
	void AddBoneWeight(int nBoneIndex, float fBoneWeight);
	void Validate();
	void Normalize();

	inline const std::vector<std::pair<int, float>>& GetBoneWeights() const { return mBoneWeights; }

private:
	std::vector<std::pair<int, float> > mBoneWeights;
};

// Model Vertex Structure
struct Vertex
{
	enum Flags
	{
		ePosition		= 1 << 0,
		eBlendWeight	= 1 << 1,
		eBlendIndices	= 1 << 2,
		eNormal			= 1 << 3,
		eTexcoord0		= 1 << 4,
		eTexcoord1		= 1 << 5,
		eTangent		= 1 << 6,
		eBinormal		= 1 << 7,	
	};

	friend bool operator< (const Vertex& lhs, const Vertex& rhs)
	{
#define CMP(value, size)							\
	for (int i = 0; i < 3; ++i) {				    \
	if (lhs.value[i] < rhs.value[i]) return true;   \
	if (rhs.value[i] < lhs.value[i]) return false;  \
	}
		CMP(Position, 3);
		CMP(Normal, 3);
		CMP(Tex0, 2);
#undef CMP

		return false;
	}

	float3 Position;
	float3 Normal;
	float3 Binormal;
	float3 Tangent;
	vector<uint32_t> BlendIndices;
	vector<float> BlendWeights;
	float2 Tex0;
	float2 Tex1;

	uint32_t Flags;
	size_t Index;	// keep track index in vertices

	Vertex() : Flags(0) {}
};

// Mesh Part Structure
struct MeshPartData
{
	String Name;
	String MaterialName;

	BoundingBoxf Bound;

	// Mesh part vertex and index data read from FBX
	uint32_t VertexFlags;
	vector<uint32_t> Indices;
	vector<Vertex> Vertices;
	
	uint32_t StartIndex;
	uint32_t IndexCount;
	
	int32_t BaseVertex;

	uint32_t VertexBufferIndex;
	uint32_t IndexBufferIndex;

	MeshPartData() : VertexFlags(0), StartIndex(0), BaseVertex(0), IndexCount(0) {}
};

// Mesh Structure
struct MeshData
{
	String Name;
	BoundingBoxf Bound;

	shared_ptr<Skeleton> Skeleton;

	vector<IndexBufferType> IndexTypes; 
	vector<vector<uint32_t> > Indices;
	vector<vector<Vertex> > Vertices;

	vector< shared_ptr<MeshPartData> > MeshParts;
};

struct AnimationClipData
{
	struct  KeyFrame
	{
		// The time offset from the start of the animation to this key frame.
		float Time;

		float3 Translation;
		Quaternionf Rotation;
		float3 Scale;
	};

	struct  AnimationTrack
	{
		String Name; // Bone name
		vector<KeyFrame> KeyFrames;
	};

	float Duration;
	vector<AnimationTrack> mAnimationTracks;
};

class FbxProcesser
{
public:
	FbxProcesser(void);
	~FbxProcesser(void);

	void RunCommand(const vector<String>& arguments);
		
public:
	void Initialize();
	bool LoadScene(const String& filename);

	void ProcessScene();
	
	void BuildAndSaveXML();
	void BuildAndSaveBinary();	
	void BuildAndSaveMaterial();
	void BuildAndSaveOBJ();

	void ExportMaterial();

	void LoadOgre(const char* filename);
	
private:
	void ProcessMesh(FbxNode* pNode);
	void ProcessAnimation(const String& clipName, FbxNode* pNode, double fFrameRate, double fStart, double fStop);

	void BuildSkeleton();
	void ComputeSkeletonBindPose(FbxNode* pMeshNode);
	void ComputeBoneLocalPose(FbxNode* pBoneFBX, Bone* pBone);
	void CalculateBindPose(Bone* bone, std::map<Bone*, FbxAMatrix>& bindPoseMap);
	shared_ptr<Skeleton> ProcessBoneWeights(FbxMesh* pMesh, std::vector<BoneWeights>& meshBoneWeights);

	void CollectSceneNodes(FbxNode* pNode);
	void CollectMaterials();
	void CollectAnimations();

	void MergeSceneMeshs();

	/**
	 * Merge mesh part vertices into one big VertexBuffer if VertexFormat are same.
	 */
	void MergeMeshParts();

	
private:
	void ImportOgreMesh(FileStream& stream, shared_ptr<MeshData> pMesh);
	void ReadSubMesh(FileStream& stream, shared_ptr<MeshData>& pMesh);
	void ReadSkeleton(FileStream& stream, shared_ptr<MeshData>& pMesh);
	void ReadAnimation(FileStream& stream, shared_ptr<Skeleton>& pMesh);
	void ReadAnimationTrack(FileStream& stream, AnimationClipData& clipData, shared_ptr<Skeleton>& pSkel);

public:
	FbxManager* mFBXSdkManager;
	FbxScene* mFBXScene;
	bool mQuietMode;
	bool mMergeScene;
	
	String mSceneName;
	String mOutputPath;
	String mAnimationName;
	
	unordered_set<FbxNode*> mFBXCameraNodes;
	unordered_set<FbxNode*> mFBXLightNodes;
	unordered_set<FbxNode*> mFBXMeshNodes;

	unordered_map<FbxNode*, bool> mFBXBoneNodes;
	unordered_map<FbxNode*, FbxAMatrix> mBoneBindPose;

	struct SkeletonBuilder
	{
		shared_ptr<Skeleton> Skeleton;
		vector<FbxNode*> BoneNodes;
	};
	unordered_map<FbxMesh*,  SkeletonBuilder> mFBXMeshSkeletonMap;

	unordered_map<String, AnimationClipData> mAnimationClips;

	vector<MaterialData> mMaterials;

	vector<shared_ptr<MeshData> > mSceneMeshes;
	
private:
	FBXTransformer mFBXTransformer;
};
