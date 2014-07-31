#include "FbxImporter.h"
#include "OgreMeshFileFormat.h"
#include <IO/FileStream.h>
#include <IO/PathUtil.h>
#include <Core/Exception.h>
#include <Graphics/Skeleton.h>

const uint16_t HEADER_STREAM_ID = 0x1000;
const uint16_t OTHER_ENDIAN_HEADER_STREAM_ID = 0x0010;
const size_t STREAM_OVERHEAD_SIZE = sizeof(uint16_t) + sizeof(uint32_t);
const size_t BONE_CHUNK_SIZE = STREAM_OVERHEAD_SIZE + sizeof(unsigned short) + sizeof(float3) + sizeof(Quaternionf);
const size_t KEY_CHUNK_SIZE = STREAM_OVERHEAD_SIZE + sizeof(float) + sizeof(float3) + sizeof(Quaternionf);

struct OgreBone
{
	unsigned short handle;
	String Name;
	float3 Translation;
	Quaternionf Rotation;
	float3 Scale;
};

#define RIGHT2LEFT

static unsigned int mCurrentstreamLen;

void determineEndianness(FileStream& stream)
{
	bool mFlipEndian = false;

	uint16_t dest;
	// read header id manually (no conversion)
	size_t actually_read = stream.Read(&dest, sizeof(uint16_t));

	stream.Seek(stream.GetPosition() - (long)actually_read);
	// skip back
	if (actually_read != sizeof(uint16_t))
	{
		int a = 0;
	}
	if (dest == HEADER_STREAM_ID)
	{
		mFlipEndian = false;
	}
	else if (dest == OTHER_ENDIAN_HEADER_STREAM_ID)
	{
		mFlipEndian = true;
	}
	else
	{

	}
}

unsigned short readChunk(FileStream& stream)
{
	unsigned short id = stream.ReadUShort();
	mCurrentstreamLen = stream.ReadUInt();
	return id;
}

struct OgreVertexElement
{
	/// The source vertex buffer, as bound to an index using VertexBufferBinding
	unsigned short Source;
	/// The offset in the buffer that this element starts at
	size_t Offset;
	/// The type of element
	VertexElementType Type;
	/// The meaning of the element
	VertexElementSemantic Semantic;
	/// Index of the item, only applicable for some elements like texture coords
	unsigned short Index;
};

String ReadString(FileStream& stream)
{
	String ret;
	for (;;)
	{
		char c = stream.ReadByte();
		if (c == '\n')
			break;
		else
			ret.push_back(c);
	}
	return ret;
}

OgreVertexElement readGeometryVertexElement(FileStream& stream)
{
	OgreVertexElement element;
		
	element.Source = stream.ReadUShort();
	element.Type = static_cast<VertexElementType>(stream.ReadUShort());
	element.Semantic = static_cast<VertexElementSemantic>(stream.ReadUShort());
	element.Offset = stream.ReadUShort();
	element.Index = stream.ReadUShort();
	
	return element;
}

void readGeometryVertexDeclaration(FileStream& stream, std::vector<OgreVertexElement>& vertexElememts)
{
	// Find optional geometry streams
	if (!stream.IsEof())
	{
		unsigned short streamID = readChunk(stream);
		while(!stream.IsEof() &&
			(streamID == M_GEOMETRY_VERTEX_ELEMENT ))
		{
			switch (streamID)
			{
			case M_GEOMETRY_VERTEX_ELEMENT:
				vertexElememts.push_back( readGeometryVertexElement(stream) );
				break;
			}
			// Get next stream
			if (!stream.IsEof())
			{
				streamID = readChunk(stream);
			}
		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of non-submesh stream
			stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
		}
	}
}

OperationType readSubMeshOperation(FileStream& stream)
{
	unsigned short opType = stream.ReadUShort();
	return static_cast<OperationType>(opType);
}

