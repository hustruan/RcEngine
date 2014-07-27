#pragma once
#ifndef RenderOperation_h__
#define RenderOperation_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {


class _ApiExport RenderOperation
{
public:
	enum StreamType
	{
		ST_Geometry,
		ST_Instance
	};

	struct StreamUnit
	{
		shared_ptr<GraphicsBuffer> Stream;
		shared_ptr<VertexDeclaration> VertexDecl;
		uint32_t Frequency;
		StreamType Type;
	};

public:
	RenderOperation(void);
	~RenderOperation();

	uint32_t GetStreamCount() const							{ return VertexStreams.size(); }
	const StreamUnit& GetStreamUnit(uint32_t index) const	{ return VertexStreams[index]; }
	
	void BindVertexStream(const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd,
		StreamType type = ST_Geometry, uint32_t freq = 1);

	/**
	 * Set indices buffer and calculate indices count.
	 */
	void BindIndexStream(const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type);

	/**
	 * Set index range in IndexBuffer.
	 */
	void SetIndexRange(uint32_t indexStart, uint32_t indexCount);
	
	/**
	 * No index buffer, set vertex range. Set VertexStart directly if need in VertexBuffer
	 */
	void SetVertexRange(uint32_t vertexStart, uint32_t vertexCount);

public:
	PrimitiveType PrimitiveType;

	std::vector<StreamUnit> VertexStreams;

	bool UseIndex;
	shared_ptr<GraphicsBuffer> IndexBuffer;
	IndexBufferType IndexType;

	uint32_t VertexStart;
	uint32_t VertexCount;

	uint32_t IndexStart;
	uint32_t IndexCount;
};


} // Namespace RcEngine


#endif // RenderOperation_h__