#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {


OpenGLMapping::OpenGLMapping(void)
{
}


OpenGLMapping::~OpenGLMapping(void)
{
}

GLenum OpenGLMapping::Mapping( VertexElementFormat format )
{
	switch(format)
	{
	case VEF_Float:
	case VEF_Float2:
	case VEF_Float3:
	case VEF_Float4:
		return GL_FLOAT;


	case VEF_Int:
	case VEF_Int2:
	case VEF_Int3:
	case VEF_Int4:
		return GL_INT;


	case VEF_UInt:
	case VEF_UInt2:
	case VEF_UInt3:
	case VEF_UInt4:
		return GL_UNSIGNED_INT;


	case VEF_Bool:
	case VEF_Bool2:
	case VEF_Bool3:
	case VEF_Bool4:
		return GL_BOOL;

	}
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported vertex format", "OpenGLGraphicCommon::Mapping");
}

void OpenGLMapping::UnMapping( GLenum glType, VertexElementFormat& oFormat )
{
	switch (glType)
	{
	case GL_FLOAT:					oFormat = VEF_Float; break;
	case GL_FLOAT_VEC2:				oFormat = VEF_Float2; break;
	case GL_FLOAT_VEC3:				oFormat = VEF_Float3; break;
	case GL_FLOAT_VEC4:				oFormat = VEF_Float4; break;
	case GL_INT:					oFormat = VEF_Int; break;
	case GL_INT_VEC2:				oFormat = VEF_Int2; break;
	case GL_INT_VEC3:				oFormat = VEF_Int3; break;
	case GL_INT_VEC4:				oFormat = VEF_Int4; break;
	case GL_UNSIGNED_INT:			oFormat = VEF_UInt; break;
	case GL_UNSIGNED_INT_VEC2:		oFormat = VEF_UInt2; break;
	case GL_UNSIGNED_INT_VEC3:		oFormat = VEF_UInt3; break;
	case GL_UNSIGNED_INT_VEC4:		oFormat = VEF_UInt4; break;
	default:
		break;
	}
}


GLenum OpenGLMapping::Mapping( PrimitiveType type )
{
	switch(type)
	{
	case PT_Point_List:				return GL_POINTS;
	case PT_Line_List:				return GL_LINES;
	case PT_Line_Strip:				return GL_LINE_STRIP;
	case PT_Line_List_Adj:			return GL_LINES_ADJACENCY;
	case PT_Line_Strip_Adj:			return GL_LINE_STRIP_ADJACENCY;
	case PT_Triangle_List:			return GL_TRIANGLES;
	case PT_Triangle_Strip:			return GL_TRIANGLE_STRIP;
	case PT_Triangle_List_Adj:		return GL_TRIANGLES_ADJACENCY;
	case PT_Triangle_Strip_Adj:		return GL_TRIANGLE_STRIP_ADJACENCY;
	default:						return GL_PATCHES;
	}
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported PrimitiveType", "OpenGLGraphicCommon::Mapping");
}

