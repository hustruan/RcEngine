#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <unordered_map>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Skeleton.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"

#pragma comment(lib, "assimp.lib")

using namespace std;
using namespace RcEngine;

static const uint32_t MAT_AMBIENT_COLOR = 0x1; 
static const uint32_t MAT_DIFFUSE_COLOR = 0x2; 
static const uint32_t MAT_SPECULAR_COLOR = 0x4; 
static const uint32_t MAT_EMISSIVE_COLOR = 0x8; 
static const uint32_t MAT_POWER_COLOR = 0x10; 

struct MaterialData
{
	MaterialData() 
		: MaterialFlags(0) { }

	String Name;
	ColorRGBA Ambient;
	ColorRGBA Diffuse;
	ColorRGBA Specular;
	ColorRGBA Emissive;
	float Power;

	uint32_t MaterialFlags;

	unordered_map<String, String> Textures;
};

struct MeshPartData
{
	String Name;
	String MaterialName;

	IndexBufferType IndexFormat;

	uint32_t StartIndex;
	uint32_t IndexCount;

	uint32_t VertexCount;
	uint32_t StartVertex;

	shared_ptr<VertexDeclaration> VertexDeclaration;

	BoundingSpheref BoundingSphere;

	vector<char> IndexData;
	vector<char> VertexData;
};




struct OutModel
{
	OutModel() 
		: RootBone(NULL), RootNode(NULL), TotalVertices(0), TotalIndices(0) 
	{

	}

	String OutName;
	vector<aiMesh*> Meshes;
	vector<aiNode*> MeshNodes;
	vector<aiNode*> Bones;
	vector<aiAnimation*> Animations;
	vector<BoundingSpheref> BoneSpheres;
	aiNode* RootNode;
	aiNode* RootBone;
	uint32_t TotalVertices;
	uint32_t TotalIndices;
	vector<shared_ptr<MeshPartData> > MeshParts;
	vector<shared_ptr<MaterialData> > Materials;
	BoundingSpheref MeshBoundingSphere;
	shared_ptr<Skeleton> Skeleton;
};

class AssimpProcesser
{
public:
	AssimpProcesser(void);
	~AssimpProcesser(void);

	bool Process(const char* filePath, const char* skeleton, const vector<String>& clips);

private:
	void ProcessScene(const aiScene* scene);

	shared_ptr<MaterialData> ProcessMaterial(aiMaterial* material);

	void ProcessAnimations();

	void ExportXML( OutModel& outModel);
	void ExportBinary( OutModel& outModel );
	void GetBoundingBox(const aiScene* scene, aiVector3D* min, aiVector3D* max);

	
	void ExportModel( OutModel& outModel, const String& outName );

private:
	void CollectMaterials();
	void CollectMeshes(OutModel& outModel, aiNode* rootNode);
	void CollectBones(OutModel& outModel);
	void CollectBonesFinal(vector<aiNode*>& bones, const set<aiNode*>& necessary, aiNode* node);
	void CollectAnimations(OutModel& model, aiScene* scene);

	void BuildAndSaveModel(OutModel& outModel);
	void BuildAndSaveAnimations(OutModel& model);
	void BuildBoneCollisions();

	void BuildSkeleton(OutModel& model);


private:
	aiScene* mAIScene;
	String mSkeletonFile;
	String mFilename;
	vector<String> mAnimationClips;
	String mName;
	
	OutModel mModel;
};