void readSubMeshBoneAssignment(FileStream& stream, vector<Vertex>& vertices)
{
	uint32_t vertexIndex = stream.ReadUInt();
	uint16_t boneIndex = stream.ReadUShort();
	float weight = stream.ReadFloat();

	Vertex& vertex = vertices[vertexIndex];

	size_t i = 0;
	while (i < vertex.BlendWeights.size() && vertex.BlendWeights[i] > weight) i++;

	vertex.BlendIndices.insert(vertex.BlendIndices.begin() + i, boneIndex);
	vertex.BlendWeights.insert(vertex.BlendWeights.begin() + i, weight);

	vertices[vertexIndex].Flags |= Vertex::eBlendIndices;
	vertices[vertexIndex].Flags |= Vertex::eBlendWeight;
}

void readSubMeshTextureAlias(FileStream& stream)
{

}

void ReadSubMeshNameTable(FileStream& stream, shared_ptr<MeshData>& pMesh)
{
	std::map<unsigned short, String> subMeshNames;
	unsigned short streamID, subMeshIndex;

	// Need something to store the index, and the objects name
	// This table is a method that imported meshes can retain their naming
	// so that the names established in the modelling software can be used
	// to get the sub-meshes by name. The exporter must support exporting
	// the optional stream M_SUBMESH_NAME_TABLE.

	// Read in all the sub-streams. Each sub-stream should contain an index and Ogre::String for the name.
	if (!stream.IsEof())
	{
		streamID = readChunk(stream);
		while(!stream.IsEof() && (streamID == M_SUBMESH_NAME_TABLE_ELEMENT ))
		{
			// Read in the index of the submesh.
			subMeshIndex = stream.ReadUShort();
			// Read in the String and map it to its index.
			subMeshNames[subMeshIndex] = stream.ReadString();
			stream.Seek(stream.GetPosition() - 1);

			// If we're not end of file get the next stream ID
			if (!stream.IsEof())
				streamID = readChunk(stream);
		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of stream
			stream.Seek(stream.GetPosition() - STREAM_OVERHEAD_SIZE);
		}
	}

	// Set all the submeshes names
	// ?

	// Loop through and save out the index and names.
	for (auto it = subMeshNames.begin(); it != subMeshNames.end(); ++it)
	{
			//pMesh->nameSubMesh(it->second, it->first);
	}
}

void ReadBoundsInfo(FileStream& stream, shared_ptr<MeshData>& pMesh)
{
	float3 min, max;
	stream.Read(&min, sizeof(float3));
	stream.Read(&max, sizeof(float3));

	//pMesh->Bound = BoundingBoxf(min, max);

	// float radius
	float radius = stream.ReadFloat();
}

void ReadEdgeList(FileStream& stream, shared_ptr<MeshData>& pMesh)
{
	unsigned short streamID;

	if (!stream.IsEof())
	{
		streamID = readChunk(stream);
		while(!stream.IsEof() &&
			streamID == M_EDGE_LIST_LOD)
		{
			// Process single LOD

			// unsigned short lodIndex
			unsigned short lodIndex = stream.ReadUShort();

			// bool isManual			// If manual, no edge data here, loaded from manual mesh
			bool isManual = stream.ReadBool();
			// Only load in non-manual levels; others will be connected up by Mesh on demand
			if (!isManual)
			{
				//MeshLodUsage& usage = const_cast<MeshLodUsage&>(pMesh->getLodLevel(lodIndex));

				//usage.edgeData = OGRE_NEW EdgeData();

				//// Read detail information of the edge list
				//readEdgeListLodInfo(stream, usage.edgeData);

				//// Postprocessing edge groups
				//EdgeData::EdgeGroupList::iterator egi, egend;
				//egend = usage.edgeData->edgeGroups.end();
				//for (egi = usage.edgeData->edgeGroups.begin(); egi != egend; ++egi)
				//{
				//	EdgeData::EdgeGroup& edgeGroup = *egi;
				//	// Populate edgeGroup.vertexData pointers
				//	// If there is shared vertex data, vertexSet 0 is that,
				//	// otherwise 0 is first dedicated
				//	if (pMesh->sharedVertexData)
				//	{
				//		if (edgeGroup.vertexSet == 0)
				//		{
				//			edgeGroup.vertexData = pMesh->sharedVertexData;
				//		}
				//		else
				//		{
				//			edgeGroup.vertexData = pMesh->getSubMesh(
				//				(unsigned short)edgeGroup.vertexSet-1)->vertexData;
				//		}
				//	}
				//	else
				//	{
				//		edgeGroup.vertexData = pMesh->getSubMesh(
				//			(unsigned short)edgeGroup.vertexSet)->vertexData;
				//	}
				//}
			}

			if (!stream.IsEof())
			{
				streamID = readChunk(stream);
			}

		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of stream
			stream.Seek(stream.GetPosition() - STREAM_OVERHEAD_SIZE);
		}
	}
}

