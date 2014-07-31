#include "AssimpProcesser.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Animation.h"
#include "Graphics/Skeleton.h"
#include "Math/ColorRGBA.h"
#include "Math/Matrix.h"
#include "Math/MathUtil.h"
#include "Math/BoundingSphere.h"
#include "Graphics/Mesh.h"
#include "MainApp/Application.h"
#include "Core/Exception.h"
#include "IO/FileStream.h"
#include "Core/XMLDom.h"
#include "Core/Utility.h"
#include "IO/PathUtil.h"


#pragma comment(lib, "assimp.lib")


bool operator == (const MaterialData& lrhs, const MaterialData& rhs)
{
	if (lrhs.MaterialFlags != rhs.MaterialFlags)
	{
		return false; 
	}

	if (lrhs.Ambient != rhs.Ambient || 
		lrhs.Diffuse != rhs.Diffuse || lrhs.Specular != rhs.Specular || 
		lrhs.Emissive != rhs.Emissive || lrhs.Power != rhs.Power)
	{
		return false;
	}

	if (lrhs.Textures.size() != rhs.Textures.size())
	{
		return false;
	}

	for (auto iter = lrhs.Textures.begin(); iter != lrhs.Textures.end(); ++iter)
	{
		const String& key = iter->first;

		auto found = rhs.Textures.find(key);
		if ( found == rhs.Textures.end() || found->second != iter->second )
		{
			return false;
		}
	}

	return true;
}


String XMLFromVector3(const float3& vec)
{
	std::stringstream sss;
	sss << "x=\"" << vec.X() << "\" y=\"" << vec.Y() << "\" z=\"" << vec.Z() << "\"";
	return  sss.str();
}

String XMLFromColorRGBA(const ColorRGBA& color)
{
	std::stringstream sss;
	sss << "r=\"" << color.R() << "\" g=\"" << color.G() << "\" b=\"" << color.B() << "\" a=\"" << color.A() <<  "\"";
	return  sss.str();
}

String XMLFromQuaternion(const Quaternionf& quat)
{
	std::stringstream sss;
	sss << "w=\"" << quat.W() << "\" x=\"" << quat.X() << "\" y=\"" << quat.Y() << "\" z=\"" << quat.Z() << "\"";
	return  sss.str();
}


float3 Transform(const float3& vec, const float4x4& mat)
{
	float4 vec4(vec.X(), vec.Y(), vec.Z(), 1.0f);
	float4 transformed = vec4 * mat;
	return float3(transformed.X(), transformed.Y(), transformed.Z());
}



// Convert aiMatrix to RcEngine matrix, note that assimp 
// assume the right handed coordinate system, so aiMatrix 
// is a right-handed matrix.You need to transpose it to get
// a left-handed matrix.
float4x4 FromAIMatrix(const aiMatrix4x4& in)
{
	float4x4 out;
	out.M11 = in.a1;
	out.M12 = in.b1;
	out.M13 = in.c1;
	out.M14 = in.d1;
	
	out.M21 = in.a2;
	out.M22 = in.b2;
	out.M23 = in.c2;
	out.M24 = in.d2;
	
	out.M31 = in.a3;
	out.M32 = in.b3;
	out.M33 = in.c3;
	out.M34 = in.d3;
	
	out.M41 = in.a4;
	out.M42 = in.b4;
	out.M43 = in.c4;
	out.M44 = in.d4;

	return out;
}

float3 FromAIVector(const aiVector3D& vec)
{
	 return float3(vec.x, vec.y, vec.z);
}

Quaternionf FromAIQuaternion(const aiQuaternion& quat)
{
	return Quaternionf(quat.w, quat.x, quat.y, quat.z);
}

String FromAIString(const aiString& str)
{
	return String(str.C_Str());
}

int32_t GetBoneIndex(const OutModel& model, const aiString& boneName)
{
	for (size_t i = 0; i < model.Bones.size(); ++i)
	{
		if (boneName == model.Bones[i]->mName)
		{
			return i;
		}
	}
	return -1;
}

aiMatrix4x4 GetDerivedTransform( aiMatrix4x4 transform, aiNode* node, aiNode* rootNode )
{
	while(node && node != rootNode)
	{
		// get parent node
		node = node->mParent;

		if (node)
			transform = node->mTransformation * transform;
		
	}
	return transform;
}

aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode)
{
	return GetDerivedTransform(node->mTransformation, node, rootNode);
}

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* meshRootNode)
{
	if (meshNode == meshRootNode)
		return aiMatrix4x4();
	else
		return GetDerivedTransform(meshNode, meshRootNode);
}

aiMatrix4x4 GetOffsetMatrix(const OutModel& model, const String& boneName)
 {
	for (size_t i = 0; i < model.Meshes.size(); ++i)
	{
		aiMesh* mesh = model.Meshes[i];
		aiNode* meshNode = model.MeshNodes[i];

		for (size_t j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			if (String(bone->mName.C_Str()) == boneName)
			{
				aiMatrix4x4 offset = bone->mOffsetMatrix;

				aiNode* node = model.RootNode->FindNode(bone->mName);

				aiMatrix4x4 derivedMat = meshNode->mTransformation * GetDerivedTransform(node, model.RootBone);
				derivedMat.Inverse();

				/* Note that the all mesh vertex has been baked into the same coordinate system which
				 * is defined by the root node, called model space. So offset matrix must first transform
				 * vertex from model space to mesh space, then transform mesh space to bone space in bind pose
				 * that why we multiply the nodeDerivedInverse matrix.If you use left-handed, change it correspondingli 
				 * V(bone) = offset * nodeDerivedInverse * V(model),
				 */
				aiMatrix4x4 nodeDerivedInverse = GetMeshBakingTransform(meshNode, model.RootNode);
				nodeDerivedInverse.Inverse();
				offset = offset * nodeDerivedInverse;
				return offset;
			}
		}
	}
	return aiMatrix4x4();
}