void OpenGLMapping::Mapping( GLenum& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat )
{
	struct PixelFormatOGL
	{
		GLenum InternalFormat;
		GLenum Format;
		GLenum Type;
	};

	static const PixelFormatOGL formatOGL[PF_Count] = {

		{ 0,							0,						0 }, //PF_Unknown = 0,

		// unorm formats
		{ GL_R8,						GL_RED,					GL_UNSIGNED_BYTE  }, //PF_R8_UNORM,
		{ GL_RG8,						GL_RG,					GL_UNSIGNED_BYTE  }, //PF_RG8_UNORM,
		{ GL_RGB8,						GL_RGB,					GL_UNSIGNED_BYTE  }, //PF_RGB8_UNORM,
		{ GL_RGB8,						GL_BGR,					GL_UNSIGNED_BYTE  }, //PF_BGR8_UNORM,
		{ GL_RGBA8,						GL_RGBA,				GL_UNSIGNED_BYTE  }, //PF_RGBA8_UNORM,
		{ GL_RGBA8,						GL_BGRA,				GL_UNSIGNED_BYTE  }, //PF_BGRA8_UNORM,
		{ GL_RGBA8,						GL_RGBA,				GL_UNSIGNED_BYTE  }, //PF_RGBX8_UNORM,
		{ GL_RGBA8,						GL_BGRA,				GL_UNSIGNED_BYTE  }, //PF_BGRX8_UNORM,

		{ GL_R16,						GL_RED,					GL_UNSIGNED_SHORT }, //PF_R16_UNORM,
		{ GL_RG16,						GL_RG,					GL_UNSIGNED_SHORT  }, //PF_RG16_UNORM,
		{ GL_RGB16,						GL_RGB,					GL_UNSIGNED_SHORT  }, //PF_RGB16_UNORM,
		{ GL_RGBA16,					GL_RGBA,				GL_UNSIGNED_SHORT  }, //PF_RGBA16_UNORM,

		// snorm formats
		{ GL_R8_SNORM,					GL_RED_SNORM,			GL_BYTE  }, //PF_RGBA8_SNORM,
		{ GL_RG8_SNORM,					GL_RED_SNORM,			GL_BYTE  }, //PF_R8_SNORM,
		{ GL_RGB8_SNORM,				GL_RED_SNORM,			GL_BYTE  }, //PF_RG8_SNORM,
		{ GL_RGBA8_SNORM,				GL_RED_SNORM,			GL_BYTE  }, //PF_RGB8_SNORM,
		
		{ GL_R16_SNORM,					GL_RED,					GL_SHORT  }, //PF_R16_SNORM,
		{ GL_RG16_SNORM,				GL_RG,					GL_SHORT  }, //PF_RG16_SNORM,
		{ GL_RGB16_SNORM,				GL_RGB,					GL_SHORT  }, //PF_RGB16_SNORM,
		{ GL_RGBA16_SNORM,				GL_RGBA,				GL_SHORT  }, //PF_RGBA16_SNORM,
	
		// Unsigned integer formats
		{ GL_R8UI,						GL_RED_INTEGER,			GL_UNSIGNED_BYTE  }, //PF_R8U,
		{ GL_RG8UI,						GL_RG_INTEGER,			GL_UNSIGNED_BYTE  }, //PF_RG8U,
		{ GL_RGB8UI,					GL_RGB_INTEGER,			GL_UNSIGNED_BYTE  }, //PF_RGB8U,
		{ GL_RGBA8UI,					GL_RGBA_INTEGER,		GL_UNSIGNED_BYTE  }, //PF_RGBA8U,

		{ GL_R16UI,						GL_RED_INTEGER,			GL_UNSIGNED_SHORT  }, //PF_R16U,
		{ GL_RG16UI,					GL_RG_INTEGER,			GL_UNSIGNED_SHORT  }, //PF_RG16U,
		{ GL_RGB16UI,					GL_RGB_INTEGER,			GL_UNSIGNED_SHORT  }, //PF_RGB16U,
		{ GL_RGBA16UI,					GL_RGBA_INTEGER,		GL_UNSIGNED_SHORT  }, //PF_RGBA16U,

		{ GL_R32UI,						GL_RED_INTEGER,			GL_UNSIGNED_INT  },  //PF_R32U,
		{ GL_RG32UI,					GL_RG_INTEGER,			GL_UNSIGNED_INT  },  //PF_RG32U,
		{ GL_RGB32UI,					GL_RGB_INTEGER,			GL_UNSIGNED_INT  },  //PF_RGB32U,
		{ GL_RGBA32UI,					GL_RGBA_INTEGER,		GL_UNSIGNED_INT  },  //PF_RGBA32U,
	
		// Signed integer formats
		{ GL_R8I,						GL_RED_INTEGER,			GL_BYTE  }, //PF_R8I,
		{ GL_RG8I,						GL_RG_INTEGER,			GL_BYTE  }, //PF_RG8I,
		{ GL_RGB8I,						GL_RGB_INTEGER,			GL_BYTE  }, //PF_RGB8I,
		{ GL_RGBA8I,					GL_RGBA_INTEGER,		GL_BYTE  }, //PF_RGBA8I,
	
		{ GL_R16I,						GL_RED_INTEGER,			GL_SHORT  }, //PF_R16I,
		{ GL_RG16I,						GL_RG_INTEGER,			GL_SHORT  }, //PF_RG16I,
		{ GL_RGB16I,					GL_RGB_INTEGER,			GL_SHORT  }, //PF_RGB16I,
		{ GL_RGBA16I,					GL_RGBA_INTEGER,		GL_SHORT  }, //PF_RGBA16I,
	
		{ GL_R32I,						GL_RED_INTEGER,			GL_INT  }, //PF_R32I,
		{ GL_RG32I,						GL_RG_INTEGER,			GL_INT  }, //PF_RG32I,
		{ GL_RGB32I,					GL_RGB_INTEGER,			GL_INT  }, //PF_RGB32I,
		{ GL_RGBA32I,					GL_RGBA_INTEGER,		GL_INT  }, //PF_RGBA32I,
	
		// Floating formats
		{ GL_R16F,						GL_RED,					GL_HALF_FLOAT  }, //PF_R16F,
		{ GL_RG16F,						GL_RG,					GL_HALF_FLOAT  }, //PF_RG16F,
		{ GL_RGB16F,					GL_RGB,					GL_HALF_FLOAT  }, //PF_RGB16F,
		{ GL_RGBA16F,					GL_RGBA,				GL_HALF_FLOAT  }, //PF_RGBA16F,
	
		{ GL_R32F,						GL_RED,					GL_FLOAT  }, //PF_R32F,
		{ GL_RG32F,						GL_RG,					GL_FLOAT  }, //PF_RG32F,
		{ GL_RGB32F,					GL_RGB,					GL_FLOAT  }, //PF_RGB32F,
		{ GL_RGBA32F,					GL_RGBA,				GL_FLOAT  }, //PF_RGBA32F,

		// Packed formats
		{ GL_RGB9_E5,					GL_RGB,					GL_UNSIGNED_INT_5_9_9_9_REV  }, //PF_RGB9E5,
		{ GL_R11F_G11F_B10F,			GL_RGB,					GL_UNSIGNED_INT_10F_11F_11F_REV  }, //PF_RG11B10F,
		{ GL_R3_G3_B2,					GL_RGB,					GL_UNSIGNED_BYTE_3_3_2  }, //PF_R3G3B2,
		{ GL_RGB5,						GL_BGR,					GL_UNSIGNED_SHORT_5_6_5  }, //PF_B5G6R5,
		{ GL_RGB5_A1,					GL_BGRA,				GL_UNSIGNED_BYTE_3_3_2  }, //PF_BGR5A1,
		{ GL_RGBA4,						GL_RGBA,				GL_UNSIGNED_SHORT_4_4_4_4  }, //PF_RGBA4,
		{ GL_RGB10_A2,					GL_RGBA,				GL_UNSIGNED_INT_10_10_10_2  }, //PF_RGB10A2,
		
	
		// Depth formats
		{ GL_DEPTH_COMPONENT16,						GL_DEPTH_COMPONENT,					GL_UNSIGNED_SHORT  }, //PF_D16,
		{ GL_DEPTH24_STENCIL8,						GL_DEPTH_STENCIL,					GL_UNSIGNED_INT_24_8_EXT  }, //PF_D24S8,
		{ GL_DEPTH_COMPONENT32,						GL_DEPTH_COMPONENT,					GL_FLOAT  }, //PF_D32F,
		{ GL_DEPTH32F_STENCIL8,						GL_DEPTH_STENCIL,					GL_FLOAT  }, //PF_D32FS8X24,

		// Compressed formats
		
		// DDS has no flag about DXT1 alpha channel, Use GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
		{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,					0 }, //PF_RGB_DXT1, 
		{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,					0 }, //PF_RGBA_DXT1,
		{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,					0 }, //PF_RGBA_DXT3,
		{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,					0 }, //PF_RGBA_DXT5,
		{ GL_COMPRESSED_RED_RGTC1,					GL_COMPRESSED_RED_RGTC1,							0 }, //PF_R_ATI1N_UNORM,
		{ GL_COMPRESSED_SIGNED_RED_RGTC1,			GL_COMPRESSED_SIGNED_RED_RGTC1,						0 }, //PF_R_ATI1N_SNORM,
		{ GL_COMPRESSED_RG_RGTC2,					GL_COMPRESSED_RG_RGTC2,								0 }, //PF_RG_ATI2N_UNORM,
		{ GL_COMPRESSED_SIGNED_RG_RGTC2,			GL_COMPRESSED_SIGNED_RG_RGTC2,						0 }, //PF_RG_ATI2N_SNORM,
		{ GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,	GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,				0 }, //PF_RGB_BP_UNSIGNED_FLOAT,
		{ GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,		GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,				0 }, //PF_RGB_BP_SIGNED_FLOAT,
		{ GL_COMPRESSED_RGBA_BPTC_UNORM,			GL_COMPRESSED_RGBA_BPTC_UNORM,						0 }, //PF_RGB_BP_UNORM,
		
		// Undefined
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGB_PVRTC_4BPPV1,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGB_PVRTC_2BPPV1,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_PVRTC_4BPPV1,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_PVRTC_2BPPV1,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_ATC_RGB,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_ATC_RGBA_EXPLICIT_ALPHA,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_ATC_RGBA_INTERPOLATED_ALPHA,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_ASTC_4x4,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_ASTC_5x4,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_ASTC_5x5,
		{ GL_INVALID_ENUM,							GL_RED,												0  }, //PF_RGBA_ASTC_6x5,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_6x6,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_8x5,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_8x6,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_8x8,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_10x5,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_10x6,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_10x8,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_10x10,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_12x10,
		{ GL_INVALID_ENUM,							GL_RED,												0 }, //PF_RGBA_ASTC_12x12,
					
		// sRGB formats
		{ GL_SRGB8,									GL_RGB,					GL_UNSIGNED_BYTE  }, //PF_SRGB8,
		{ GL_SRGB8,									GL_BGR,					GL_UNSIGNED_BYTE  }, //PF_SBGR8_UNORM,
		{ GL_SRGB8_ALPHA8,							GL_RGBA,				GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_UNORM,
		{ GL_SRGB8_ALPHA8,							GL_BGRA,				GL_UNSIGNED_BYTE  }, //PF_SBGR8_ALPHA8_UNORM,
		{ GL_SRGB8_ALPHA8,							GL_RGBA,				GL_UNSIGNED_BYTE  }, //PF_SRGBX8_UNORM,
		{ GL_SRGB8_ALPHA8,							GL_BGRA,				GL_UNSIGNED_BYTE  }, //PF_SBGRX8_UNORM,
		
		// DDS has no flag about DXT1 alpha channel, Use GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
		// { GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,			GL_RGB,					0  }, //PF_SRGB_DXT1, 
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,	GL_RGBA,					0  }, //PF_SRGB_DXT1, 
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,	GL_RGBA,				0  }, //PF_SRGB_ALPHA_DXT1,
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,	GL_RGBA,				0  }, //PF_SRGB_ALPHA_DXT3,
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,	GL_RGBA,				0  }, //PF_SRGB_ALPHA_DXT5,
		{ GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,		GL_RGBA,				0  }, //PF_SRGB_BP_UNORM,
		
		// Undefined
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB_PVRTC_2BPPV1,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB_PVRTC_4BPPV1,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB_ALPHA_PVRTC_2BPPV1,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB_ALPHA_PVRTC_4BPPV1,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_4x4,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_5x4,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_5x5,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_6x5,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_6x6,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_8x5,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_8x6,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_8x8,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_10x5,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_10x6,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_10x8,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_10x10,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_12x10,
		{ GL_INVALID_ENUM,									GL_RED,					GL_UNSIGNED_BYTE  }, //PF_SRGB8_ALPHA8_ASTC_12x12,
	};

	static_assert( ARRAY_SIZE(formatOGL) == PF_Count, "OpenGL Internal Format not match with PixelFormat" );
	
	outInternalformat = formatOGL[inPixelFormat].InternalFormat;
	outFormat = formatOGL[inPixelFormat].Format;
	outType = formatOGL[inPixelFormat].Type;
}

