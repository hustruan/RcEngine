#ifndef _PixelFormat__H
#define _PixelFormat__H

#include <Core/Prerequisites.h>

namespace RcEngine {


/**
 * Pixel format which describes the way that the images in Textures store their data.
 *
 * LSB->MSB, that is PF_R8G8B8A8, store red in least significant bit.
 */
enum PixelFormat
{
	PF_Unknown = 0,

	// unsigned normalized formats
	PF_R8_UNORM,
	PF_RG8_UNORM,
	PF_RGB8_UNORM,
	PF_BGR8_UNORM,
	PF_RGBA8_UNORM,
	PF_BGRA8_UNORM,
	PF_RGBX8_UNORM,
	PF_BGRX8_UNORM,

	PF_R16_UNORM,
	PF_RG16_UNORM,
	PF_RGB16_UNORM,
	PF_RGBA16_UNORM,

	// signed normalized formats
	PF_R8_SNORM,
	PF_RG8_SNORM,
	PF_RGB8_SNORM,
	PF_RGBA8_SNORM,

	PF_R16_SNORM,
	PF_RG16_SNORM,
	PF_RGB16_SNORM,
	PF_RGBA16_SNORM,

	// Unsigned integer formats
	PF_R8U,
	PF_RG8U,
	PF_RGB8U,
	PF_RGBA8U,

	PF_R16U,
	PF_RG16U,
	PF_RGB16U,
	PF_RGBA16U,

	PF_R32U,
	PF_RG32U,
	PF_RGB32U,
	PF_RGBA32U,

	// Signed integer formats
	PF_R8I,
	PF_RG8I,
	PF_RGB8I,
	PF_RGBA8I,

	PF_R16I,
	PF_RG16I,
	PF_RGB16I,
	PF_RGBA16I,

	PF_R32I,
	PF_RG32I,
	PF_RGB32I,
	PF_RGBA32I,

	// Floating formats
	PF_R16F,
	PF_RG16F,
	PF_RGB16F,
	PF_RGBA16F,

	PF_R32F,
	PF_RG32F,
	PF_RGB32F,
	PF_RGBA32F,

	// Packed formats
	PF_RGB9E5,
	PF_RG11B10F,
	PF_R3G3B2,
	PF_B5G6R5,
	PF_BGR5A1,
	PF_RGBA4,
	PF_RGB10A2,

	// Depth formats
	PF_D16,
	PF_D24S8,
	PF_D32F,
	PF_D32FS8X24,

	// Compressed formats
	PF_RGB_DXT1_UNORM,
	PF_RGBA_DXT1_UNORM,
	PF_RGBA_DXT3_UNORM,
	PF_RGBA_DXT5_UNORM,
	PF_R_ATI1N_UNORM,
	PF_R_ATI1N_SNORM,
	PF_RG_ATI2N_UNORM,
	PF_RG_ATI2N_SNORM,
	PF_RGB_BP_UNSIGNED_FLOAT,
	PF_RGB_BP_SIGNED_FLOAT,
	PF_RGB_BP_UNORM,
	PF_RGB_PVRTC_4BPPV1,
	PF_RGB_PVRTC_2BPPV1,
	PF_RGBA_PVRTC_4BPPV1,
	PF_RGBA_PVRTC_2BPPV1,
	PF_ATC_RGB,
	PF_ATC_RGBA_EXPLICIT_ALPHA,
	PF_ATC_RGBA_INTERPOLATED_ALPHA,
	PF_RGBA_ASTC_4x4,
	PF_RGBA_ASTC_5x4,
	PF_RGBA_ASTC_5x5,
	PF_RGBA_ASTC_6x5,
	PF_RGBA_ASTC_6x6,
	PF_RGBA_ASTC_8x5,
	PF_RGBA_ASTC_8x6,
	PF_RGBA_ASTC_8x8,
	PF_RGBA_ASTC_10x5,
	PF_RGBA_ASTC_10x6,
	PF_RGBA_ASTC_10x8,
	PF_RGBA_ASTC_10x10,
	PF_RGBA_ASTC_12x10,
	PF_RGBA_ASTC_12x12,

	// sRGB formats
	PF_SRGB8_UNORM,
	PF_SBGR8_UNORM,
	PF_SRGB8_ALPHA8_UNORM,
	PF_SBGR8_ALPHA8_UNORM,
	PF_SRGBX8_UNORM,
	PF_SBGRX8_UNORM,
	PF_SRGB_DXT1_UNORM,
	PF_SRGB_ALPHA_DXT1_UNORM,
	PF_SRGB_ALPHA_DXT3_UNORM,
	PF_SRGB_ALPHA_DXT5_UNORM, 
	PF_SRGB_BP_UNORM,
	PF_SRGB_PVRTC_2BPPV1,
	PF_SRGB_PVRTC_4BPPV1,
	PF_SRGB_ALPHA_PVRTC_2BPPV1,
	PF_SRGB_ALPHA_PVRTC_4BPPV1,
	PF_SRGB8_ALPHA8_ASTC_4x4,
	PF_SRGB8_ALPHA8_ASTC_5x4,
	PF_SRGB8_ALPHA8_ASTC_5x5,
	PF_SRGB8_ALPHA8_ASTC_6x5,
	PF_SRGB8_ALPHA8_ASTC_6x6,
	PF_SRGB8_ALPHA8_ASTC_8x5,
	PF_SRGB8_ALPHA8_ASTC_8x6,
	PF_SRGB8_ALPHA8_ASTC_8x8,
	PF_SRGB8_ALPHA8_ASTC_10x5,
	PF_SRGB8_ALPHA8_ASTC_10x6,
	PF_SRGB8_ALPHA8_ASTC_10x8,
	PF_SRGB8_ALPHA8_ASTC_10x10,
	PF_SRGB8_ALPHA8_ASTC_12x10,
	PF_SRGB8_ALPHA8_ASTC_12x12,

	PF_Count
};

//enum PixelComponentType
//{
//	PCT_Byte = 0,    /// Byte per component (8 bit fixed 0.0..1.0)
//	PCT_Short = 1,   /// Short per component (16 bit fixed 0.0..1.0))
//	PCT_Float16 = 2, /// 16 bit float per component
//	PCT_Float32 = 3, /// 32 bit float per component
//	PCT_Count = 4    /// Number of pixel types
//};

class _ApiExport PixelFormatUtils
{
public:
		
	static uint32_t GetNumElemBytes(PixelFormat format);

	static uint32_t GetComponentCount(PixelFormat fmt);

	/** Shortcut method to determine if the format has an alpha component */
	static bool HasAlpha(PixelFormat format);

	/** Shortcut method to determine if the format is floating point */
	static bool IsFloatingPoint(PixelFormat format);

	/** Shortcut method to determine if the format is compressed */
	static bool IsCompressed(PixelFormat format);

	/** Shortcut method to determine if the format is a depth format. */
	static bool IsDepth(PixelFormat format);

	static bool IsStencil(PixelFormat format);

	static void GetNumDepthStencilBits(PixelFormat format, uint32_t& depth, uint32_t& stencil);		
};


} // Namespace RcEngine




#endif // PixelFormat_h__