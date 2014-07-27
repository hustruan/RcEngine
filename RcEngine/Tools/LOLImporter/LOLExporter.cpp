#include "LOLExporter.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <IO/FileStream.h>
#include <Core/Loger.h>
#include <IO/PathUtil.h>
#include <Graphics/VertexDeclaration.h>
#include <Math/MathUtil.h>

using namespace std;
using namespace RcEngine;


void CorrectName(String& matName)
{
	std::replace(matName.begin(), matName.end(), ':', '_');
}

LOLExporter::LOLExporter(void)
	: mDummyRootAdded(false)
{
}

LOLExporter::~LOLExporter(void)
{
}

//void LOLExporter::ImportSkl( const char* sklFilename )
//{
//	//errno_t err;
//	//size_t numRead;
//
//	//* pFile = NULL;
//	//err = fopen_s(&pFile, sklFilename, "rb");
//	//if (err != 0)
//	//{
//	//	printf("ERROR: could not open %s\n", sklFilename);
//	//	exit(1);
//	//}
//
//	//static const char SkeletonFileType[] = "r3d2sklt";
//
//	//Header header;
//	//fread_s(&header, sizeof(header), sizeof(header), 1, pFile);
//
//	//if (strncmp(header.fileType, SkeletonFileType, 8) == 0)
//	//{
//	//	mBones.resize(header.numElements);
//	//	for (int i = 0; i < header.numElements; i++) 
//	//	{
//	//		LOLBone& bone = mBones[i];
//	//		fread_s(&bone, sizeof(bone), sizeof(bone), 1, pFile);
//
//	//		printf("Bone: %s, Parent: %s\n", bone.name, (bone.parent != -1 ) ? mBones[bone.parent].name : "");
//	//	}
//
//	//	int32_t id;
//	//	while ( fread_s(&id, 4, 4, 1, pFile) )
//	//	{
//	//		printf("boneid: %d\n", id);
//	//	}
//
//	//	//int boneSize = sizeof(Bone);
//	//	//long int currPos = ftell(pFile);
//	//	//fseek(pFile, 0, SEEK_END);
//	//	//long int left = ftell(pFile) - currPos;
//	//}
//	//else
//	//{
//	//	printf("Unsupported skeleton format!\n");
//	//}
//	//
//
//	//// if exit multiple bone with no parent, add a dummy root node 
//	//uint32_t numRootBones = std::count_if(mBones.begin(), mBones.end(), [&](const LOLBone& bone) { return bone.parent == -1;} );
//	//if (numRootBones > 1)
//	//{
//	//	printf("\nAdd dummy root node!\n\n");
//
//	//	LOLBone dummyRoot;
//	//	sprintf_s(dummyRoot.name, "dummyRoot");
//	//	
//	//	dummyRoot.parent = -1;
//
//	//	dummyRoot.matrix[0][0] = 1.0f; dummyRoot.matrix[0][1] = 0.0f; dummyRoot.matrix[0][2] = 0.0f; dummyRoot.matrix[0][3] = 0.0f;
//	//	dummyRoot.matrix[1][0] = 0.0f; dummyRoot.matrix[1][1] = 1.0f; dummyRoot.matrix[1][2] = 0.0f; dummyRoot.matrix[1][3] = 0.0f;
//	//	dummyRoot.matrix[2][0] = 0.0f; dummyRoot.matrix[2][1] = 0.0f; dummyRoot.matrix[2][2] = 1.0f; dummyRoot.matrix[2][3] = 0.0f;
//	//
//	//	dummyRoot.scale = mBones.front().scale;
//
//	//	mBones.insert(mBones.begin(), dummyRoot);
//
//	//	for (size_t i = 1; i < mBones.size(); ++i)
//	//	{
//	//		LOLBone& bone = mBones[i];
//	//		mBones[i].parent += 1;
//	//		printf("Bone: %s, Parent: %s\n", bone.name, (bone.parent != -1 ) ? mBones[bone.parent].name : "");
//	//	}
//
//	//	mDummyRootAdded = true;
//	//}
//
//	//printf("\nBuild Skeleton \n");
//	//mSkeleton = std::make_shared<Skeleton>();
//	//mSkeleton->AddBone(mBones[0].name, nullptr);
//	//for (size_t i = 1; i < mBones.size(); ++i)
//	//{
//	//	Bone* parent = mSkeleton->GetBone(mBones[mBones[i].parent].name);
//	//	Bone* newBone = mSkeleton->AddBone(mBones[i].name, parent);
//	//	
//	//	printf("Bone: %s, Parent: %s\n", newBone->GetName().c_str(), parent->GetName().c_str());
//	//}
//
//	//
//	//for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
//	//{
//	//	const LOLBone& bone = mBones[i];
//	//	Bone* engineBone = mSkeleton->GetBone(i);
//
//	//	float4x4 rotation(
//	//		bone.matrix[0][0], bone.matrix[1][0], bone.matrix[2][0], 0.0f,
//	//		bone.matrix[0][1], bone.matrix[1][1], bone.matrix[2][1], 0.0f,
//	//		bone.matrix[0][2], bone.matrix[1][2], bone.matrix[2][2], 0.0f,
//	//		0.0f,			   0.0f,			  0.0f,			     1.0f);
//
//	//	Quaternionf rotQuat = QuaternionFromRotationMatrix(rotation);
//
//	//	//engineBone->SetScale(float3(bone.scale, bone.scale, bone.scale));
//	//	engineBone->SetWorldPosition(float3(bone.matrix[0][3], bone.matrix[1][3], bone.matrix[2][3]));
//	//	engineBone->SetWorldRotation(rotQuat);
//	//}
//
//	//mOutputPath = sklFilename;
//	//PathUtil::SplitPath(mOutputPath, mOutputPath, mMeshName, mAnimationName);
//}