//shared_ptr<VertexDeclaration> GetVertexDeclaration(aiMesh* mesh)
//{
//	vector<VertexElement> vertexElements;
//	unsigned int offset = 0;
//	if (mesh->HasPositions())
//	{
//		vertexElements.push_back(VertexElement(offset, VEF_Float3, VEU_Position, 0));
//		//offset += VertexElement::GetTypeSize(VEF_Float3);
//	}
//
//	if (mesh->HasNormals())
//	{
//		vertexElements.push_back(VertexElement(offset, VEF_Float3, VEU_Normal, 0));
//		//offset += VertexElement::GetTypeSize(VEF_Float3);
//	}
//
//	if (mesh->HasTangentsAndBitangents())
//	{
//		vertexElements.push_back(VertexElement(offset, VEF_Float3, VEU_Tangent, 0));
//		//offset += VertexElement::GetTypeSize(VEF_Float3);
//
//		vertexElements.push_back(VertexElement(offset, VEF_Float3, VEU_Binormal, 0));
//		//offset += VertexElement::GetTypeSize(VEF_Float3);
//	}
//
//	for (unsigned int i = 0; i < mesh->GetNumUVChannels(); ++i)
//	{
//		switch (mesh->mNumUVComponents[i])
//		{
//		case 1:
//			vertexElements.push_back(VertexElement(offset, VEF_Float, VEU_TextureCoordinate, i));
//			//offset += VertexElement::GetTypeSize(VEF_Float);
//			break;
//		case 2:
//			vertexElements.push_back(VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, i));
//			//offset += VertexElement::GetTypeSize(VEF_Float2);
//			break;
//		case 3:
//			vertexElements.push_back(VertexElement(offset, VEF_Float3, VEU_TextureCoordinate, i));
//			//offset += VertexElement::GetTypeSize(VEF_Float3);
//			break;
//		}
//	}
//
//	if (mesh->HasBones())
//	{
//		vertexElements.push_back(VertexElement(offset, VEF_Float4, VEU_BlendWeight, 0));
//		//offset += VertexElement::GetTypeSize(VEF_Float4);
//
//		vertexElements.push_back(VertexElement(offset, VEF_UInt4, VEU_BlendIndices, 0));
//		//offset += VertexElement::GetTypeSize(VEF_UInt4);
//	}
//
//	//shared_ptr<VertexDeclaration> vd ( new VertexDeclaration(vertexElements) );
//	//assert(vd->GetVertexSize() == offset );
//	return vd;
//}

void GetBlendData(OutModel& model, aiMesh* mesh, vector<uint32_t>& boneMappings, vector<vector<uint8_t> >&
	blendIndices, vector<vector<float> >& blendWeights)
{
	blendIndices.resize(mesh->mNumVertices);
	blendWeights.resize(mesh->mNumVertices);

	for (size_t i = 0; i < mesh->mNumBones; ++i)
	{
		aiBone* boneAI = mesh->mBones[i];
		String boneName = String( boneAI->mName.C_Str() );
		Bone* bone = model.Skeleton->GetBone(boneName);
		
		if (!bone)
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Bone:" + boneName + " not found!", "GetBlendData");
		}
		
		for (size_t j = 0; j < boneAI->mNumWeights; ++j)
		{
			uint32_t vertexID = boneAI->mWeights[j].mVertexId;
			float weight = boneAI->mWeights[j].mWeight;
			blendIndices[vertexID].push_back(bone->GetBoneIndex());
			blendWeights[vertexID].push_back(weight);
			if (blendWeights[vertexID].size() > 4)
			{
				auto minIter = std::min_element(blendWeights[vertexID].begin(), blendWeights[vertexID].begin() + 4);
				float minWeight = *minIter;
				size_t minIndex = std::distance(blendWeights[vertexID].begin(), minIter);
				*minIter += weight;
				break;;
				//ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Per vertex bones limit less than 4", "GetBlendData");
			}
		}
		
	}
}

void PrintLine(const String& str)
{
	cout << str << endl;
}

AssimpProcesser::AssimpProcesser(void)
{
}


AssimpProcesser::~AssimpProcesser(void)
{
}

bool AssimpProcesser::Process( const char* filePath, const char* skeleton /*= ""*/, const vector<String>& clips )
{
	mSkeletonFile = skeleton;
	mAnimationClips = clips;
	mFilename = filePath;

	Assimp::Importer importer;

	mAIScene = const_cast<aiScene*>( importer.ReadFile(filePath, aiProcess_Triangulate |
		aiProcess_RemoveRedundantMaterials  /*| aiProcess_FlipUVs*/) );

	if(!mAIScene)
	{
		// Error
		std::cerr <<  importer.GetErrorString() << std::endl;
		return false;
	}

	ProcessScene(mAIScene);

	return true;
}

void AssimpProcesser::ProcessScene( const aiScene* scene )
{
	// if user don't specify model root node, use assimp root scene node 
	mModel.RootNode = mAIScene->mRootNode;

	String outputName = PathUtil::GetFileName(mFilename);
	ExportModel(mModel, outputName);
}


