#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Animation.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/Skeleton.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <Core/Loger.h>
#include <Graphics/Animation.h>
#include <IO/Stream.h>
#include <IO/FileSystem.h>
#include <IO/PathUtil.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

#define InvalidMaterialID UINT32_MAX

namespace RcEngine {

Mesh::Mesh(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Mesh, creator, handle, name, group)
{
	printf("Create Mesh: %s\n", mResourceName.c_str());
}

Mesh::~Mesh()
{
	printf("Delete Mesh: %s\n", mResourceName.c_str());
}

/**
 * Mesh Layout:
   
   Magic Number			uint32_t
   Mesh Name			String
   Mesh Bound			BoundingBox
   Mesh Parts Count		uint32_t
   Bone Count			uint32_t
   Vertex Buffer Count  uint32_t
   Index Buffer Count   uint32_t
   Mesh Part Info
   Bones 
   Vertex Buffer Data
   Index Buffer Data
*/

void Mesh::LoadImpl()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	String currMeshDirectory = PathUtil::GetParentPath(mResourceName);

	shared_ptr<Stream> streamPtr = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *streamPtr;

	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	uint32_t header = source.ReadUInt();
	assert(header == MeshId);

	// read mesh name
	String meshName = source.ReadString();

	// read bounding box
	float3 min, max;
	source.Read(&min, sizeof(float3));
	source.Read(&max, sizeof(float3));
	mBoundingBox = BoundingBoxf(min, max);

	uint32_t numMeshParts = source.ReadUInt();
	uint32_t numBones = source.ReadUInt();
	uint32_t numVertexBuffers = source.ReadUInt();
	uint32_t numIndexBuffers = source.ReadUInt();

	// Read mesh parts
	for (uint32_t i = 0; i < numMeshParts; ++i)
	{
		shared_ptr<MeshPart> subMesh = std::make_shared<MeshPart>(*this);
		subMesh->Load(source);

		String matPath;

		if (currMeshDirectory.empty())
			matPath = subMesh->mMaterialName;
		else 
			matPath = currMeshDirectory + "/" + subMesh->mMaterialName;

		// Hack: if material doesn't exit, not add it
		if (fileSystem.Exits(matPath, mGroup) == false)
		{
			EngineLogger::LogWarning("Material %s Not Exits!", matPath.c_str());
			continue;
		}

		// add mesh part material resource
		ResourceManager::GetSingleton().AddResource(RT_Material, matPath, mGroup);
		mMeshParts.push_back(subMesh);
	}

	// Read bones
	if (numBones > 0)
	{		
		mSkeleton = Skeleton::LoadFrom(source, numBones);
	}
	
	// Read vertex buffers
	mVertexBuffers.resize(numVertexBuffers);
	for (uint32_t i = 0; i < numVertexBuffers; ++i)
	{
		uint32_t vertexCount = source.ReadUInt();

		// Read vertex declaration
		uint32_t veCount = source.ReadUInt();
		vector<VertexElement> elements(veCount);
		for (VertexElement& vertexElement : elements)
		{
			vertexElement.Offset = source.ReadUInt();
			vertexElement.Type =  static_cast<VertexElementFormat>(source.ReadUInt());
			vertexElement.Usage =  static_cast<VertexElementUsage>(source.ReadUInt());
			vertexElement.UsageIndex = source.ReadUShort();
		}

		mVertexBuffers[i].VertexDecl = factory->CreateVertexDeclaration(&elements[0], elements.size());
		
		// Read vertex buffer
		uint32_t vertexSize = mVertexBuffers[i].VertexDecl->GetVertexSize();
		uint32_t vertexBufferSize = vertexSize * vertexCount;

		mVertexBuffers[i].Buffer = factory->CreateVertexBuffer(vertexBufferSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, nullptr);

		void* pBuffer = mVertexBuffers[i].Buffer->Map(0, vertexBufferSize, RMA_Write_Discard);
		source.Read(pBuffer, vertexBufferSize);
		mVertexBuffers[i].Buffer->UnMap();
	}

