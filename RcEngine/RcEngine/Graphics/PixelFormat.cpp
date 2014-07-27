#include <Graphics/PixelFormat.h>
#include <Core/Exception.h>

namespace RcEngine {

/**
* Flags defining some on/off properties of pixel formats
*/
enum PixelFormatFlags {
	// This format has an alpha channel
	PFF_HasAlpha        = 1UL << 1,      
	// This format is compressed. This invalidates the values in elemBytes,
	// elemBits and the bit counts as these might not be fixed in a compressed format.
	PFF_Compressed      = 1UL << 2,     
	// This is a depth format (for depth textures)
	PFF_Depth           = 1UL << 3,
	// Format is in native endian. Generally true for the 16, 24 and 32 bits
	// formats which can be represented as machine integers.
	PFF_NativeEndian    = 1UL << 4,
	// This is an intensity format instead of a RGB one. The luminance
	// replaces R,G and B. (but not A)
	PFF_Luminance       = 1UL << 5,

	PFF_sRGB            = 1UL << 6,
};

struct PixelFormatDescription
{
	uint32_t Bytes;
	uint32_t Component;
	uint32_t Flags;
};

static const PixelFormatDescription& GetPixelFormatDescription(PixelFormat format)
{

	static const PixelFormatDescription PixelFormatDesc[] = {
		
		{ 0, 0, 0 },

		// unsigned normalized formats
		{ 1, 1, 0 },					// PF_R8_UNORM
		{ 2, 2, 0 },					// PF_RG8_UNORM
		{ 3, 3, 0 },					// PF_RGB8_UNORM
		{ 3, 3, 0 },					// PF_BGR8_UNORM
		{ 4, 4, PFF_HasAlpha },			// PF_RGBA8_UNORM
		{ 4, 4, PFF_HasAlpha },			// PF_RGBA8_UNORM
		{ 4, 4, 0 },					// PF_RGBX8_UNORM
		{ 4, 4, 0 },					// PF_BGRX8_UNORM

		{ 2, 1, 0 },					// PF_R16_UNORM	
		{ 4, 2, 0 },					// PF_RG16_UNORM
		{ 6, 3, 0 },					// PF_RGB16_UNORM
		{ 8, 4, PFF_HasAlpha },			// PF_RGBA16_UNORM

		// signed normalized formats
		{ 1, 1, 0 },					// PF_R8_SNORM
		{ 2, 2, 0 },					// PF_RG8_SNORM
		{ 3, 3, 0 },				    // PF_RGB8_SNORM
		{ 4, 4, PFF_HasAlpha },			// PF_RGBA8_SNORM

		{ 2, 1, 0 },					// PF_R16_SNORM
		{ 4, 2, 0 },					// PF_RG16_SNORM
		{ 6, 3, 0 },					// PF_RGB16_SNORM
		{ 8, 4, PFF_HasAlpha },			// PF_RGBA16_SNORM

		// Unsigned integer formats
		{ 1, 1, 0 },					// PF_R8U
		{ 2, 2, 0 },					// PF_RG8U
		{ 3, 3, 0 },					// PF_RGB8U
		{ 4, 4, PFF_HasAlpha },			// PF_RGBA8U

		{ 2, 1, 0 },					// PF_R16U
		{ 4, 2, 0 },					// PF_RG16U
		{ 6, 3, 0 },					// PF_RGB16U
		{ 8, 4, PFF_HasAlpha },			// PF_RGBA16U

		{ 4,  1, 0 },					// PF_R32U
		{ 8,  2, 0 },					// PF_RG32U
		{ 12, 3, 0 },					// PF_RGB32U
		{ 16, 4, PFF_HasAlpha },		// PF_RGBA32U

		// Signed integer formats
		{ 1, 1, 0 },					// PF_R8U
		{ 2, 2, 0 },					// PF_RG8U
		{ 3, 3, 0 },					// PF_RGB8U
		{ 4, 4, PFF_HasAlpha },			// PF_RGBA8U

		{ 2, 1, 0 },					// PF_R16U
		{ 4, 2, 0 },					// PF_RG16U
		{ 6, 3, 0 },					// PF_RGB16U
		{ 8, 4, PFF_HasAlpha },			// PF_RGBA16U

		{ 4,  1, 0 },					// PF_R32U
		{ 8,  2, 0 },					// PF_RG32U
		{ 12, 3, 0 },					// PF_RGB32U
		{ 16, 4, PFF_HasAlpha },		// PF_RGBA32U

		// Floating formats
		{ 2, 1, 0 },					// PF_R16F
		{ 4, 2, 0 },					// PF_RG16F
		{ 6, 3, 0 },					// PF_RGB16F
		{ 8, 4, 0 },					// PF_RGBA16F
	
		{ 4,  1, 0 },					// PF_R32F
		{ 8,  2, 0 },					// PF_RG32F
		{ 12, 3, 0 },					// PF_RGB32F
		{ 16, 4, 0 },					// PF_RGBA32F

		// Packed formats
		{ 4, 3, 0 },					// PF_RGB9E5
		{ 4, 3, 0 },					// PF_RG11B10F
		{ 1, 3, 0 },					// PF_R3G3B2
		{ 2, 3, 0 },					// PF_B5G6R5
		{ 2, 4, PFF_HasAlpha },			// PF_B5G5R5A1
		{ 2, 4, PFF_HasAlpha },			// PF_RGBA4
		{ 4, 4, PFF_HasAlpha },			// PF_RGB10A2

		// Depth formats
		{ 2, 1, PFF_Depth },			// PF_D16
		{ 4, 2, PFF_Depth },			// PF_D24S8
		{ 4, 1, PFF_Depth },			// PF_D32F
		{ 8, 2, PFF_Depth },			// PF_D32FS8X24

		// Compressed formats
		{ 2, 3, PFF_Compressed },       // PF_RGB_DXT1
		{ 4, 4, PFF_Compressed },       // PF_RGBA_DXT1
		{ 4, 4, PFF_Compressed },       // PF_RGBA_DXT3
		{ 4, 4, PFF_Compressed },       // PF_RGBA_DXT5
								        
		// Undefined
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },
		{ 8, 4, PFF_Compressed },

		// sRGB formats
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_UNORM,
		{ 3, 3, PFF_sRGB },					//PF_SBGR8_UNORM,
		{ 4, 4, PFF_sRGB | PFF_HasAlpha },	//PF_SRGB8_ALPHA8_UNORM,
		{ 4, 4, PFF_sRGB | PFF_HasAlpha},	//PF_SBGR8_ALPHA8_UNORM,
		{ 4, 4, PFF_sRGB },					//PF_SRGBX8_UNORM,
		{ 4, 4, PFF_sRGB },					//PF_SBGRX8_UNORM,
		{ 3, 3, PFF_sRGB },					//PF_SRGB_DXT1,
		{ 3, 4, PFF_sRGB | PFF_HasAlpha },	//PF_SRGB_ALPHA_DXT1,
		{ 3, 4, PFF_sRGB | PFF_HasAlpha },	//PF_SRGB_ALPHA_DXT3,
		{ 3, 4, PFF_sRGB | PFF_HasAlpha },	//PF_SRGB_ALPHA_DXT5,
				
		{ 3, 3, PFF_sRGB },					//PF_SRGB_BP_UNORM,
		{ 3, 3, PFF_sRGB },					//PF_SRGB_PVRTC_2BPPV1,
		{ 3, 3, PFF_sRGB },					//PF_SRGB_PVRTC_4BPPV1,
		{ 3, 3, PFF_sRGB },					//PF_SRGB_ALPHA_PVRTC_2BPPV1,
		{ 3, 3, PFF_sRGB },					//PF_SRGB_ALPHA_PVRTC_4BPPV1,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_4x4,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_5x4,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_5x5,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_6x5,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_6x6,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_8x5,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_8x6,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_8x8,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_10x5,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_10x6,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_10x8,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_10x10,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_12x10,
		{ 3, 3, PFF_sRGB },					//PF_SRGB8_ALPHA8_ASTC_12x12,
	};