GLenum OpenGLMapping::Mapping( BlendOperation op )
{
	switch(op)
	{
	case BOP_Add:
		return GL_FUNC_ADD;
	case BOP_Sub:
		return GL_FUNC_SUBTRACT;
	case BOP_Rev_Sub:
		return GL_FUNC_REVERSE_SUBTRACT;
	case BOP_Min:
		return GL_MIN;
	case BOP_Max:
		return GL_MAX;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported BlendOperation!", 
			"OpenGLMapping::Mapping( BlendOperation op )");
	}
}

GLenum OpenGLMapping::Mapping( AlphaBlendFactor factor )
{
	switch(factor)
	{
	case ABF_Zero:
		return GL_ZERO;

	case ABF_One:
		return GL_ONE;

	case ABF_Src_Alpha:
		return GL_SRC_ALPHA;

	case ABF_Dst_Alpha:
		return GL_DST_ALPHA;

	case ABF_Inv_Src_Alpha:
		return GL_ONE_MINUS_SRC_ALPHA;

	case ABF_Inv_Dst_Alpha:
		return GL_ONE_MINUS_DST_ALPHA;

	case ABF_Src_Color:
		return GL_SRC_COLOR;

	case ABF_Dst_Color:
		return GL_DST_COLOR;

	case ABF_Inv_Src_Color:
		return GL_ONE_MINUS_SRC_COLOR;

	case ABF_Inv_Dst_Color:
		return GL_ONE_MINUS_DST_COLOR;

	case ABF_Src_Alpha_Sat:
		return GL_SRC_ALPHA_SATURATE;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported AlphaBlendFactor!", 
			"OpenGLMapping::Mapping( AlphaBlendFactor factor )");
	}
}