shared_ptr<Skeleton> BuildOgreSkeleton(std::vector<OgreBone>& bones, const std::unordered_map<unsigned short, unsigned short>& childParents)
{
	shared_ptr<Skeleton> pSkel( new Skeleton );

	std::sort(bones.begin(), bones.end(), [](const OgreBone& a, const OgreBone& b) { return a.handle < b.handle; } );
	for (const OgreBone& ogreBone : bones)
		Bone* childBone = pSkel->AddBone(ogreBone.Name, nullptr);

	for (const auto& kv : childParents)
	{
		Bone* childBone = pSkel->GetBone(kv.first);
		Bone* parentBone = pSkel->GetBone(kv.second);

		printf("Bone: %s Parent: %s\n", childBone->GetName().c_str(), parentBone->GetName().c_str());

		parentBone->AttachChild(childBone);
	}
	
	for (const OgreBone& ogreBone : bones)
	{
		Bone* childBone = pSkel->GetBone(ogreBone.handle);
		childBone->SetTransform(ogreBone.Translation, ogreBone.Rotation);
		childBone->SetScale(ogreBone.Scale);
	}

	return pSkel;
}

void FbxProcesser::LoadOgre( const char* filename )
{
	FileStream stream;
	if (stream.Open(filename) == false)
	{
		return;
	}

#ifdef RIGHT2LEFT
	g_ExportSettings.SwapWindOrder = true;
#endif

	const uint16_t HEADER_STREAM_ID = 0x1000;

	uint16_t header = stream.ReadUShort();
	assert(header == HEADER_STREAM_ID);

	String version = stream.ReadString();
	stream.Seek(0);
	determineEndianness(stream);

	// Read header ID
	header = stream.ReadUShort();
	assert(header == HEADER_STREAM_ID);
	version = ReadString(stream);

	shared_ptr<MeshData> newOgreMesh(new MeshData);
	newOgreMesh->Name = PathUtil::GetFileName(filename);

	unsigned short streamID;
	while(!stream.IsEof())
	{
		streamID = readChunk(stream);
		switch (streamID)
		{
		case M_MESH:
			ImportOgreMesh(stream, newOgreMesh);
			break;
		}

	}

	mSceneMeshes.push_back(newOgreMesh);
}

