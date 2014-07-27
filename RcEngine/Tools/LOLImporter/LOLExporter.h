#pragma once
#ifndef LOLExporter_h__
#define LOLExporter_h__

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <Core/Prerequisites.h>
#include <Math/BoundingBox.h>
#include <Graphics/Skeleton.h>
#include <Math/Quaternion.h>

using RcEngine::float3;
using RcEngine::float2;
using RcEngine::Quaternionf;
using RcEngine::BoundingBoxf;

//Structure for header on skl files
struct Header 
{
	char fileType[8];
	int32_t numObjects;
	int32_t skeletonHash;
	int32_t numElements;
};

//Structure for a bone in skl files
struct LOLBone 
{
	String Name;
	int32_t Index;
	int32_t ParentIndex;
	float Scale;
	float3 Position;
	Quaternionf Orientation;
};

struct LOLSkeleton
{
	uint32_t Version;

	std::vector<LOLBone> Bones;
	std::vector<uint32_t> BoneIDs;

	// Maps .skl bone ID's to version 4 .anm bone ID's.
	std::unordered_map<uint32_t, uint32_t> BoneIDMap;
};

struct LOLSkinMesh
{
	struct MeshPart
	{
		String Material;
		int32_t StartVertex;
		uint32_t VertexCount;
		int32_t StartIndex;
		uint32_t IndexCount;

		BoundingBoxf Bound;
	};

	struct Vertex
	{
		float3 Position;
		uint8_t BoneIndices[4];
		float BoneWeights[4];
		float3 Normal;
		float2 Texcoords;
	};

	uint32_t Version;
	BoundingBoxf Bound;
	std::vector<MeshPart> MeshParts;
	
	std::vector<uint16_t> Indices;
	std::vector<Vertex> Verteces;
};

struct LOLAnimation
{
	struct AnimationClip
	{
		struct KeyFrame
		{
			Quaternionf Orientation;
			float3 Position;
		};

		struct AnimationTrack
		{
			String BoneName;
			std::vector<KeyFrame> KeyFrames;
		};

		std::vector<AnimationTrack> AnimationTracks;
	};

	uint32_t Version;
	AnimationClip Clip;
};


class LOLExporter
{
public:
	LOLExporter(void);
	~LOLExporter(void);

	void ImportSkl(const String& sklFilename);
	void ImportSkn(const String& sknFilename);
	void ImportAnm(const String& anmFilename);

	/*void ImportSkl(const char* sklFilename);
	void ImportSkn(const char* sknFilename);
	void ImportAnimation(const char* animFilename);*/

	void BuildAndSaveBinary();

	void ExportObj(const char* objFilename);

private:

	String mOutputPath;
	String mAnimationName;
	String mMeshName;
	
	LOLSkeleton mLOLSkeleton;
	LOLSkinMesh mLOLSkinMesh;
	LOLAnimation mLOLAnimation;

	std::shared_ptr<RcEngine::Skeleton> mSkeleton;
	bool mDummyRootAdded;
};

#endif // LOLExporter_h__
