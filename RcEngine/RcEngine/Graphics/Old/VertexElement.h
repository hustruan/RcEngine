#ifndef _VertexElement__H
#define _VertexElement__H

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class _ApiExport VertexElement
{
public:
	/// <summary>
	// Constructor, should not be called directly, only needed because of list
	/// </summary>
	VertexElement() {}

	/// <summary>
	// Constructor, should not be called directly, call VertexDeclaration::AddElement
	/// </summary>
	VertexElement(uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint16_t index = 0);

	/// <summary>
	// Gets the size of this element in bytes
	/// </summary>
	uint32_t GetSize(void) const { return GetTypeSize(Type); }

	/// <summary>
	// Compare two VertexElement 
	/// </summary>
	inline bool operator == (const VertexElement& rhs) const
	{
		return Type == rhs.Type &&
			UsageIndex == rhs.UsageIndex &&
			Offset == rhs.Offset &&
			Usage == rhs.Usage;
	}

	inline bool operator != (const VertexElement& rhs) const
	{
		return !(*this == rhs);	
	}


public:

	/// <summary>
	// Gets the size of this element in bytes
	/// </summary>
	static uint32_t GetTypeSize(VertexElementFormat etype);

	/// <summary>
	// Utility method which returns the count of values in a given type
	/// </summary>
	static uint16_t GetTypeCount(VertexElementFormat etype);

	/// <summary>
	// Simple converter function which will turn a single-value type into a multi-value type based on a parameter.
	/// </summary>
	static VertexElementFormat MultiplyTypeCount(VertexElementFormat baseType, unsigned short count);
			
	/// <summary>
	// Simple converter function which will a type into it's single-value equivalent - makes switches on type easier.
	/// </summary>
	static VertexElementFormat GetBaseType(VertexElementFormat multiType);

	static bool IsNormalized(VertexElementFormat etype);

public:
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
	// Index of the item, only applicable for some elements like texture coords
	/// </summary>
	uint16_t UsageIndex;
};

} // Namespace RcEngine

#endif // _VertexElement__H