void LOLExporter::ImportSkl( const String& sklFilename )
{
	FileStream stream;
	if (!stream.Open(sklFilename))
	{
		printf("ERROR: could not open %s\n", sklFilename.c_str());
		exit(1);
	}

	char id[8];
	stream.Read(id, 8);
	mLOLSkeleton.Version = stream.ReadUInt();

	if (mLOLSkeleton.Version == 1 || mLOLSkeleton.Version == 2)
	{
		uint32_t designerID = stream.ReadUInt();

		char nameBuffer[32];
		float matrix[12];

		// Read in the bones.
		uint32_t numBones = stream.ReadUInt();
		mLOLSkeleton.Bones.resize(numBones);
		for (uint32_t i = 0; i < numBones; ++i)
		{
			LOLBone& bone = mLOLSkeleton.Bones[i];

			stream.Read(nameBuffer, 32);
			bone.Name = nameBuffer;

			bone.Index = i;
			bone.ParentIndex = stream.ReadInt();
			bone.Scale = stream.ReadFloat();

			// Read in transform matrix.
			stream.Read(matrix, sizeof(matrix));

			float4x4 rotation(
				matrix[0], matrix[4], matrix[8],  0.0f,
				matrix[1], matrix[5], matrix[9],  0.0f,
				matrix[2], matrix[6], matrix[10], 0.0f,
				0.0f,	   0.0f,	  0.0f,		  1.0f);

			bone.Orientation = QuaternionFromRotationMatrix(rotation);
			bone.Position = float3(matrix[3], matrix[7], matrix[11]);
		}

		// Version two contains bone IDs.
		if (mLOLSkeleton.Version == 2)
		{
			uint32_t numBoneIDs = stream.ReadUInt();
			for (uint32_t i = 0; i < numBoneIDs; ++i)
				mLOLSkeleton.BoneIDs.push_back(stream.ReadUInt());
		}
	}
	else if (mLOLSkeleton.Version == 0)
	{
		uint16_t zero = stream.ReadUShort();
		uint16_t numBones = stream.ReadUShort();
		uint32_t numBoneIDs = stream.ReadUInt();
		uint16_t offsetToVertexData = stream.ReadUShort(); // Should be 64.

		int unknown = stream.ReadShort(); // ?

		int offset1 = stream.ReadInt();
		int offsetToAnimationIndices = stream.ReadInt();
		int offset2 = stream.ReadInt();
		int offset3 = stream.ReadInt();
		int offsetToStrings = stream.ReadInt();

		// Not sure what this data represents.
		// I think it's padding incase more header data is required later.
		//stream.Seek(stream.GetPosition() + 20);

		mLOLSkeleton.Bones.resize(numBones);
		stream.Seek(offsetToVertexData);	
		for (int i = 0; i < numBones; ++i)
		{
			LOLBone& bone = mLOLSkeleton.Bones[i];

			// The old scale was always 0.1. For now, just go with it.
			bone.Scale = 0.1f;

			zero = stream.ReadShort(); // ?
			bone.Index = stream.ReadShort();
			bone.ParentIndex = stream.ReadShort();
			unknown = stream.ReadShort(); // ?

			int namehash = stream.ReadInt();
			float twoPointOne = stream.ReadFloat();

			stream.Read(&bone.Position, sizeof(float3));

			float one = stream.ReadFloat(); // ? Maybe scales for X, Y, and Z
			one = stream.ReadFloat();
			one = stream.ReadFloat();

			stream.Read(&bone.Orientation, sizeof(Quaternionf));

			float ctx = stream.ReadFloat(); // ctx
			float cty = stream.ReadFloat(); // cty
			float ctz = stream.ReadFloat(); // ctz

			// The rest of the bone data is unknown. Maybe padding?
			stream.Seek(stream.GetPosition() + 32);
		}

		stream.Seek(offset1);
		for (uint32_t i = 0; i < numBones; ++i) // Inds for version 4 animation.
		{
			// 8 bytes
			uint32_t sklID = stream.ReadUInt();
			uint32_t anmID = stream.ReadUInt();
			mLOLSkeleton.BoneIDMap[anmID] = sklID;
		}

		stream.Seek(offsetToAnimationIndices);
		for (uint32_t i = 0; i < numBoneIDs; ++i) // Inds for animation
		{
			// 2 bytes
			uint16_t boneID = stream.ReadUShort();
			mLOLSkeleton.BoneIDs.push_back(boneID);
		}

		stream.Seek(offsetToStrings);
		char nameBuffer[4];
		for (int i = 0; i < numBones; ++i)
		{
			bool finished = false;
			do 
			{
				stream.Read(nameBuffer, 4);
				for (char c : nameBuffer)
				{
					if (c == '\0')
					{
						finished = true;
						break;
					}
					
					mLOLSkeleton.Bones[i].Name.push_back(c);
				}
			} while (!finished);		
		}
	}
}