void FbxProcesser::ImportOgreMesh( FileStream& stream, shared_ptr<MeshData> pMesh)
{
	uint32_t streamID;

	bool skeletallyAnimated;
	stream.Read(&skeletallyAnimated, sizeof(bool));

	if (!stream.IsEof())
	{
		streamID = readChunk(stream);
		while(!stream.IsEof() &&
			(streamID == M_GEOMETRY ||
			streamID == M_SUBMESH ||
			streamID == M_MESH_SKELETON_LINK ||
			streamID == M_MESH_BONE_ASSIGNMENT ||
			streamID == M_MESH_LOD ||
			streamID == M_MESH_BOUNDS ||
			streamID == M_SUBMESH_NAME_TABLE ||
			streamID == M_EDGE_LISTS ||
			streamID == M_POSES ||
			streamID == M_ANIMATIONS ||
			streamID == M_TABLE_EXTREMES))
		{
			switch(streamID)
			{
			case M_GEOMETRY:
				//pMesh->sharedVertexData = OGRE_NEW VertexData();
				//try {
				//	readGeometry(stream, pMesh, pMesh->sharedVertexData);
				//}
				//catch (Exception& e)
				//{
				//	if (e.getNumber() == Exception::ERR_ITEM_NOT_FOUND)
				//	{
				//		// duff geometry data entry with 0 vertices
				//		OGRE_DELETE pMesh->sharedVertexData;
				//		pMesh->sharedVertexData = 0;
				//		// Skip this stream (pointer will have been returned to just after header)
				//		stream->skip(mCurrentstreamLen - STREAM_OVERHEAD_SIZE);
				//	}
				//	else
				//	{
				//		throw;
				//	}
				//}
				//break;
			case M_SUBMESH:
				ReadSubMesh(stream, pMesh);
				break;
			case M_MESH_SKELETON_LINK:
				ReadSkeleton(stream, pMesh);
				break;
			case M_MESH_BONE_ASSIGNMENT:
				assert(false);
				break;
			case M_MESH_LOD:
				//ReadMeshBoneAssignment(stream, pMesh);
				break;
			case M_MESH_BOUNDS:
				ReadBoundsInfo(stream, pMesh);
				break;
			case M_SUBMESH_NAME_TABLE:
				ReadSubMeshNameTable(stream, pMesh);
				break;
			case M_EDGE_LISTS:
			case M_POSES:
			case M_ANIMATIONS:
			case M_TABLE_EXTREMES:
				break;
			}
			if (!stream.IsEof())
			{
				streamID = readChunk(stream);
			}
		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of stream
			stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
		}
	}
	
}