shared_ptr<MaterialData> AssimpProcesser::ProcessMaterial( aiMaterial* material )
{
	if (!material)
	{
		return nullptr;
	}

	shared_ptr<MaterialData> materialData( new MaterialData );

	for (unsigned int i = 0; i < material->mNumProperties; ++i)
	{
		aiMaterialProperty* prop = material->mProperties[i];

		if (prop->mKey == aiString("?mat.name"))
		{
			aiString name;
			material->Get(AI_MATKEY_NAME,name);
			String matName = string(name.C_Str());
			/*if (matName.size())
			{
				size_t split = matName.find_last_of('\\');
				if (split != string::npos)
				{
					matName = mName.substr(split);
				}
			}*/
			materialData->Name = matName;
		}
		else if (prop->mKey == aiString("$clr.ambient"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_AMBIENT,color);
			materialData->Ambient = ColorRGBA(color.r, color.g, color.b, 1.0f);
			materialData->MaterialFlags |= MAT_AMBIENT_COLOR;

		}
		else if (prop->mKey == aiString("$clr.diffuse"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
			materialData->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
			materialData->MaterialFlags |= MAT_DIFFUSE_COLOR;
		}
		else if (prop->mKey == aiString("$clr.specular"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_SPECULAR,color);
			materialData->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
			materialData->MaterialFlags |= MAT_SPECULAR_COLOR;
		}
		else if (prop->mKey == aiString("$mat.shininess"))
		{
			float shininess;
			material->Get(AI_MATKEY_SHININESS,shininess);
			materialData->Power = shininess;
			materialData->MaterialFlags |= MAT_POWER_COLOR;
		}
		else if (prop->mKey == aiString("$tex.file"))
		{
			switch(prop->mSemantic)
			{
			case aiTextureType_AMBIENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_AMBIENT, 0, &file);
					materialData->Textures.insert(make_pair(string("AmbientMap"), string(file.C_Str())));
				}
			case aiTextureType_DIFFUSE:
				{
					aiString file;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
					materialData->Textures.insert(make_pair(String("DiffuseMap"), String(file.C_Str())));
				}
				break;
			case aiTextureType_SPECULAR:
				{
					aiString file;
					material->GetTexture(aiTextureType_SPECULAR, 0, &file);
					materialData->Textures.insert(make_pair(string("SpecularMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_NORMALS:
				{
					aiString file;
					material->GetTexture(aiTextureType_NORMALS, 0, &file);
					materialData->Textures.insert(make_pair(string("NormalMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_DISPLACEMENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_DISPLACEMENT, 0, &file);
					materialData->Textures.insert(make_pair(string("DisplacementMap"), string(file.C_Str())));
				}
				break;
			default:
				{

				}
			}
		}
	}

	return materialData;
}

void AssimpProcesser::ExportXML(  OutModel& outModel )
{
	ofstream file(outModel.OutName + ".xml");

	file << "<mesh name=\"" << outModel.OutName << "\">" << endl;

	// write mesh bouding sphere
	const float3& meshCenter = outModel.MeshBoundingSphere.Center;
	const float meshRadius = outModel.MeshBoundingSphere.Radius;
	file << "\t<bounding x=\"" << meshCenter[0] << "\" y=\"" << meshCenter[1] << "\" z=\"" << meshCenter[2] << " radius=" << meshRadius << "\"/>\n";
		
	/*if (outModel.Bones.size())
	{
		vector<Bone*> bones = outModel.Skeleton->GetBones();
		file << "\t<bones boneCount=\"" << bones.size() << "\">" << std::endl;
		for (size_t i = 0; i < bones.size(); ++i)
		{
			Bone* bone = bones[i];
			const String& boneName = bone->GetName();
			String parentName;
			if (bone->etParent())
			{
				parentName = bone->GetParent()->GetName();
			}
			else
			{
				parentName = "";
			}
			file << "\t\t<bone name=\"" << boneName << "\" parent=\"" << parentName <<  "\">" << std::endl;

			float3 pos = bone->GetPosition();
			float3 scale = bone->GetScale();
			Quaternionf quat = bone->GetRotation();
			file << "\t\t\t<bindPosition x=\"" << pos[0] << "\" y=\"" << pos[1] << "\" z=\"" << pos[2] << "\"/>\n";
			file << "\t\t\t<bindRotation w=\"" << quat.W() << "\" x=\"" << quat.X() << "\" y=\"" << quat.Y() << " z=" << quat.Z() << "\"/>\n";
			file << "\t\t\t<bindScale x=\"" << scale[0] << "\" y=\"" << scale[1] << "\" z=\"" << scale[2]  << "\"/>\n";
			file << "\t\t</bone>" << std::endl; 
		}
		file << "\t</bones>" << std::endl;
	}*/
	
	/*if (outModel.Materials.size())
	{
		file << "\t<materials count=\"" << outModel.Materials.size() << "\">" << std::endl;
		for (size_t i = 0; i < outModel.Materials.size(); ++i)
		{
			shared_ptr<MaterialData> material = outModel.Materials[i];

			file << "\t\t<materials name=\""  << material->Name << "\">\n";

			if (material->MaterialFlags & MAT_AMBIENT_COLOR)
			{
				file << "\t\t\t<ambiemtColor " << XMLFromColorRGBA(material->Ambient) << "/>\n";
			}

			if (material->MaterialFlags & MAT_DIFFUSE_COLOR)
			{
				file << "\t\t\t<diffuseColor " << XMLFromColorRGBA(material->Diffuse) << "/>\n";
			}

			if (material->MaterialFlags & MAT_SPECULAR_COLOR)
			{
				file << "\t\t\t<apecularColor " << XMLFromColorRGBA(material->Specular) << "/>\n";
			}

			if (material->MaterialFlags & MAT_EMISSIVE_COLOR)
			{
				file << "\t\t\t<emissiveColor " << XMLFromColorRGBA(material->Emissive) << "/>\n";
			}

			if (material->MaterialFlags & MAT_POWER_COLOR)
			{
				file << "\t\t\t<ambiemtColor power=\"" << material->Power << "\"/>\n";
			}

			for (auto iter = material->Textures.begin(); iter != material->Textures.end(); ++iter)
			{
				String texType = iter->first;
				String value = iter->second;

				file << "\t\t\t<texture type=\"" << texType << "\"" << " value=\"" << value << "\"/>\n";
			}
		}
	}*/
	

	//vector<shared_ptr<MeshPartData> >& subMeshes = outModel.MeshParts;
	//for (size_t i = 0; i < subMeshes.size(); ++i)
	//{
	//	shared_ptr<MeshPartData> submesh = subMeshes[i];
	//	
	//	file << "\t<subMesh name=\"" << submesh->Name << "\" material=\"" << submesh->MaterialName << "\">\n";

	//	// write bounding sphere
	//	const float3& center = submesh->BoundingSphere.Center;
	//	const float radius = submesh->BoundingSphere.Radius;
	//	file << "\t\t<bounding x=\"" << center[0] << "\" y=\"" << center[1] << "\" z=\"" << center[2] << " radius=" << radius << "\"/>\n";

	//	// write each vertex element
	//	const std::vector<VertexElement>& elements = submesh->VertexDeclaration->GetElements();
	//	file << "\t\t<vertice vertexCount=\"" << submesh->VertexCount << "\" vertexSize=\"" << submesh->VertexDeclaration->GetVertexSize()<<"\"/>\n";

	//	for (size_t i = 0; i < submesh->VertexCount; ++i)
	//	{
	//		file << "\t\t\t<vertex>\n";
	//		int baseOffset = submesh->VertexDeclaration->GetVertexSize() * i;
	//		for (auto iter = elements.begin(); iter != elements.end(); ++iter)
	//		{
	//			const VertexElement& element = *iter;
	//			float* vertexPtr = (float*)(&submesh->VertexData[0] + baseOffset + element.Offset);
	//			switch(element.Usage)
	//			{
	//			case VEU_Position:
	//				{
	//					file << "\t\t\t\t<position x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
	//				}	
	//				break;
	//			case VEU_Normal:
	//				{
	//					file << "\t\t\t\t<normal x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
	//				}
	//				break;
	//			case VEU_Tangent:
	//				{
	//					file << "\t\t\t\t<tangent x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
	//				}			
	//				break;
	//			case VEU_Binormal:
	//				{
	//					file << "\t\t\t\t<binormal x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
	//				}			
	//				break;
	//			case VEU_TextureCoordinate:
	//				{
	//					switch(VertexElement::GetTypeCount(element.Type))
	//					{
	//					case 3:
	//						file << "\t\t\t\t<texcoord u=\"" << vertexPtr[0] << "\" v=\"" << vertexPtr[1] << "\" r=\"" << vertexPtr[2] << "\"/>\n";
	//						break;
	//					case 2:
	//						file << "\t\t\t\t<texcoord u=\"" << vertexPtr[0] << "\" v=\"" << vertexPtr[1] << "\"/>\n";
	//						break;
	//					case 1:
	//						file << "\t\t\t\t<texcoord u=\"" << vertexPtr[0] << "\"/>\n";
	//						break;
	//					}
	//				}
	//				break;
	//			case VEU_BlendWeight:
	//				{
	//					uint32_t* indexPtr = (uint32_t*)(vertexPtr + 4);
	//					for (size_t k = 0; k < 4; ++k)
	//					{
	//						file << "\t\t\t\t<bone weight=\"" << vertexPtr[k] << "\" index=\"" << indexPtr[k] << "\"/>\n";
	//					}	
	//				}
	//				break;	
	//			default:
	//				break;
	//			}
	//		}
	//		file << "\t\t\t</vertex>\n";
	//	}
	//	file << "\t\t<vertices/>\n";

	//	file << "\t\t<triangles triangleCount=\""<<submesh->IndexCount/3 << "\">\n";
	//	
	//	uint32_t offset = 0;
	//	for (size_t i = 0; i < submesh->IndexCount / 3; i+=3)
	//	{
	//		
	//		if (submesh->IndexFormat == IBT_Bit32)
	//		{
	//			uint32_t* idx = (uint32_t*)( &submesh->IndexData[0] + offset);
	//			offset += sizeof(uint32_t) * 3;
	//			file << "\t\t\t<triangle a=\"" << idx[0] << "\" b=\"" << idx[1] << "\" c=\"" << idx[2] << "\"/>\n";
	//		}else
	//		{
	//			uint16_t* idx = (uint16_t*)( &submesh->IndexData[0] + offset);
	//			offset += sizeof(uint16_t) * 3;
	//			file << "\t\t\t<triangle a=\"" << idx[0] << "\" b=\"" << idx[1] << "\" c=\"" << idx[2] << "\"/>\n";
	//		}
	//	}
	//	file << "\t\t</triangles>\n";
	//	file << "\t</submesh>\n";
	//}

	//if (mAIScene->HasAnimations())
	//{
	//	size_t animSize = mAnimationClips.size();
	//	file << "\t\t<animations count=\"" << animSize << "\">\n";
	//	for (size_t i = 0; i < animSize; ++i)
	//	{
	//		file << "\t\t\t<clip name=\"" << mAnimationClips[i] << "\"/>\n";
	//	}
	//	file << "\t\t</animations>\n";
	//}
	
}

void AssimpProcesser::ExportBinary( OutModel& outModel )
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	FileStream stream(outModel.OutName + ".mdl", FILE_WRITE);

	stream.WriteUInt(MeshId);

	// write mesh name, for test
	stream.WriteString(outModel.OutName);

	// write mesh bounding sphere
	float3 center = outModel.MeshBoundingSphere.Center;
	float radius = outModel.MeshBoundingSphere.Radius;
	stream.Write(&center, sizeof(float3));
	stream.WriteFloat(radius);

	// write material
	//stream.WriteUInt(outModel.Materials.size());
	//if (outModel.Materials.size())
	//{
	//	for (size_t i = 0; i < outModel.Materials.size(); ++i)
	//	{
	//		shared_ptr<MaterialData> material = outModel.Materials[i];

	//		String matName;
	//		if (material->Name.empty())
	//		{
	//			int a = 0;
	//		}
	//		else
	//		{
	//			matName = material->Name + ".material.xml";
	//		}

	//		// write name
	//		stream.WriteString(matName);
	//	}
	//}

	// write mesh parts count
	//stream.WriteUInt(outModel.MeshParts.size());

	//vector<shared_ptr<MeshPartData> >& subMeshes = outModel.MeshParts;
	//for (size_t i = 0; i < subMeshes.size(); ++i)
	//{
	//	shared_ptr<MeshPartData> submesh = subMeshes[i];
	//	
	//	// write sub mesh name
	//	stream.WriteString(submesh->Name);	

	//	// write material index
	//	stream.WriteString(submesh->MaterialName + ".material.xml");

	//	// write sub mesh bounding sphere
	//	float3 center = submesh->BoundingSphere.Center;
	//	float radius = submesh->BoundingSphere.Radius;
	//	stream.Write(&center, sizeof(float3));
	//	stream.WriteFloat(radius);

	//	// write vertex count and vertex size
	//	stream.WriteUInt(submesh->VertexCount);
	//	stream.WriteUInt(submesh->VertexDeclaration->GetVertexSize());

	//	// write vertex declaration, elements count
	//	stream.WriteUInt(submesh->VertexDeclaration->GetElementCount());

	//	// write each vertex element
	//	const std::vector<VertexElement>& elements = submesh->VertexDeclaration->GetElements();
	//	for (auto iter = elements.begin(); iter != elements.end(); ++iter)
	//	{
	//		const VertexElement& ve = *iter;
	//		stream.WriteUInt(ve.Offset);
	//		stream.WriteUInt(ve.Type);
	//		stream.WriteUInt(ve.Usage);
	//		stream.WriteUShort(ve.UsageIndex);
	//	}

	//	// write vertex buffer data
	//	uint32_t bufferSize = submesh->VertexCount * submesh->VertexDeclaration->GetVertexSize();
	//	stream.Write(&(submesh->VertexData[0]), sizeof(char) * submesh->VertexData.size());

	//	// write triangles count
	//	stream.WriteUInt(submesh->IndexCount);
	//	stream.WriteUInt(submesh->IndexFormat);
	//	stream.Write(&submesh->IndexData[0], sizeof(char) * submesh->IndexData.size());
	//}

	//if (outModel.Bones.empty())
	//{
	//	stream.WriteUInt(0);
	//}
	//else
	//{
	//	FileStream skeleton;
	//	skeleton.Open(mSkeletonFile);
	//	uint32_t size = skeleton.GetSize();
	//	vector<char> skeletonData(size);
	//	skeleton.Read(&skeletonData[0], size);
	//	stream.Write(&skeletonData[0], size);
	//	skeleton.Close();
	//}

	//if (mAIScene->HasAnimations())
	//{
	//	size_t animSize = mAnimationClips.size();
	//	stream.WriteUInt(animSize);
	//	for (size_t i = 0; i < animSize; ++i)
	//	{
	//		stream.WriteString(mAnimationClips[i]);
	//	}
	//}else
	//{
	//	stream.WriteUInt(0);
	//}

	//if (outModel.Bones.empty())
	//{
	//	stream.WriteUInt(0);
	//}
	//else
	//{
	//	vector<Bone*> bones = outModel.Skeleton->GetBones();
	//	stream.WriteUInt(bones.size());
	//	for (size_t i = 0; i < bones.size(); ++i)
	//	{
	//		Bone* bone = bones[i];

	//		float3 pos = bone->GetPosition();
	//		Quaternionf rot = bone->GetRotation();
	//		float3 scale = bone->GetScale();

	//		Bone* parent = static_cast_checked<Bone*>( bone->GetParent() );
	//		String parentName = parent ? parent->GetName() : String("");

	//		stream.WriteString(bone->GetName());
	//		stream.WriteString(parentName);
	//		stream.Write(&pos, sizeof(float3));
	//		stream.Write(&rot, sizeof(Quaternionf));
	//		stream.Write(&scale, sizeof(float3));

	//		// sphere radius
	//		float radius = mModel.BoneSpheres[i].Radius;
	//		if (!mModel.BoneSpheres[i].Defined)
	//		{
	//			radius = 5;
	//		}
	//		stream.WriteFloat(radius);
	//	}
	//}
	
	stream.Close();
}

void AssimpProcesser::CollectMeshes( OutModel& outModel, aiNode* node )
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = mAIScene->mMeshes[node->mMeshes[i]];
		for (size_t j = 0; j < outModel.Meshes.size(); ++j)
		{
			if (outModel.Meshes[j] == mesh)
			{
				PrintLine("Same mesh found in multiple node");
				break;;
			}
		}

		outModel.Meshes.push_back(mesh);
		outModel.MeshNodes.push_back(node);
		outModel.TotalVertices += mesh->mNumVertices;
		outModel.TotalIndices += mesh->mNumFaces * 3;
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		CollectMeshes(outModel, node->mChildren[i]);
	}
}

void AssimpProcesser::CollectBones( OutModel& outModel )
{
	std::set<aiNode*> necessary;
	std::set<aiNode*> rootNodes;

	for (uint32_t i =0; i < outModel.Meshes.size(); ++i)
	{
		aiMesh* mesh = outModel.Meshes[i];
		aiNode* meshNode = outModel.MeshNodes[i];
		aiNode* meshParentNode = meshNode->mParent;
		

		for (size_t j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			String boneName(bone->mName.C_Str());
			aiNode* boneNode = mAIScene->mRootNode->FindNode(bone->mName);

			if (!boneNode)
			{
				ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Couldn't find the scene node for Bone: " + boneName
					, "AssimpProcesser::CollectBones");
			}

			necessary.insert(boneNode);

			// flag this node and all parents of this node as needed, until we reach the node holding the mesh, or the parent.
			aiNode* rootNode = boneNode;
			while(true)
			{
				boneNode = boneNode->mParent;
				if (!boneNode || boneNode == meshNode || boneNode == meshParentNode)
					break;
				rootNode = boneNode;
				necessary.insert(boneNode);
			}
			rootNodes.insert(rootNode);
		}
	}

	 // If we find multiple root nodes
	if (rootNodes.size() > 1)
	{
		aiNode* commonParent =  (*rootNodes.begin())->mParent;

		for(auto iter = rootNodes.begin(); iter != rootNodes.begin(); ++iter)
		{
			if ( (*iter) != commonParent )
			{
				if (!commonParent || (*iter)->mParent != commonParent)
				{
					 ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR,
						 "Skeleton with multiple root nodes found, not supported", "AssimpProcesser::CollectBones");
				}
			}
		}

		rootNodes.clear();
		rootNodes.insert(commonParent);
		necessary.insert(commonParent);
	}

	if (rootNodes.empty())
		return;

	outModel.RootBone = *rootNodes.begin();
	CollectBonesFinal(outModel.Bones, necessary, outModel.RootBone);
}

void AssimpProcesser::CollectBonesFinal( vector<aiNode*>& bones, const set<aiNode*>& necessary, aiNode* node )
{
	if (necessary.find(node) != necessary.end())
	{
		bones.push_back(node);

		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			CollectBonesFinal(bones, necessary, node->mChildren[i]);
		}
	}
}

