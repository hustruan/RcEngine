#include <Graphics/RenderOperation.h>

namespace RcEngine {

RenderOperation::RenderOperation()
	: PrimitiveType(PT_Triangle_List),
	  NumInstances(0),
	  VertexStart(0),
	  VertexCount(0),
	  IndexStart(0),
	  IndexCount(0),
	  BaseVertex(0)
{

}

void RenderOperation::BindVertexStream( uint32_t streamUnit, shared_ptr<GraphicsBuffer> vertexBuffer )
{
	if (VertexStreams.size() < streamUnit + 1)
		VertexStreams.resize(streamUnit+1);

	VertexStreams[streamUnit] = vertexBuffer;
}

void RenderOperation::BindIndexStream( shared_ptr<GraphicsBuffer> indexBuffer, IndexBufferType indexType )
{
	IndexBuffer = indexBuffer;
	IndexType = indexType;
}

void RenderOperation::SetIndexRange( uint32_t indexStart, uint32_t indexCount, uint32_t numInstance /*= 0*/ )
{
	IndexStart = indexStart;
	IndexCount = indexCount;
	numInstance = numInstance;
}

void RenderOperation::SetVertexRange( uint32_t vertexStart, uint32_t vertexCount, uint32_t numInstance /*= 0*/ )
{
	VertexStart = vertexStart;
	VertexCount = vertexCount;
	numInstance = numInstance;
}

}