GLenum OpenGLMapping::Mapping( FillMode fm )
{
	switch(fm)
	{
	case FM_Solid:
		return GL_FILL;
	case FM_WireFrame:
		return GL_LINE;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported FillMode!", 
			"OpenGLMapping::Mapping( FillMode fm )");
	}
}

GLenum OpenGLMapping::Mapping( CompareFunction cf )
{
	switch(cf)
	{
	case CF_Always:
		return GL_ALWAYS;
	case CF_Never:
		return GL_NEVER;
	case CF_Less:
		return GL_LESS;
	case CF_LessEqual:
		return GL_LEQUAL;
	case CF_Equal:
		return GL_EQUAL;
	case CF_NotEqual:
		return GL_NOTEQUAL;
	case CF_GreaterEqual:
		return GL_GEQUAL;
	case CF_Greater:
		return GL_GREATER;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported CompareFunction!", 
			"OpenGLMapping::Mapping( CompareFunction cf )");
	}
}

GLenum OpenGLMapping::Mapping( StencilOperation sop )
{				
	switch(sop)
	{
	case SOP_Keep:		return GL_KEEP;
	case SOP_Zero:		return GL_ZERO;
	case SOP_Replace:	return GL_REPLACE;
	case SOP_Incr:		return GL_INCR;
	case SOP_Decr:		return GL_DECR;
	case SOP_Invert:	return GL_INVERT;
	case SOP_Incr_Wrap: return GL_INCR_WRAP;
	case SOP_Decr_Wrap: return GL_DECR_WRAP;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported StencilOperation!", 
			"OpenGLMapping::Mapping( StencilOperation )");
	}
}