void AssimpProcesser::CollectAnimations( OutModel& model, aiScene* scene )
{
	for (size_t i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];

		for (size_t j = 0; j < anim->mNumChannels; ++j)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			aiString boneName = channel->mNodeName;

			auto found = std::find_if(model.Bones.begin(), model.Bones.end(),
				[&boneName](aiNode* bone) {return boneName == bone->mName;} );

			if (found != model.Bones.end())
			{
				model.Animations.push_back(anim);
			}
		}
	}
}

void AssimpProcesser::ExportModel( OutModel& outModel, const String& outName )
{
	outModel.OutName = outName;
	
	CollectMaterials();
	
	CollectMeshes(outModel, outModel.RootNode);

	CollectBones(outModel);

	BuildAndSaveModel(outModel);

	if (mAIScene->HasAnimations())
	{
		CollectAnimations(outModel, mAIScene);
		/*BuildAndSaveAnimations(outModel);
		BuildBoneCollisions();*/
	}

	ExportBinary(mModel);
	ExportXML(mModel);
}

void AssimpProcesser::BuildAndSaveModel( OutModel& outModel )
{

}

//void AssimpProcesser::BuildAndSaveModel( OutModel& outModel )
//{
//	if (!outModel.RootNode)
//	{
//		// Error no model root node
//		return;
//	}
//
//	String rootNodeName = String(outModel.RootNode->mName.C_Str());
//	if (outModel.Meshes.empty())
//	{
//		// Error, no geometry start from this node
//		return;
//	}
//
//	// animated model
//	if (!outModel.Bones.empty())
//	{
//		BuildSkeleton(outModel);
//	}
//
//	for (size_t i = 0; i < outModel.Meshes.size(); ++i)
//	{
//		BoundingSpheref sphere; 
//
//		aiMesh* mesh = outModel.Meshes[i];
//		aiNode* meshNode = outModel.MeshNodes[i];
//
//		// if animated all submeshes must have bone weights
//		if (outModel.Bones.size() && !mesh->HasBones())
//		{
//			continue;
//			//ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Animated all submeshes must have bone weights", "AssimpProcesser::BuildAndSaveModel");
//		}
//
//		// Get the world transform of the mesh for baking into the vertices
//		aiMatrix4x4 vertexTransformAI = GetMeshBakingTransform(meshNode, outModel.RootNode);
//		aiMatrix4x4 normalTransformAI = vertexTransformAI.Inverse().Transpose();
//
//		aiVector3D translationAI, scaleAI;
//		aiQuaternion rotationAI;
//		vertexTransformAI.Decompose(scaleAI, rotationAI, translationAI);
//
//		shared_ptr<MeshPartData> meshPart(new MeshPartData);
//		//shared_ptr<VertexDeclaration> vertexDecl = GetVertexDeclaration(mesh);
//
//		meshPart->Name = String(mesh->mName.C_Str());
//		meshPart->MaterialName = outModel.Materials[mesh->mMaterialIndex]->Name;
//
//		// Store index data
//		bool largeIndices = mesh->mNumVertices > 65535;
//		if (!largeIndices)
//		{
//			meshPart->IndexData.resize( sizeof(uint16_t) * mesh->mNumFaces * 3 );
//			uint16_t* dest = (uint16_t*)(&meshPart->IndexData[0]);
//			for (unsigned f = 0; f < mesh->mNumFaces; ++f)
//			{
//				aiFace face = mesh->mFaces[f];
//				assert(face.mNumIndices == 3);
//				*dest++ =  face.mIndices[0];
//				*dest++ =  face.mIndices[1];
//				*dest++ =  face.mIndices[2];
//			}
//			meshPart->IndexFormat = IBT_Bit16;
//			meshPart->IndexCount =  mesh->mNumFaces * 3;
//		}
//		else
//		{
//			meshPart->IndexData.resize( sizeof(uint32_t) * mesh->mNumFaces * 3 );
//			uint32_t* dest = (uint32_t*)(&meshPart->IndexData[0]);
//			for (unsigned f = 0; f < mesh->mNumFaces; ++f)
//			{
//				aiFace face = mesh->mFaces[f];
//				assert(face.mNumIndices == 3);
//				*dest++ =  face.mIndices[0];
//				*dest++ =  face.mIndices[1];
//				*dest++ =  face.mIndices[2];
//			}
//			meshPart->IndexFormat = IBT_Bit32;
//			meshPart->IndexCount =  mesh->mNumFaces * 3;
//		}
//
//		// Store vertex data
//		meshPart->VertexData.resize( vertexDecl->GetVertexSize() * mesh->mNumVertices );
//
//		// assign per vertex bone info
//		vector<vector<uint8_t> > blendIndices;
//		vector<vector<float> > blendWeights;
//		vector<uint32_t> boneMappings;
//		if (outModel.Bones.size())
//		{
//			GetBlendData(outModel, mesh, boneMappings, blendIndices, blendWeights);
//		}
//
//		size_t vs = vertexDecl->GetVertexSize();
//
//		const vector<VertexElement>& vertexElements = vertexDecl->GetElements();
//		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
//		{
//			int baseOffset = vertexDecl->GetVertexSize() * i;
//			for (size_t ve = 0; ve < vertexElements.size(); ++ve)
//			{
//				const VertexElement& element = vertexElements[ve];
//				float* vertexPtr = (float*)(&meshPart->VertexData[0] + baseOffset + element.Offset);
//				switch(element.Usage)
//				{
//				case VEU_Position:
//					{
//						// Bake the mesh vertex in model space defined by the root node
//						// So even without the skeleton, the mesh can render with unskin mesh.				
//						float3 vertex = FromAIVector( vertexTransformAI *  mesh->mVertices[i] );
//						
//						*(vertexPtr) = vertex.X();
//						*(vertexPtr+1) = vertex.Y();
//						*(vertexPtr+2) = vertex.Z();
//
//						// bouding sphere
//						sphere.Merge(vertex);
//					}	
//					break;
//				case VEU_Normal:
//					{
//						float3 normal = FromAIVector( normalTransformAI * mesh->mNormals[i] );
//						*(vertexPtr) = normal.X();
//						*(vertexPtr+1) = normal.Y();
//						*(vertexPtr+2) = normal.Z();
//					}
//					break;
//				case VEU_Tangent:
//					{
//						float3 tangent = FromAIVector(normalTransformAI * mesh->mTangents[i]);
//						*(vertexPtr) = tangent.X();
//						*(vertexPtr+1) = tangent.Y();
//						*(vertexPtr+2) = tangent.Z();
//					}			
//					break;
//				case VEU_Binormal:
//					{
//						float3 bitangent = FromAIVector(normalTransformAI * mesh->mBitangents[i]);
//						*(vertexPtr) = bitangent.X();
//						*(vertexPtr+1) = bitangent.Y();
//						*(vertexPtr+2) = bitangent.Z();
//					}			
//					break;
//				case VEU_TextureCoordinate:
//					{
//						switch(mesh->mNumUVComponents[element.UsageIndex])
//						{
//						case 3:
//							*(vertexPtr+2) = mesh->mTextureCoords[element.UsageIndex][i].z;
//						case 2:
//							*(vertexPtr+1) = mesh->mTextureCoords[element.UsageIndex][i].y;	
//						case 1:
//							*(vertexPtr) = mesh->mTextureCoords[element.UsageIndex][i].x;
//							break;
//						}
//					}
//					break;
//				case VEU_BlendWeight:
//					{
//						for (uint32_t j = 0; j < 4; ++j)
//						{
//							if (j < blendWeights[i].size())
//							{
//								float weight = blendWeights[i][j];
//								*vertexPtr++ = weight;
//							}
//							else
//								*vertexPtr++ = 0.0f;
//						}
//					}
//					break;
//				case VEU_BlendIndices:
//					{
//						uint32_t* destBytes = (uint32_t*)vertexPtr;
//						for (uint32_t j = 0; j < 4; ++j)
//						{
//							if (j < blendIndices[i].size())
//							{
//								uint32_t index =  blendIndices[i][j];
//								*destBytes++ = blendIndices[i][j];
//							}
//							else
//								*destBytes++ = 0;
//						}
//					}
//					break;
//
//				default:
//					break;
//				}
//			}
//		}
//
//		meshPart->StartVertex = 0;
//		meshPart->VertexCount = mesh->mNumVertices;
//		meshPart->VertexDeclaration = vertexDecl;
//		meshPart->BoundingSphere = sphere;
//		outModel.MeshParts.push_back(meshPart);
//	}
//
//	// Merge all sub mesh's bouding sphere
//	for (size_t i = 0; i < outModel.MeshParts.size(); ++i)
//	{
//		outModel.MeshBoundingSphere.Merge( outModel.MeshParts[i]->BoundingSphere );
//	}
//}