void LOLExporter::ImportSkn( const String& sknFilename )
{
	FileStream file;
	if (!file.Open(sknFilename))
	{
		printf("ERROR: could not open %s\n", sknFilename.c_str());
		exit(1);
	}

	uint32_t magic = file.ReadInt();

	uint16_t version = file.ReadUShort();
	uint16_t numObjects = file.ReadUShort();

	mLOLSkinMesh.Version = version;
	if (version == 1 || version == 2)
	{
		// Contains material headers.
		uint32_t numParts = file.ReadUInt();

		char nameBuffer[64];

		mLOLSkinMesh.MeshParts.resize(numParts);
		for (uint32_t i = 0; i < numParts; ++i)
		{
			// Read in the headers.
			LOLSkinMesh::MeshPart& meshPart = mLOLSkinMesh.MeshParts[i];

			file.Read(nameBuffer, sizeof(nameBuffer));
			meshPart.Material = nameBuffer;

			meshPart.StartVertex = file.ReadInt();
			meshPart.VertexCount = file.ReadUInt();
			meshPart.StartIndex = file.ReadInt();
			meshPart.IndexCount = file.ReadUInt();
		}

		uint32_t numIndices = file.ReadUInt();
		uint32_t numVertices = file.ReadUInt();

		mLOLSkinMesh.Indices.resize(numIndices);
		file.Read(&mLOLSkinMesh.Indices[0], numIndices * sizeof(uint16_t));

		mLOLSkinMesh.Verteces.resize(numVertices);
		for (LOLSkinMesh::Vertex& vertex : mLOLSkinMesh.Verteces)
		{
			file.Read(&vertex.Position, sizeof(float3));
			file.Read(&vertex.BoneIndices, sizeof(uint8_t)*4);
			file.Read(&vertex.BoneWeights, sizeof(float)*4);
			file.Read(&vertex.Normal, sizeof(float3));
			file.Read(&vertex.Texcoords, sizeof(float2));

			// Check SkinModelVertex
			/*float totalWeight = 0.0f;
			for (int i = 0; i < 4; ++i) 
			{
				if (vertex.BoneIndices[i] >= mBones.size())
					printf("Bone Index Out of Range!");

				totalWeight += vertex.weights[i];
			}

			if ( fabsf(totalWeight - 1.0f) > 0.001)
				printf("Unnormalized Bone Weights!");

			if ( vertex.texcoords[0] < 0.0f || vertex.texcoords[0] > 1.0f ||
				vertex.texcoords[1] < 0.0f || vertex.texcoords[1] > 1.0f )
				printf("Texcoords Index Out of Range!");*/
		}

		for ( size_t i = 0; i < mLOLSkinMesh.MeshParts.size(); ++i )
		{
			LOLSkinMesh::MeshPart& lolMeshPart = mLOLSkinMesh.MeshParts[i];

			const int32_t StartIndex = lolMeshPart.StartIndex;
			const int32_t EndIndex = lolMeshPart.StartIndex + lolMeshPart.IndexCount;
			for (int32_t j = StartIndex; j < EndIndex; ++j)
			{
				uint16_t index = mLOLSkinMesh.Indices[j];
				const LOLSkinMesh::Vertex& vertex = mLOLSkinMesh.Verteces[index];
				lolMeshPart.Bound.Merge(vertex.Position);
			}

			mLOLSkinMesh.Bound.Merge(lolMeshPart.Bound);
		}
	}
	else
	{
		printf("Unsupported Skn format!\n");
		exit(1);
	}

	printf("SkinnedMesh %s\n", sknFilename.c_str());
	printf("Version: %d\n", mLOLSkinMesh.Version);
	printf("Number of Objects: %d\n", numObjects);
	printf("Number of Material Headers: %d\n", mLOLSkinMesh.MeshParts.size());
	printf("Number of Vertices: %d\n", mLOLSkinMesh.Verteces.size());
	printf("Number of Indices: %d\n", mLOLSkinMesh.Indices.size());
}

