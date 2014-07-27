#include <Graphics/VertexDeclaration.h>
#include <Core/Exception.h>

namespace RcEngine {

VertexElement::VertexElement( uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint32_t index /*= 0*/ )
	: Offset(offset),
      Type(theType),
      Usage(semantic),
      UsageIndex(index),
      InputSlot(0),
      InstanceStepRate(0)
{

}

//////////////////////////////////////////////////////////////////////////
VertexDeclaration::VertexDeclaration( const VertexElement* element, uint32_t count )
{
	mVertexElemets.assign(element, element + count);
}

uint32_t VertexDeclaration::GetStreamStride( uint32_t streamSlot )
{
	uint32_t stride = 0;
	for (const VertexElement& element : mVertexElemets)
	{
		if (element.InputSlot == streamSlot)
			stride += VertexElementUtil::GetElementSize(element);
	}

	return stride;
}

uint32_t VertexDeclaration::GetVertexSize() const
{
	uint32_t size = 0;
	for (const VertexElement& element : mVertexElemets)
		size += VertexElementUtil::GetElementSize(element);

	return size;
}


uint32_t VertexElementUtil::GetElementComponentCount( const VertexElement& element )
{
	switch(element.Type)
	{
	case VEF_Float:
	case VEF_Int:
	case VEF_UInt:
	case VEF_Bool:
		return 1;

	case VEF_Float2:
	case VEF_Int2:
	case VEF_UInt2:
	case VEF_Bool2:
		return 2;

	case VEF_UInt3:
	case VEF_Float3:
	case VEF_Int3:
	case VEF_Bool3:
		return 3;

	case VEF_Float4:
	case VEF_Int4:
	case VEF_Bool4:
	case VEF_UInt4:
		return 4;
	}
	ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid type",  "VertexElement::GetTypeCount");
}

uint32_t VertexElementUtil::GetElementSize( const VertexElement& element )
{
	switch(element.Type)
	{
	case VEF_Float:		return sizeof(float);
	case VEF_Float2:    return sizeof(float)*2;
	case VEF_Float3:    return sizeof(float)*3;
	case VEF_Float4:    return sizeof(float)*4;
	case VEF_Int:       return sizeof(int32_t);
	case VEF_Int2:      return sizeof(int32_t)*2;
	case VEF_Int3:      return sizeof(int32_t)*3;
	case VEF_Int4:		return sizeof(int32_t)*4;
	case VEF_UInt:		return sizeof(uint32_t);
	case VEF_UInt2:		return sizeof(uint32_t)*2;
	case VEF_UInt3:		return sizeof(uint32_t)*3;
	case VEF_UInt4:		return sizeof(uint32_t)*4;
	case VEF_Bool:		return sizeof(bool);
	case VEF_Bool2:		return sizeof(bool)*2;
	case VEF_Bool3:		return sizeof(bool)*3;
	case VEF_Bool4:		return sizeof(bool)*4;
	default:			break;
	}

	ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid type", "VertexElement::GetElementSize");
}

}