aiMatrix4x4 GetBoneOffset(OutModel& model, aiString name)
{
	for(size_t i = 0; i < model.Meshes.size(); ++i)
	{
		aiMesh* mesh = model.Meshes[i];
		for (size_t j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			if (name == bone->mName)
			{
				return bone->mOffsetMatrix;
			}
		}
	}
	return aiMatrix4x4();
}

void AssimpProcesser::BuildSkeleton( OutModel& outModel )
{
	//// Build skeleton if necessary
	//if (outModel.Bones.size() && outModel.RootBone)
	//{
	//	shared_ptr<Skeleton> skeleton(new Skeleton);

	//	for (size_t i = 0; i < outModel.Bones.size(); ++i)
	//	{
	//		aiNode* boneNode = outModel.Bones[i];
	//		String boneName(boneNode->mName.C_Str());

	//		Bone* bone = new Bone(boneName, i);

	//		aiMatrix4x4 transform = boneNode->mTransformation;

	//		// Make the root bone transform relative to the model's root node, if it is not already
	//		// This will put the mesh in model coordinate system.
	//		if (boneNode == outModel.RootBone)
	//			transform = GetDerivedTransform(boneNode, outModel.RootNode);

	//		aiVector3D scale, position;
	//		aiQuaternion rot;
	//		transform.Decompose(scale, rot, position);

	//		Quaternionf quat = FromAIQuaternion(rot);
	//		float yaw, roll, pitch;
	//		//QuaternionToRotationYawPitchRoll(yaw, pitch, roll, quat);

	//		bone->SetPosition( FromAIVector(position) );
	//		bone->SetRotation( FromAIQuaternion(rot) );
	//		bone->SetScale( FromAIVector(scale) );

	//		// Get offset information if exists
	//		aiMatrix4x4 offsetMatrix = GetOffsetMatrix(outModel, boneName);
	//		bones.push_back(bone);
	//	}

	//	// Set the bone hierarchy
	//	for (size_t i = 1; i < outModel.Bones.size(); ++i)
	//	{
	//		String parentName(outModel.Bones[i]->mParent->mName.C_Str());

	//		for (size_t j = 0; j < bones.size(); ++j)
	//		{
	//			if (bones[j]->GetName() == parentName)
	//			{
	//				//std::cout << bones[i]->GetName() << "   Parent:" << parentName << std::endl;
	//				bones[j]->AttachChild( bones[i] );
	//				break;
	//			}
	//		}
	//	}

	//	outModel.Skeleton = skeleton;
	//}

	////ValidateOffsetMatrix(outModel);
}