void FbxProcesser::ReadSubMesh(FileStream& stream, shared_ptr<MeshData>& pMesh)
{
	unsigned short streamID;

	shared_ptr<MeshPartData> pSubMesh(new MeshPartData);

	// char* materialName
	String materialName = ReadString(stream);
	size_t lastSlashPos = materialName.find_last_of('/'); 
	if (lastSlashPos != String::npos)
		materialName = materialName.substr(lastSlashPos+1);
	pSubMesh->MaterialName = materialName;

	bool useSharedVertices = stream.ReadBool();

	uint32_t indexStart = 0;
	uint32_t indexCount = stream.ReadUInt();

	pSubMesh->BaseVertex = 0;
	pSubMesh->StartIndex = 0;
	pSubMesh->IndexCount = indexCount;

	bool idx32bit = stream.ReadBool();
	if (indexCount > 0)
	{
		pSubMesh->IndexBufferIndex = pMesh->Indices.size();
		pMesh->Indices.resize(pMesh->Indices.size() + 1);
		pMesh->IndexTypes.resize(pMesh->IndexTypes.size() + 1);

		if (idx32bit)
		{
			std::vector<uint32_t>& indexBuffer = pMesh->Indices.back();
			pMesh->IndexTypes.back() = IBT_Bit32;
			indexBuffer.resize(indexCount);

			stream.Read(&indexBuffer[0], sizeof(uint32_t) * indexCount);
		}
		else // 16-bit
		{
			pMesh->IndexTypes.back() = IBT_Bit16;

			std::vector<uint16_t> indexBuffer(indexCount);
			stream.Read(&indexBuffer[0], sizeof(uint16_t) * indexCount);

			std::vector<uint32_t>& indexBuffer32Bit = pMesh->Indices.back();
			indexBuffer32Bit.reserve(indexCount);

			for (uint16_t index : indexBuffer)
				indexBuffer32Bit.push_back(index);
		}
	}

	// M_GEOMETRY stream (Optional: present only if useSharedVertices = false)
	if (!useSharedVertices)
	{
		streamID = readChunk(stream);
		if (streamID != M_GEOMETRY)
		{
			ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Missing geometry data in mesh file",
				"MeshSerializerImpl::readSubMesh");
		}

		uint32_t vertexStart = 0;
		uint32_t vertexCount = stream.ReadUInt();

		pSubMesh->VertexBufferIndex = pMesh->Vertices.size();
		pMesh->Vertices.resize(pMesh->Vertices.size() + 1);
		vector<Vertex>& vertexBuffer = pMesh->Vertices.back();
		vertexBuffer.resize(vertexCount);

		std::vector<OgreVertexElement> vertexElements;
		vector<uint8_t> vertexBufferRaw;

		// Find optional geometry streams
		if (!stream.IsEof())
		{
			unsigned short streamID = readChunk(stream);
			while(!stream.IsEof() &&
				(streamID == M_GEOMETRY_VERTEX_DECLARATION ||
				streamID == M_GEOMETRY_VERTEX_BUFFER ))
			{
				switch (streamID)
				{
				case M_GEOMETRY_VERTEX_DECLARATION:
					readGeometryVertexDeclaration(stream, vertexElements);
					break;
				case M_GEOMETRY_VERTEX_BUFFER:
					{
						unsigned short bindIndex, vertexSize;
						// unsigned short bindIndex;	// Index to bind this buffer to
						bindIndex = stream.ReadUShort(); 
						// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
						vertexSize = stream.ReadUShort(); 

						// Check for vertex data header
						unsigned short headerID;
						headerID = readChunk(stream);
						if (headerID != M_GEOMETRY_VERTEX_BUFFER_DATA)
						{
							ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Can't find vertex buffer data area",
								"MeshSerializerImpl::readGeometryVertexBuffer");
						}

						vertexBufferRaw.resize(vertexSize * vertexCount);
						stream.Read(&vertexBufferRaw[0], vertexSize * vertexCount);

						uint8_t* pBuffer = &vertexBufferRaw[0];
						for (size_t i = 0; i < vertexCount; ++i)
						{
							for (OgreVertexElement& ve : vertexElements)
							{
								if (ve.Source == bindIndex)
								{
									switch (ve.Semantic)
									{
									case VES_POSITION:
										vertexBuffer[i].Position = float3((float*)(pBuffer + ve.Offset));
										vertexBuffer[i].Flags |= Vertex::ePosition;
#ifdef RIGHT2LEFT
										mFBXTransformer.TransformPosition(&vertexBuffer[i].Position, &vertexBuffer[i].Position);
#endif
										pSubMesh->Bound.Merge(vertexBuffer[i].Position);
										break;
									case VES_NORMAL:
										vertexBuffer[i].Normal = float3((float*)(pBuffer + ve.Offset));
										vertexBuffer[i].Flags |= Vertex::eNormal;
#ifdef RIGHT2LEFT
										mFBXTransformer.TransformDirection(&vertexBuffer[i].Normal, &vertexBuffer[i].Normal);
#endif
										break;
									case VES_TEXTURE_COORDINATES:
										vertexBuffer[i].Tex0 = float2((float*)(pBuffer + ve.Offset));
										vertexBuffer[i].Flags |= Vertex::eTexcoord0;
									default:
										break;
									}
								}
							}

							pBuffer += vertexSize;
						}
					}
					break;
				}

				// Get next stream
				if (!stream.IsEof())
				{
					streamID = readChunk(stream);
				}
			}
			if (!stream.IsEof())
			{
				// Backpedal back to start of non-submesh stream
				stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
			}
		}
	}

	// Find all bone assignments, submesh operation, and texture aliases (if present)
	if (!stream.IsEof())
	{
		streamID = readChunk(stream);
		while(!stream.IsEof() &&
			(streamID == M_SUBMESH_BONE_ASSIGNMENT ||
			streamID == M_SUBMESH_OPERATION ||
			streamID == M_SUBMESH_TEXTURE_ALIAS))
		{
			switch(streamID)
			{
			case M_SUBMESH_OPERATION:
				{
					OperationType type = readSubMeshOperation(stream);
					assert(type == OT_TRIANGLE_LIST);
				}
				break;
			case M_SUBMESH_BONE_ASSIGNMENT:
				readSubMeshBoneAssignment(stream, pMesh->Vertices[pSubMesh->VertexBufferIndex]);
				break;
			case M_SUBMESH_TEXTURE_ALIAS:
				readSubMeshTextureAlias(stream);
				break;
			}

			if (!stream.IsEof())
			{
				streamID = readChunk(stream);
			}

		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of stream
			stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
		}
	}

	size_t index = 0;
	for (Vertex& vertex : pMesh->Vertices[pSubMesh->VertexBufferIndex])
	{
		if (vertex.Flags & Vertex::eBlendIndices)
		{
			vertex.BlendIndices.resize(4, 0);
			vertex.BlendWeights.resize(4, 0.0f);

			float sumWeight = 0.0f;
			for (float w : vertex.BlendWeights) sumWeight += w;

			float invSumWeight = 1.0f / sumWeight;
			for (float& w : vertex.BlendWeights) w *= invSumWeight;
		}

		vertex.Index = index++;
	}

	pMesh->Bound.Merge(pSubMesh->Bound);
	pMesh->MeshParts.push_back(pSubMesh);
} 

