#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/VertexDeclaration.h>

namespace RcEngine {


RenderOperation::RenderOperation( void )
	: PrimitiveType(PT_Triangle_List),
	  UseIndex(false), 
	  VertexStart(0), 
	  IndexStart(0), 
	  IndexCount(0)
{

}

RenderOperation::~RenderOperation()
{
}

void RenderOperation::BindVertexStream( const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd, StreamType type /*= ST_Geometry*/, uint32_t freq /*= 1*/ )
{
	if(type == ST_Geometry)
	{
		for(size_t i = 0; i < VertexStreams.size(); i++)
		{
			if( (*VertexStreams[i].VertexDecl) == *vd )
			{
				VertexStreams[i].Stream = buffer;
				VertexStreams[i].Type = type;
				VertexStreams[i].Frequency = freq;
				return;
			}
		}
	}
	StreamUnit su;
	su.Stream = buffer;
	su.VertexDecl = vd;
	su.Frequency = freq;
	su.Type = type;
	VertexStreams.push_back(su);
}

void RenderOperation::BindIndexStream( const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type )
{
	static uint32_t sIndexSize[2] = { 2, 4 };

	IndexType = type;
	IndexBuffer = buffer;
	UseIndex = true;

	IndexCount = IndexBuffer->GetBufferSize() / sIndexSize[IndexType];
}

void RenderOperation::SetIndexRange( uint32_t indexStart, uint32_t indexCount )
{
	IndexStart = indexStart; 
	indexCount = IndexCount;
}

void RenderOperation::SetVertexRange( uint32_t vertexStart, uint32_t vertexCount )
{
	VertexStart = vertexStart; 
	VertexCount = vertexCount;
	UseIndex = false;
}




} // Namespace RcEngine