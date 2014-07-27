#ifndef RHVertexDeclaration_h__
#define RHVertexDeclaration_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

struct _ApiExport VertexElement
{
	/// <summary>
	// The offset in the buffer that this element starts at
	/// </summary>
	uint32_t Offset;

	/// <summary>
	// The format of this vertex element.
	/// </summary>
	VertexElementFormat Type;

	/// <summary>
	// The format of this vertex element.
	/// </summary>
	VertexElementUsage Usage;

	/// <summary>
	// Index of the item, only applicable for some elements like texture coordinate.
	/// </summary>
	uint32_t UsageIndex;

	/// <summary>
	// An integer value that identifies the input stream. 
	/// </summary>
	uint32_t InputSlot;

	/// <summary>
	// The number of instances to draw using the same per-instance data before advancing in the buffer by one element
	/// </summary>
	uint32_t InstanceStepRate;

	VertexElement() : InputSlot(0), InstanceStepRate(0) {}
	VertexElement(uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint32_t index = 0);
};

struct _ApiExport VertexElementUtil
{
	static uint32_t GetElementComponentCount(const VertexElement& element);
	static uint32_t GetElementSize(const VertexElement& element);
};

class _ApiExport VertexDeclaration 
{
public:
	VertexDeclaration(const VertexElement* element, uint32_t count);
	virtual ~VertexDeclaration() {}

	uint32_t GetStreamStride(uint32_t streamSlot);
	uint32_t GetVertexSize() const;

	const std::vector<VertexElement>& GetVertexElements() const { return mVertexElemets; }

public:
	std::vector<VertexElement> mVertexElemets;
};

}


#endif // RHVertexDeclaration_h__