void FbxProcesser::ReadSkeleton(FileStream& stream, shared_ptr<MeshData>& pMesh)
{
	String skelName = stream.ReadString();
	stream.Seek(stream.GetPosition() - 1);
	skelName = skelName.substr(0, skelName.size() - 1);

	String fullPath, dummy;
	PathUtil::SplitPath(stream.GetName(), fullPath, dummy, dummy);

	fullPath += skelName;

	FileStream skeletonStream;
	if ( skeletonStream.Open(fullPath) == false)
	{
		printf("skeleton %s not exits!", fullPath.c_str());
		exit(1);
	}

	// Determine endianness (must be the first thing we do!)
	determineEndianness(skeletonStream);

	// Check header
	uint16_t header = skeletonStream.ReadUShort();
	assert(header == HEADER_STREAM_ID);
	String ver = ReadString(skeletonStream);

	std::vector<OgreBone> ogreBones;
	std::unordered_map<unsigned short, unsigned short> ogreBoneChildParents;

	unsigned short streamID;
	while(!skeletonStream.IsEof())
	{
		streamID = readChunk(skeletonStream);
		switch (streamID)
		{
		case SKELETON_BONE: // Read Bone
			{
				OgreBone bone;

				// char* name
				bone.Name = ReadString(skeletonStream);
				
				// unsigned short handle            : handle of the bone, should be contiguous & start at 0
				bone.handle = skeletonStream.ReadUShort();

				// Vector3 position                 : position of this bone relative to parent 
				skeletonStream.Read(&bone.Translation, sizeof(float3));

				// Quaternion orientation           : orientation of this bone relative to parent 
				float tmp[4];
				skeletonStream.Read(&tmp, sizeof(tmp));
				bone.Rotation = Quaternionf(tmp[3], tmp[0], tmp[1], tmp[2]);

				// Do we have scale?
				if (mCurrentstreamLen > BONE_CHUNK_SIZE)
				{
					float3 scale;
					skeletonStream.Read(&bone.Scale, sizeof(float3));
				}
				else
					bone.Scale = float3(1, 1, 1);

#ifdef RIGHT2LEFT
				float4x4 transform = CreateTransformMatrix(bone.Scale, bone.Rotation, bone.Translation);
				mFBXTransformer.TransformMatrix(&transform, &transform);
				MatrixDecompose(bone.Scale, bone.Rotation, bone.Translation, transform);
#endif

				ogreBones.push_back(bone);
			}
			break;
		case SKELETON_BONE_PARENT:
			{
				// All bones have been created by this point
				unsigned short childHandle, parentHandle;

				// unsigned short handle         : child bone
				childHandle = skeletonStream.ReadUShort();
				// unsigned short parentHandle   : parent bone
				parentHandle = skeletonStream.ReadUShort();
				// Find bones
				ogreBoneChildParents[childHandle] = parentHandle;
			}
			break;
		case SKELETON_ANIMATION:
			if (!pMesh->Skeleton)
				pMesh->Skeleton = BuildOgreSkeleton(ogreBones, ogreBoneChildParents);

			ReadAnimation(skeletonStream, pMesh->Skeleton);
			break;
		case SKELETON_ANIMATION_LINK:
			{
				// char* skeletonName
				String skelName = ReadString(skeletonStream);
				// float scale
				float scale = skeletonStream.ReadFloat();

				//pSkel->addLinkedSkeletonAnimationSource(skelName, scale);
			}
			break;
		}
	}
}