void LOLExporter::ImportAnm( const String& anmFilename )
{
	FileStream file;
	if (!file.Open(anmFilename))
	{
		printf("ERROR: could not open %s\n", anmFilename.c_str());
		exit(1);
	}

	char id[8];
	file.Read(id, sizeof(id));
	
	uint32_t version = file.ReadUInt();
	mLOLAnimation.Version = version;

	// Version 0, 1, 2, 3 Code
	if (version == 0 || version == 1 || version == 2 || version == 3)
	{
		uint32_t magic = file.ReadUInt();
		uint32_t numBones = file.ReadUInt();
		uint32_t numFrames = file.ReadUInt();
		uint32_t playbackFPS = file.ReadUInt();

		char nameBuffer[32];

		// Read in all the bones
		mLOLAnimation.Clip.AnimationTracks.resize(numBones);
		for (uint32_t i = 0; i < numBones; ++i)
		{
			LOLAnimation::AnimationClip::AnimationTrack& animTrack = mLOLAnimation.Clip.AnimationTracks[i];

			file.Read(nameBuffer, sizeof(nameBuffer));
			animTrack.BoneName = nameBuffer;
			
			// Unknown
			uint32_t boneType = file.ReadUInt();

			// For each bone, read in its value at each frame in the animation.
			animTrack.KeyFrames.resize(numFrames);
			for (LOLAnimation::AnimationClip::KeyFrame& frame : animTrack.KeyFrames)
			{
				// Read in the frame's quaternion.
				frame.Orientation[3] = file.ReadFloat(); // x
				frame.Orientation[1] = file.ReadFloat(); // y
				frame.Orientation[2] = file.ReadFloat(); // z
				frame.Orientation[0] = file.ReadFloat(); // w

				// Read in the frame's position.
				file.Read(&frame.Position, sizeof(float3));
			}
		}
	}
	else if (version == 4)
	{
		uint32_t magic = file.ReadUInt();

		// Not sure what any of these mean.
		float unknown = file.ReadFloat();
		unknown = file.ReadFloat();
		unknown = file.ReadFloat();

		uint32_t numBones = file.ReadUInt();
		uint32_t numFrames = file.ReadUInt();
		uint32_t playbackFPS = (uint32_t)(1.0f / file.ReadFloat() + 0.5f);

		// These are offsets to specific data sections in the file.
		uint32_t unknownOffset = file.ReadUInt();
		unknownOffset = file.ReadUInt();
		unknownOffset = file.ReadUInt();

		uint32_t positionOffset = file.ReadUInt();
		uint32_t orientationOffset = file.ReadUInt();
		uint32_t indexOffset = file.ReadUInt();

		// These last three values are confusing.
		// They aren't a vector and they throw off the offset values
		// by 12 bytes. Just ignore them and keep reading.
		unknownOffset = file.ReadUInt();
		unknownOffset = file.ReadUInt();
		unknownOffset = file.ReadUInt();

		//
		// Vector section.
		//

		std::vector<float> positions;
		uint32_t numPositions = (orientationOffset - positionOffset) / sizeof(float);
		for (uint32_t i = 0; i < numPositions; ++i)
			positions.push_back(file.ReadFloat());

		//
		// Quaternion section.
		//

		std::vector<float> orientations;
		uint32_t numOrientations = (indexOffset - orientationOffset) / sizeof(float);
		for (uint32_t i = 0; i < numOrientations; ++i)
			orientations.push_back(file.ReadFloat());

		//
		// Offset section.
		//
		// Note: Unlike versions 0-3, data in this version is
		// Frame 1:
		//      Bone 1:
		//      Bone 2:
		// ...
		// Frame 2:
		//      Bone 1:
		// ...
		//
		//Dictionary<UInt32, ANMBone> boneMap = new Dictionary<UInt32, ANMBone>();
		for (uint32_t i = 0; i < numBones; ++i)
		{
			// The first frame is a special case since we are allocating bones
			// as we read them in.

			// Read in the offset data.
			uint32_t boneID = file.ReadUInt();
			uint16_t positionID = file.ReadUShort();
			uint16_t unknownIndex = file.ReadUShort(); // Unknown.
			uint16_t orientationID = file.ReadUShort();
			unknownIndex = file.ReadUShort(); // Unknown. Seems to always be zero.

			// Allocate the bone.
			//ANMBone bone = new ANMBone();
			//bone.id = boneID;

			//// Allocate all the frames for the bone.
			//for (int j = 0; j < numBones; ++j)
			//{
			//	bone.frames.Add(new ANMFrame());
			//}

			//// Retrieve the data for the first frame.
			//ANMFrame frame = bone.frames[0];
			//frame.position = LookUpVector(positionID, positions);
			//frame.orientation = LookUpQuaternion(orientationID, orientations);

			//// Store the bone in the dictionary by bone ID.
			//boneMap[boneID] = bone;
		}
	}	
}