	static_assert(ARRAY_SIZE(PixelFormatDesc) == PF_Count, "PixelFormatDesc not match PixelFormat");
	return PixelFormatDesc[format];
}


bool PixelFormatUtils::IsDepth( PixelFormat format )
{
	return (GetPixelFormatDescription(format).Flags & PFF_Depth) != 0;
}

uint32_t PixelFormatUtils::GetNumElemBytes( PixelFormat format )
{
	return GetPixelFormatDescription(format).Bytes;
}

uint32_t PixelFormatUtils::GetComponentCount( PixelFormat format )
{
	return GetPixelFormatDescription(format).Component;
}

bool PixelFormatUtils::IsCompressed( PixelFormat format )
{
	return (GetPixelFormatDescription(format).Flags & PFF_Compressed) != 0;
}

void PixelFormatUtils::GetNumDepthStencilBits( PixelFormat format, uint32_t& depth, uint32_t& stencil )
{
	switch (format)
	{
	case PF_D16:		{ depth = 16; stencil = 0; } break;
	case PF_D24S8:		{ depth = 24; stencil = 8; } break;
	case PF_D32F:		{ depth = 32; stencil = 0; } break;
	case PF_D32FS8X24:  { depth = 32; stencil = 8; } break;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid depth format!", "PixelFormatUtils::GetNumDepthStencilBits");
	}
}

bool PixelFormatUtils::IsStencil( PixelFormat format )
{
	return (format == PF_D24S8 || format == PF_D32FS8X24);
}

} // Namespace RcEngineRcEngine