#ifndef RenderOperation_h__
#define RenderOperation_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class _ApiExport RenderOperation
{
public:
	RenderOperation();

	void BindVertexStream(uint32_t streamUnit, shared_ptr<GraphicsBuffer> vertexBuffer);
	void BindIndexStream(shared_ptr<GraphicsBuffer> indexBuffer, IndexBufferType indexType);
	
	// glDrawArrays, glDrawArraysInstanced
	void SetIndexRange(uint32_t indexStart, uint32_t indexCount, uint32_t numInstance = 0);
	
	// glDrawElements, glDrawElementsInstanced
	void SetVertexRange(uint32_t vertexStart, uint32_t vertexCount, uint32_t numInstance = 0);


public:
	PrimitiveType PrimitiveType;
	vector< shared_ptr<GraphicsBuffer> > VertexStreams;

	IndexBufferType IndexType;
	shared_ptr<GraphicsBuffer> IndexBuffer;
	
	shared_ptr<VertexDeclaration> VertexDecl; 

	// Non-index mode
	uint32_t VertexStart;
	uint32_t VertexCount;

	// Index mode 
	uint32_t IndexStart;
	uint32_t IndexCount;

	int32_t BaseVertex; // indices[i] + basevertex

	// Instance 
	uint32_t NumInstances;
};


}



#endif // RenderOperation_h__