//void LOLExporter::ImportMesh( const char* sknFilename )
//{
//	errno_t err;
//	size_t numRead;
//
//	FILE* pFile = NULL;
//	err = fopen_s(&pFile, sknFilename, "rb");
//	if (err != 0)
//	{
//		printf("ERROR: could not open %s\n", sknFilename);
//		exit(1);
//	}
//
//	SkinModelHeader header;
//	numRead = fread_s(&header, sizeof(SkinModelHeader), sizeof(SkinModelHeader), 1, pFile);
//
//	mMaterials.resize(header.numMaterials);
//	for (int i = 0; i < header.numMaterials; i++) 
//	{
//		SkinModelMaterial& mat = mMaterials[i];
//		numRead = fread_s(&mat, sizeof(SkinModelMaterial), sizeof(SkinModelMaterial), 1, pFile);
//	}
//
//	numRead = fread_s(&mSkinModelData.numIndices, sizeof(int32_t), sizeof(int32_t), 1, pFile);
//	numRead = fread_s(&mSkinModelData.numVertices, sizeof(int32_t), sizeof(int32_t), 1, pFile);
//	
//	/*long int pCurr = ftell(pFile);
//	fseek(pFile, 0, SEEK_END);
//	long int pEnd = ftell(pFile);
//	fseek(pFile, 0, SEEK_SET);
//	fseek(pFile, pCurr, SEEK_SET);
//
//	long int totalLeft = pEnd - pCurr;*/
//
//	//uint32_t meshSize = sizeof(SkinModelVertex) * mSkinModelData.numVertices + sizeof(uint16_t) * mSkinModelData.numIndices;
//	//long int remain = totalLeft - meshSize;
//
//	mSkinModelData.indices.resize(mSkinModelData.numIndices);
//	size_t indexBufferSize = mSkinModelData.numIndices * sizeof(uint16_t);
//	fread_s(&mSkinModelData.indices[0], indexBufferSize, sizeof(uint16_t), mSkinModelData.numIndices, pFile);
//
//	mSkinModelData.verteces.resize(mSkinModelData.numVertices);
//	size_t vertexBufferSize = mSkinModelData.numVertices * sizeof(SkinModelVertex);
//	fread_s(&mSkinModelData.verteces[0], vertexBufferSize, sizeof(SkinModelVertex), mSkinModelData.numVertices, pFile);
//
//	// Check SkinModelVertex
//	for (const SkinModelVertex& vertex : mSkinModelData.verteces)
//	{
//		float totalWeight = 0.0f;
//		for (int i = 0; i < 4; ++i) 
//		{
//			if (vertex.boneIndex[i] >= mBones.size())
//				printf("Bone Index Out of Range!");
//
//			totalWeight += vertex.weights[i];
//		}
//		
//		if ( fabsf(totalWeight - 1.0f) > 0.001)
//			printf("Unnormalized Bone Weights!");
//
//		if ( vertex.texcoords[0] < 0.0f || vertex.texcoords[0] > 1.0f ||
//			 vertex.texcoords[1] < 0.0f || vertex.texcoords[1] > 1.0f )
//			 printf("Texcoords Index Out of Range!");
//	}
//
//	mMeshPartBounds.resize(mMaterials.size());
//	for ( size_t i = 0; i < mMaterials.size(); ++i )
//	{
//		BoundingBoxf meshPartBound = mMeshPartBounds[i];
//		const SkinModelMaterial& material = mMaterials[i];
//		for (int32_t j = material.startIndex; j < material.startIndex + material.numIndices; ++j)
//		{
//			uint16_t index = mSkinModelData.indices[j];
//			const SkinModelVertex& vertex = mSkinModelData.verteces[index];
//			meshPartBound.Merge(vertex.position);
//		}
//		
//		mMeshBound.Merge(meshPartBound);
//	}
//}
//
//void LOLExporter::ExportObj( const char* objFilename )
//{
//	//exports this skin model as a .obj stream
//	ofstream fout(objFilename);
//	for (size_t i = 0; i < mSkinModelData.verteces.size(); i++) {
//		fout << "v " << mSkinModelData.verteces[i].position[0] << " " << mSkinModelData.verteces[i].position[1] << " " << mSkinModelData.verteces[i].position[2] << endl;
//		fout << "vn " << mSkinModelData.verteces[i].normal[0] << " " << mSkinModelData.verteces[i].normal[1] << " " << mSkinModelData.verteces[i].normal[2] << endl;
//		fout << "vt " << mSkinModelData.verteces[i].texcoords[0] << " " << 1-mSkinModelData.verteces[i].texcoords[1] << endl;
//	}
//	if (mMaterials.size()) {
//		fout << "g mat_" << mMaterials[0].name << std::endl;
//	}
//	for (int i = 0;  i < mSkinModelData.numIndices/3; i++) {
//		int a = mSkinModelData.indices[i*3] + 1;
//		int b = mSkinModelData.indices[i*3 + 1] + 1;
//		int c = mSkinModelData.indices[i*3 + 2] + 1; 
//		fout << "f " << a << '/' << a << '/' << a << " " << b << '/' << b << '/' << b << " " << c << '/' << c << '/' << c << endl;
//	}
//}
//
//void LOLExporter::ImportAnimation( const char* animFilename )
//{
//	errno_t err;
//	size_t numRead;
//
//	FILE* pFile = NULL;
//	err = fopen_s(&pFile, animFilename, "rb");
//	if (err != 0)
//	{
//		printf("ERROR: could not open %s\n", animFilename);
//		exit(1);
//	}
//
//	static const char AninationFileType[] = "r3d2anmd";
//
//	fread_s(&mAnimationHeader, sizeof(SkinAnimationHeader), sizeof(SkinAnimationHeader), 1, pFile);
//
//	if (strncmp(mAnimationHeader.fileType, AninationFileType, 8) == 0)
//	{
//		mAnimationClipData.mAnimationTracks.resize(mAnimationHeader.numBones);
//		for (int32_t i = 0; i < mAnimationHeader.numBones; ++i)
//		{
//			AnimationClipData::AnimationTrack& anmTrack = mAnimationClipData.mAnimationTracks[i];
//
//			/**
//			 *  name:           str         name of bone
//			 *  boneType:       int         type of bone (2 = root, 0 = ordinary, )
//			 *	quat:           float[4]    quaternion of bone
//			 *	pos:            float[3]    postion of bone
//			 */
//
//			fread_s(&anmTrack.BoneName, 32, 32, 1, pFile);
//			fread_s(&anmTrack.BoneType, 4, 4, 1, pFile);
//
//			anmTrack.KeyFrames.resize(mAnimationHeader.numFrames);
//			for (int32_t i = 0; i < mAnimationHeader.numFrames; ++i)
//			{
//				fread_s(&anmTrack.KeyFrames[i], sizeof(AnimationClipData::KeyFrame), sizeof(AnimationClipData::KeyFrame), 1, pFile);
//			}
//		}
//	}
//	else
//	{
//		printf("Unsupported skeleton format!\n");
//	}
//
//	fclose(pFile);
//
//	String dummy;
//	PathUtil::SplitPath(animFilename, dummy, mAnimationName, dummy);
//}

