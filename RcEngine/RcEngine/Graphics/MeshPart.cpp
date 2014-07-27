#include <Graphics/MeshPart.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <IO/Stream.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Core/Exception.h>

#define InvalidMaterialID UINT32_MAX

namespace RcEngine {

MeshPart::MeshPart(Mesh& mesh)
	: mParentMesh(mesh),
	  mIndexCount(0),
	  mPrimitiveCount(0), 
	  mIndexStart(0),
	  mVertexStart(0)
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

	mVertexStart = source.ReadUInt();
	mVertexCount = source.ReadUInt();

	mIndexStart = source.ReadUInt();

	mPrimitiveCount = indexCount / 3;
	mIndexFormat = indexFormat;
	mIndexStart = 0;
	mIndexCount = indexCount;
}


void MeshPart::Save( Stream& source )
{

}


MeshPart::~MeshPart()
{

}

void MeshPart::GetRenderOperation( RenderOperation& op, uint32_t lodIndex )
{
	op.PrimitiveType = PT_Triangle_List;
	op.BindVertexStream(0, mVertexBuffer);
	op.VertexDecl = mVertexDecl;

	if (mIndexCount > 0)
	{
		// use indices buffer
		op.BindIndexStream(mIndexBuffer, mIndexFormat);
		op.SetIndexRange(mIndexStart, mIndexCount);
		op.VertexStart = mVertexStart;
	}
	else
	{
		op.SetVertexRange(mVertexStart, mVertexCount);
	}	
}

}