void AssimpProcesser::BuildAndSaveAnimations( OutModel& model )
{

}

void AssimpProcesser::CollectMaterials()
{	
	std::set<String> matNames;

	for (size_t i = 0; i < mAIScene->mNumMaterials; ++i)
	{
		shared_ptr<MaterialData> material = ProcessMaterial(mAIScene->mMaterials[i]);
		
		mModel.Materials.push_back(material);
	}
}

void AssimpProcesser::BuildBoneCollisions()
{
	mModel.BoneSpheres.resize(mModel.Bones.size());
	for (unsigned i = 0; i < mModel.Meshes.size(); ++i)
	{
		aiMesh* mesh = mModel.Meshes[i];
		for (unsigned j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			unsigned boneIndex = GetBoneIndex(mModel, bone->mName);
			if (boneIndex == -1)
				continue;
			aiNode* boneNode = mModel.Bones[boneIndex];
			for (unsigned k = 0; k < bone->mNumWeights; ++k)
			{
				float weight = bone->mWeights[k].mWeight;
				if (weight > 0.33f)
				{
					aiVector3D vertexBoneSpace = bone->mOffsetMatrix * mesh->mVertices[bone->mWeights[k].mVertexId];
					float3 vertex = FromAIVector(vertexBoneSpace);
					mModel.BoneSpheres[boneIndex].Merge(vertex);
				}
			}
		}
	}
}

void ValidateOffsetMatrix( OutModel& outModel ) 
{
	/*vector<Bone*>& bones = outModel.Skeleton->GetBones();

	for (size_t i = 0; i < bones.size(); ++i)
	{
		Bone* bone = bones[i];
		
		float4x4 derivedTransform = bone->GetWorldTransform();
		
		aiNode* node = outModel.RootNode->FindNode(bone->GetName().c_str());
		aiMatrix4x4 mat = GetDerivedTransform(node, outModel.RootNode->FindNode("Scene_Root"));
		mat.Transpose();

		aiMatrix4x4 offset = GetBoneOffset(outModel, aiString(bone->GetName().c_str()));
		offset.Inverse();
		offset.Transpose();

		aiMatrix4x4 derivedTransformInverse = GetOffsetMatrix(outModel, bone->GetName());
		derivedTransformInverse.Inverse();
		derivedTransformInverse.Transpose();

		float4x4 test = FromAIMatrix(derivedTransformInverse);
		
		int a = 0;
	}*/
}