//void LOLExporter::BuildAndSaveBinary()
//{
//	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');
//
//	FileStream stream;
//	stream.Open(mOutputPath + mMeshName + ".mesh", FILE_WRITE);
//
//	String objFile = mOutputPath + mMeshName + ".obj";
//	ExportObj(objFile.c_str());
//
//	// Write mesh id
//	stream.WriteUInt(MeshId);
//
//	// write mesh name
//	stream.WriteString(mMeshName);
//
//	// write mesh bounding box
//	stream.Write(&mMeshBound.Min, sizeof(float3));
//	stream.Write(&mMeshBound.Max, sizeof(float3));
//
//	// write mesh part count
//	stream.WriteUInt( mMaterials.size() );
//	stream.WriteUInt( mBones.size() );
//	stream.WriteUInt( 1 );
//	stream.WriteUInt( 1 );
//
//	// Write mesh part 
//	for (size_t mpi = 0; mpi < mMaterials.size(); ++mpi)
//	{
//		const SkinModelMaterial& meshPart = mMaterials[mpi];
//
//		// write sub mesh name
//		stream.WriteString("Dummy");	
//
//		String MaterialName = meshPart.name;
//		CorrectName(MaterialName);
//
//		// write material name
//		stream.WriteString(MaterialName + ".material.xml");
//		std::cout << "MaterialName: " << MaterialName << std::endl;
//
//		// write sub mesh bounding sphere
//		stream.Write(&mMeshPartBounds[mpi].Min, sizeof(float3));
//		stream.Write(&mMeshPartBounds[mpi].Max, sizeof(float3));
//
//		stream.WriteUInt(0);
//		stream.WriteUInt(0);
//
//		// write vertex count and vertex size
//		stream.WriteUInt(meshPart.startIndex);
//		stream.WriteUInt(meshPart.numIndices);
//		stream.WriteInt(0);
//	}
//
//	// Write skeleton
//	for (size_t iBone = 0; iBone < mBones.size(); ++iBone)
//	{
//		Bone* bone = mSkeleton->GetBone(iBone);
//		Bone* parentBone = static_cast<Bone*>(bone->GetParent());
//
//		float3 pos = bone->GetPosition();
//		float3 scale = bone->GetScale();
//		Quaternionf rot = bone->GetRotation();
//
//		stream.WriteString(bone->GetName());
//		stream.WriteInt(parentBone ? parentBone->GetBoneIndex() : -1);
//
//		stream.Write(&pos, sizeof(float3));
//		stream.Write(&rot, sizeof(Quaternionf));
//		stream.Write(&scale, sizeof(float3));
//	}
//
//	uint32_t offset = 0;
//	std::vector<VertexElement> vertexElements;
//	vertexElements.push_back( VertexElement(offset, VEF_Float3, VEU_Position, 0) ); offset += 12;
//	vertexElements.push_back( VertexElement(offset, VEF_Float4, VEU_BlendWeight, 0) );  offset += 16;
//	vertexElements.push_back( VertexElement(offset, VEF_UInt4, VEU_BlendIndices, 0) );  offset += 16;
//	vertexElements.push_back( VertexElement(offset, VEF_Float3, VEU_Normal, 0) );  offset += 12;
//	vertexElements.push_back( VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 0) );  offset += 8;
//
//
//	// Write vertex and index buffer
//	stream.WriteUInt(mSkinModelData.numVertices); // Vertex Count
//	stream.WriteUInt(vertexElements.size());   // Vertex Size
//	for (const VertexElement& ve : vertexElements)
//	{
//		stream.WriteUInt(ve.Offset);
//		stream.WriteUInt(ve.Type);
//		stream.WriteUInt(ve.Usage);
//		stream.WriteUShort(ve.UsageIndex);
//	}
//
//	int dummyRootNode = mDummyRootAdded ? 1 : 0;
//	for (const SkinModelVertex& vertex : mSkinModelData.verteces)
//	{
//		stream.Write(&vertex.position, sizeof(vertex.position));
//		stream.Write(&vertex.weights, sizeof(vertex.weights));
//		stream.WriteUInt(vertex.boneIndex[0] + dummyRootNode);
//		stream.WriteUInt(vertex.boneIndex[1] + dummyRootNode);
//		stream.WriteUInt(vertex.boneIndex[2] + dummyRootNode);
//		stream.WriteUInt(vertex.boneIndex[3] + dummyRootNode);
//		stream.Write(&vertex.normal, sizeof(vertex.normal));
//		stream.Write(&vertex.texcoords, sizeof(vertex.texcoords));
//	}
//
//	stream.WriteUInt(mSkinModelData.numIndices);
//	stream.WriteUInt(IBT_Bit16);
//	stream.Write(&mSkinModelData.indices[0], mSkinModelData.indices.size() * sizeof(uint16_t));
//
//	stream.Close();
//
//	String clipName = mAnimationName  + ".anim";
//
//	FileStream clipStream;
//	clipStream.Open(mOutputPath + clipName, FILE_WRITE);
//
//	double frameRate = 1.0 / mAnimationHeader.fps;
//	double Duration = frameRate * mAnimationClipData.mAnimationTracks.size();
//	clipStream.WriteFloat((float)Duration);
//	clipStream.WriteUInt(mAnimationClipData.mAnimationTracks.size());
//
//	for (const AnimationClipData::AnimationTrack& track : mAnimationClipData.mAnimationTracks)
//	{
//		// write track name
//		String trackName = track.BoneName;
//		clipStream.WriteString(trackName);
//		// write track key frame count
//		clipStream.WriteUInt(track.KeyFrames.size());
//
//		double keyTime = 0.0f;
//		const float3 scale(1, 1, 1);
//		for (const AnimationClipData::KeyFrame& key : track.KeyFrames)
//		{
//			// write key time
//			clipStream.WriteFloat((float)keyTime);
//			clipStream.Write(&key.Translation, sizeof(float3));
//			clipStream.WriteFloat(key.Rotation[3]);
//			clipStream.WriteFloat(key.Rotation[0]);
//			clipStream.WriteFloat(key.Rotation[1]);
//			clipStream.WriteFloat(key.Rotation[2]);
//			//clipStream.Write(&key.Rotation, sizeof(Quaternionf));
//			
//			clipStream.Write(&scale, sizeof(float3));
//
//			keyTime += frameRate;
//		}
//	}
//	
//	clipStream.Close();
//}

int main()
{
	LOLExporter exporter;


	exporter.ImportSkl("blitzcrank_skin07.skl");
	exporter.ImportSkn("blitzcrank_skin07.skn");
	exporter.ImportAnm("blitzcrank_skin07_dance.anm");

	//exporter.ImportSkeleton("Lulu_darkcandy.skl");
	//exporter.ImportMesh("Lulu_darkcandy.skn");
	//exporter.ImportAnimation("Lulu_dance.anm");
	//exporter.BuildAndSaveBinary();

	return 1;
}