void FbxProcesser::ReadAnimation(FileStream& stream, shared_ptr<Skeleton>& pSkel)
{
	// char* name                       : Name of the animation
	String clipName = ReadString(stream);

	AnimationClipData& clip = mAnimationClips[clipName];

	// float length                      : Length of the animation in seconds
	clip.Duration = stream.ReadFloat();

	// Read all tracks
	if (!stream.IsEof())
	{
		unsigned short streamID = readChunk(stream);
		while(streamID == SKELETON_ANIMATION_TRACK && !stream.IsEof())
		{
			ReadAnimationTrack(stream, clip, pSkel);

			if (!stream.IsEof())
			{
				// Get next stream
				streamID = readChunk(stream);
			}
		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of this stream if we've found a non-track
			stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
		}
	}
}

void FbxProcesser::ReadAnimationTrack(FileStream& stream, AnimationClipData& clipData, shared_ptr<Skeleton>& pSkel)
{
	// unsigned short boneIndex     : Index of bone to apply to
	unsigned short boneHandle = stream.ReadShort();

	Bone* pTargetBone = pSkel->GetBone(boneHandle);
	assert(pTargetBone != 0);

	//Bone* targetBone = pSkel->GetBone(boneHandle);
	float3 localBoneTranslation = pTargetBone->GetPosition();
	Quaternionf localBoneRotation = pTargetBone->GetRotation();

	// Create track
	clipData.mAnimationTracks.resize( clipData.mAnimationTracks.size() + 1 );
	AnimationClipData::AnimationTrack& track = clipData.mAnimationTracks.back(); 
	track.KeyFrames.reserve(50);
	track.Name = pTargetBone->GetName();

	// Keep looking for nested keyframes
	if (!stream.IsEof())
	{
		unsigned short streamID = readChunk(stream);
		while(streamID == SKELETON_ANIMATION_TRACK_KEYFRAME && !stream.IsEof())
		{
			track.KeyFrames.resize(track.KeyFrames.size() + 1);
			AnimationClipData::KeyFrame& kf = track.KeyFrames.back();

			// float time                    : The time position (seconds)
			kf.Time = stream.ReadFloat();

			// Quaternion rotate            : Rotation to apply at this keyframe
			kf.Rotation.X() = stream.ReadFloat();
			kf.Rotation.Y() = stream.ReadFloat();
			kf.Rotation.Z() = stream.ReadFloat();
			kf.Rotation.W() = stream.ReadFloat();

#ifdef RIGHT2LEFT
			float4x4 transform = QuaternionToRotationMatrix(kf.Rotation);
			mFBXTransformer.TransformMatrix(&transform, &transform);
		    kf.Rotation = QuaternionFromRotationMatrix(transform);
#endif
			kf.Rotation = kf.Rotation * localBoneRotation; // Ogre ¾ØÕó×ø³Ë

			// Vector3 translate            : Translation to apply at this keyframe
			stream.Read(&kf.Translation, sizeof(float3));
#ifdef RIGHT2LEFT
			mFBXTransformer.TransformPosition(&kf.Translation, &kf.Translation);
#endif
			kf.Translation += localBoneTranslation;

			// Do we have scale?
			if (mCurrentstreamLen > KEY_CHUNK_SIZE)
			{
				stream.Read(&kf.Scale, sizeof(float3));
			}
			else
				kf.Scale = float3(1, 1, 1);

			if (!stream.IsEof())
			{
				// Get next stream
				streamID = readChunk(stream);
			}
		}
		if (!stream.IsEof())
		{
			// Backpedal back to start of this stream if we've found a non-keyframe
			stream.Seek(stream.GetPosition() -STREAM_OVERHEAD_SIZE);
		}
	}
}
