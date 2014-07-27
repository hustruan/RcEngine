#include <Graphics/VertexElement.h>
#include <Core/Exception.h>

namespace RcEngine {

VertexElement::VertexElement(uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint16_t index /*= 0*/ )
	:   Offset(offset),
		Type(theType),
		Usage(semantic),
		UsageIndex(index)
{
	
}

uint32_t VertexElement::GetTypeSize( VertexElementFormat etype )
{
	switch(etype)
	{
	case VEF_Float:
		return sizeof(float);

	case VEF_Float2:
		return sizeof(float)*2;

	case VEF_Float3:
		return sizeof(float)*3;

	case VEF_Float4:
		return sizeof(float)*4;

	case VEF_Int:
		return sizeof(int32_t);

	case VEF_Int2:
		return sizeof(int32_t)*2;

	case VEF_Int3:
		return sizeof(int32_t)*3;

	case VEF_Int4:
		return sizeof(int32_t)*4;

	case VEF_UInt:
		return sizeof(uint32_t);

	case VEF_UInt2:
		return sizeof(uint32_t)*2;

	case VEF_UInt3:
		return sizeof(uint32_t)*3;

	case VEF_UInt4:
		return sizeof(uint32_t)*4;

	case VEF_Bool:
		return sizeof(bool);

	case VEF_Bool2:
		return sizeof(bool)*2;

	case VEF_Bool3:
		return sizeof(bool)*3;

	case VEF_Bool4:
		return sizeof(bool)*4;
	}

	ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid type", "VertexElement::GetTypeSize");
}

uint16_t VertexElement::GetTypeCount( VertexElementFormat etype )
{
	switch(etype)
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
	ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid type", 
		"VertexElement::GetTypeCount");
}

bool VertexElement::IsNormalized( VertexElementFormat etype )
{
	return false;
}


} // Namespace RcEngine