	// Read index buffers
	mIndexBuffers.resize(numIndexBuffers);
	for (uint32_t i = 0; i < numIndexBuffers; ++i)
	{
		uint32_t indexCount = source.ReadUInt();
		
		uint32_t indexBufferSize;
		if (source.ReadUInt() == IBT_Bit16)
		{
			mIndexBuffers[i].IndexFormat = IBT_Bit16;
			indexBufferSize = sizeof(uint16_t) * indexCount;
		}
		else
		{
			mIndexBuffers[i].IndexFormat = IBT_Bit32;
			indexBufferSize = sizeof(uint32_t) * indexCount;
		}

		// Read index buffer
		mIndexBuffers[i].Buffer = factory->CreateIndexBuffer(indexBufferSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Index, nullptr);
		void* pBuffer = mIndexBuffers[i].Buffer->Map(0, indexBufferSize, RMA_Write_Discard);
		source.Read(pBuffer, indexBufferSize);
		mIndexBuffers[i].Buffer->UnMap();
	}
}

void Mesh::UnloadImpl()
{

}

shared_ptr<Resource> Mesh::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	assert(creator != nullptr);
	return std::make_shared<Mesh>(creator, handle, name, group);
}

shared_ptr<Resource> Mesh::Clone()
{
	shared_ptr<Mesh> retVal(new Mesh(mCreator, mResourceHandle, mResourceName, mGroup));

	retVal->mBoundingBox = mBoundingBox;
	retVal->mPrimitiveCount = mPrimitiveCount;
	retVal->mBoundingBox = mBoundingBox;
	retVal->mMeshParts = mMeshParts;
	mSkeleton = mSkeleton->Clone();

	return retVal;
}

//////////////////////////////////////////////////////////////////////////

MeshPart::MeshPart(Mesh& mesh)
	: mParentMesh(mesh),
	  mIndexCount(0),
	  mPrimitiveCount(0),
	  mIndexStart(0), 
	  mVertexStart(0)
{

}

MeshPart::~MeshPart()
{

}

void MeshPart::Load(  Stream& source )
{
	// read name
	mName = source.ReadString();

	// read material name
	mMaterialName = source.ReadString(); 

	// read bounding box
	float3 min, max;
	source.Read(&min, sizeof(float3));
	source.Read(&max, sizeof(float3));
	mBoundingBox = BoundingBoxf(min, max);

	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	mVertexBufferIndex = source.ReadInt();
	mIndexBufferIndex = source.ReadInt();

	mIndexStart = source.ReadUInt();
	mIndexCount = source.ReadUInt();
	mBaseVertex = source.ReadInt();
	
	mPrimitiveCount = mIndexCount / 3;
}

void MeshPart::Save( Stream& source )
{

}

void MeshPart::GetRenderOperation( RenderOperation& op, uint32_t lodIndex )
{
	const Mesh::VertexBuffer& vertexBuffer = mParentMesh.mVertexBuffers[mVertexBufferIndex];

	op.PrimitiveType = PT_Triangle_List;
	op.BindVertexStream(0, vertexBuffer.Buffer);
	op.VertexDecl = vertexBuffer.VertexDecl;

	if (mIndexCount > 0)
	{
		const Mesh::IndexBuffer& indexBuffer = mParentMesh.mIndexBuffers[mIndexBufferIndex];

		// use indices buffer
		op.BindIndexStream(indexBuffer.Buffer, indexBuffer.IndexFormat);
		op.SetIndexRange(mIndexStart, mIndexCount);
		op.VertexStart = mVertexStart;
		op.BaseVertex = mBaseVertex;
	}
	else
	{
		assert(mIndexBufferIndex == -1);
		op.SetVertexRange(mVertexStart, mVertexCount);
	}	
}


} // Namespace RcEngine