GLenum OpenGLMapping::Mapping( TextureAddressMode mode )
{
	switch (mode)
	{
	case TAM_Wrap:
		return GL_REPEAT;
	case TAM_Border:
		return GL_CLAMP_TO_BORDER;
	case TAM_Clamp:
		return GL_CLAMP_TO_EDGE;
	case TAM_Mirror:
		return GL_MIRRORED_REPEAT;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported TextureAddressMode!", 
			"OpenGLMapping::Mapping( TextureAddressMode )");
	}
}

void OpenGLMapping::Mapping( GLenum& min, GLenum& mag, TextureFilter filter )
{
	switch(filter)
	{
	case TF_Min_Mag_Mip_Point:
		{
			min = GL_NEAREST;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Mag_Point_Mip_Linear:
		{
			min = GL_NEAREST_MIPMAP_LINEAR;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Point_Mag_Linear_Mip_Point:
		{
			min = GL_NEAREST_MIPMAP_NEAREST;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Point_Mag_Mip_Linear:
		{
			min = GL_NEAREST_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Linear_Mag_Mip_Point:
		{
			min = GL_LINEAR_MIPMAP_NEAREST;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Linear_Mag_Point_Mip_Linear:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Mag_Linear_Mip_Point:
		{
			min = GL_LINEAR_MIPMAP_NEAREST;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Mag_Mip_Linear:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	case TF_Anisotropic:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Texture Filter", 
			"OpenGLMapping::Mapping");
	}
}

GLenum OpenGLMapping::Mapping( ShaderType type )
{
	switch (type)
	{
	case ST_Vertex:
		return GL_VERTEX_SHADER;
	case ST_TessControl:
		return GL_TESS_CONTROL_SHADER;
	case ST_TessEval:
		return GL_TESS_EVALUATION_SHADER;
	case ST_Pixel:
		return GL_FRAGMENT_SHADER;
	case ST_Geomerty:
		return GL_GEOMETRY_SHADER;
	case ST_Compute:
		return GL_COMPUTE_SHADER;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid ShaderType", "OpenGLMapping::Mapping");
	}
}

GLenum OpenGLMapping::Mapping( uint32_t accessHint )
{
	GLenum usage = GL_STATIC_DRAW;

	if (accessHint & EAH_GPU_Write)
	{
		usage = GL_DYNAMIC_COPY;
	}
	else if (accessHint & EAH_CPU_Write)
	{
		usage = GL_DYNAMIC_DRAW;
	}
	else if (accessHint == EAH_GPU_Read)
	{
		// Only sample by GPU
		usage = GL_STATIC_DRAW;
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid access hint", "D3D11Mapping::MapUsage");
	}

	return usage;
}

GLbitfield OpenGLMapping::Mapping( ResourceMapAccess mapType )
{
	switch(mapType)
	{
	case RMA_Read_Only:			 return GL_MAP_READ_BIT;
	case RMA_Write_Only:		 return GL_MAP_WRITE_BIT;
	case RMA_Read_Write:		 return GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
	case RMA_Write_Discard:		 return GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
	case RMA_Write_No_Overwrite: return GL_MAP_WRITE_BIT;
	default:
		return GL_MAP_WRITE_BIT;
		break;
	}
}

void OpenGLMapping::UnMapping( GLenum glType, EffectParameterType& paramType, ShaderParameterClass& paramClass )
{
	switch (glType)
	{
	case GL_FLOAT:			                   { paramClass = Shader_Param_Uniform; paramType = EPT_Float; } break;
	case GL_FLOAT_VEC2:		                   { paramClass = Shader_Param_Uniform; paramType = EPT_Float2; } break;
	case GL_FLOAT_VEC3:	                       { paramClass = Shader_Param_Uniform; paramType = EPT_Float3; } break;
	case GL_FLOAT_VEC4:		                   { paramClass = Shader_Param_Uniform; paramType = EPT_Float4; } break;
	case GL_UNSIGNED_INT:	                   { paramClass = Shader_Param_Uniform; paramType = EPT_UInt; } break;
	case GL_UNSIGNED_INT_VEC2:				   { paramClass = Shader_Param_Uniform; paramType = EPT_UInt2; } break;
	case GL_UNSIGNED_INT_VEC3:				   { paramClass = Shader_Param_Uniform; paramType = EPT_UInt3; } break;
	case GL_UNSIGNED_INT_VEC4:				   { paramClass = Shader_Param_Uniform; paramType = EPT_UInt4; } break;
	case GL_INT:							   { paramClass = Shader_Param_Uniform; paramType = EPT_Int; } break;
	case GL_INT_VEC2:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Int2; }	break;   
	case GL_INT_VEC3:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Int3; } break;
	case GL_INT_VEC4:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Int4; } break;
	case GL_BOOL:							   { paramClass = Shader_Param_Uniform; paramType = EPT_Boolean; } break;
	case GL_FLOAT_MAT2:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Matrix2x2; } break;
	case GL_FLOAT_MAT3:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Matrix3x3; } break;
	case GL_FLOAT_MAT4:						   { paramClass = Shader_Param_Uniform; paramType = EPT_Matrix4x4; } break;

	case GL_SAMPLER_1D:						   
	case GL_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_SAMPLER_1D_SHADOW:			       { paramClass = Shader_Param_SRV; paramType = EPT_Texture1D; } break;

	case GL_SAMPLER_2D:	
	case GL_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_SHADOW:				   { paramClass = Shader_Param_SRV; paramType = EPT_Texture2D; } break;

	case GL_SAMPLER_3D:	
	case GL_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_3D:           { paramClass = Shader_Param_SRV; paramType = EPT_Texture3D; } break;

	case GL_SAMPLER_CUBE:					  
	case GL_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_SAMPLER_CUBE_SHADOW:			   { paramClass = Shader_Param_SRV; paramType = EPT_TextureCube; } break;

	case GL_SAMPLER_1D_ARRAY:				 
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:		   { paramClass = Shader_Param_SRV; paramType = EPT_Texture1DArray; } break;

	case GL_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_2D_ARRAY_SHADOW:		   { paramClass = Shader_Param_SRV; paramType = EPT_Texture2DArray; } break;

	case GL_SAMPLER_BUFFER:					   
	case GL_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:	   { paramClass = Shader_Param_SRV; paramType = EPT_TextureBuffer; } break;

	case GL_IMAGE_1D:
	case GL_INT_IMAGE_1D:
	case GL_UNSIGNED_INT_IMAGE_1D:             { paramClass = Shader_Param_UAV; paramType = EPT_Texture1D; } break;

	case GL_IMAGE_1D_ARRAY:	
	case GL_INT_IMAGE_1D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:	   { paramClass = Shader_Param_UAV; paramType = EPT_Texture1DArray; } break;

	case GL_IMAGE_2D:
	case GL_INT_IMAGE_2D:
	case GL_UNSIGNED_INT_IMAGE_2D:             { paramClass = Shader_Param_UAV; paramType = EPT_Texture2D; } break;

	case GL_IMAGE_2D_ARRAY:		
	case GL_INT_IMAGE_2D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:       { paramClass = Shader_Param_UAV; paramType = EPT_Texture2DArray; } break;

	case GL_IMAGE_3D:		
	case GL_INT_IMAGE_3D:	
	case GL_UNSIGNED_INT_IMAGE_3D:             { paramClass = Shader_Param_UAV; paramType = EPT_Texture3D; } break;
	
	case GL_IMAGE_BUFFER:
	case GL_INT_IMAGE_BUFFER:		
	case GL_UNSIGNED_INT_IMAGE_BUFFER:		   { paramClass = Shader_Param_UAV; paramType = EPT_TextureBuffer; } break;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Shader Parameter Type", 
			"OpenGLMapping::UnMapping");
	}
}


bool OpenGLMapping::IsIntegerType( GLenum type )
{
	switch(type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_INT:
	case GL_UNSIGNED_INT:
		return true;

	default:
		return false;
	}

	return false;
}

}
