#include <Graphics/Image.h>
#include <Graphics/GraphicsResource.h>
#include <IO/FileStream.h>
#include <Core/Exception.h>

namespace RcEngine {

enum D3D11_RESOURCE_DIMENSION
{
	D3D11_RESOURCE_DIMENSION_UNKNOWN	= 0,
	D3D11_RESOURCE_DIMENSION_BUFFER		= 1,
	D3D11_RESOURCE_DIMENSION_TEXTURE1D	= 2,
	D3D11_RESOURCE_DIMENSION_TEXTURE2D	= 3,
	D3D11_RESOURCE_DIMENSION_TEXTURE3D	= 4
} 	D3D11_RESOURCE_DIMENSION;

typedef enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN	                    = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
	DXGI_FORMAT_R32G32B32A32_UINT           = 3,
	DXGI_FORMAT_R32G32B32A32_SINT           = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
	DXGI_FORMAT_R32G32B32_FLOAT             = 6,
	DXGI_FORMAT_R32G32B32_UINT              = 7,
	DXGI_FORMAT_R32G32B32_SINT              = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
	DXGI_FORMAT_R16G16B16A16_UINT           = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
	DXGI_FORMAT_R16G16B16A16_SINT           = 14,
	DXGI_FORMAT_R32G32_TYPELESS             = 15,
	DXGI_FORMAT_R32G32_FLOAT                = 16,
	DXGI_FORMAT_R32G32_UINT                 = 17,
	DXGI_FORMAT_R32G32_SINT                 = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
	DXGI_FORMAT_R10G10B10A2_UINT            = 25,
	DXGI_FORMAT_R11G11B10_FLOAT             = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
	DXGI_FORMAT_R8G8B8A8_UINT               = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
	DXGI_FORMAT_R8G8B8A8_SINT               = 32,
	DXGI_FORMAT_R16G16_TYPELESS             = 33,
	DXGI_FORMAT_R16G16_FLOAT                = 34,
	DXGI_FORMAT_R16G16_UNORM                = 35,
	DXGI_FORMAT_R16G16_UINT                 = 36,
	DXGI_FORMAT_R16G16_SNORM                = 37,
	DXGI_FORMAT_R16G16_SINT                 = 38,
	DXGI_FORMAT_R32_TYPELESS                = 39,
	DXGI_FORMAT_D32_FLOAT                   = 40,
	DXGI_FORMAT_R32_FLOAT                   = 41,
	DXGI_FORMAT_R32_UINT                    = 42,
	DXGI_FORMAT_R32_SINT                    = 43,
	DXGI_FORMAT_R24G8_TYPELESS              = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
	DXGI_FORMAT_R8G8_TYPELESS               = 48,
	DXGI_FORMAT_R8G8_UNORM                  = 49,
	DXGI_FORMAT_R8G8_UINT                   = 50,
	DXGI_FORMAT_R8G8_SNORM                  = 51,
	DXGI_FORMAT_R8G8_SINT                   = 52,
	DXGI_FORMAT_R16_TYPELESS                = 53,
	DXGI_FORMAT_R16_FLOAT                   = 54,
	DXGI_FORMAT_D16_UNORM                   = 55,
	DXGI_FORMAT_R16_UNORM                   = 56,
	DXGI_FORMAT_R16_UINT                    = 57,
	DXGI_FORMAT_R16_SNORM                   = 58,
	DXGI_FORMAT_R16_SINT                    = 59,
	DXGI_FORMAT_R8_TYPELESS                 = 60,
	DXGI_FORMAT_R8_UNORM                    = 61,
	DXGI_FORMAT_R8_UINT                     = 62,
	DXGI_FORMAT_R8_SNORM                    = 63,
	DXGI_FORMAT_R8_SINT                     = 64,
	DXGI_FORMAT_A8_UNORM                    = 65,
	DXGI_FORMAT_R1_UNORM                    = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
	DXGI_FORMAT_BC1_TYPELESS                = 70,
	DXGI_FORMAT_BC1_UNORM                   = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
	DXGI_FORMAT_BC2_TYPELESS                = 73,
	DXGI_FORMAT_BC2_UNORM                   = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
	DXGI_FORMAT_BC3_TYPELESS                = 76,
	DXGI_FORMAT_BC3_UNORM                   = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
	DXGI_FORMAT_BC4_TYPELESS                = 79,
	DXGI_FORMAT_BC4_UNORM                   = 80,
	DXGI_FORMAT_BC4_SNORM                   = 81,
	DXGI_FORMAT_BC5_TYPELESS                = 82,
	DXGI_FORMAT_BC5_UNORM                   = 83,
	DXGI_FORMAT_BC5_SNORM                   = 84,
	DXGI_FORMAT_B5G6R5_UNORM                = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
	DXGI_FORMAT_BC6H_TYPELESS               = 94,
	DXGI_FORMAT_BC6H_UF16                   = 95,
	DXGI_FORMAT_BC6H_SF16                   = 96,
	DXGI_FORMAT_BC7_TYPELESS                = 97,
	DXGI_FORMAT_BC7_UNORM                   = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
	DXGI_FORMAT_AYUV                        = 100,
	DXGI_FORMAT_Y410                        = 101,
	DXGI_FORMAT_Y416                        = 102,
	DXGI_FORMAT_NV12                        = 103,
	DXGI_FORMAT_P010                        = 104,
	DXGI_FORMAT_P016                        = 105,
	DXGI_FORMAT_420_OPAQUE                  = 106,
	DXGI_FORMAT_YUY2                        = 107,
	DXGI_FORMAT_Y210                        = 108,
	DXGI_FORMAT_Y216                        = 109,
	DXGI_FORMAT_NV11                        = 110,
	DXGI_FORMAT_AI44                        = 111,
	DXGI_FORMAT_IA44                        = 112,
	DXGI_FORMAT_P8                          = 113,
	DXGI_FORMAT_A8P8                        = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM              = 115,

	// Add for 24 bit foramt, not a part of DXGI
	DXGI_FORMAT_R8G8B8_UNORM,
	DXGI_FORMAT_B8G8R8_UNORM
} DXGI_FORMAT;

static PixelFormat DXGI2PixelFormat[] = {

	PF_Unknown,// DXGI_FORMAT_UNKNOWN						= 0,
	PF_Unknown,//DXGI_FORMAT_R32G32B32A32_TYPELESS        = 1,
	PF_RGBA32F,//DXGI_FORMAT_R32G32B32A32_FLOAT           = 2,
	PF_RGBA32U,//DXGI_FORMAT_R32G32B32A32_UINT            = 3,
	PF_RGBA32I,//DXGI_FORMAT_R32G32B32A32_SINT            = 4,
	PF_Unknown,//DXGI_FORMAT_R32G32B32_TYPELESS           = 5,
	PF_RGB32F,//DXGI_FORMAT_R32G32B32_FLOAT              = 6,
	PF_RGB32U,//DXGI_FORMAT_R32G32B32_UINT               = 7,
	PF_RGB32I,//DXGI_FORMAT_R32G32B32_SINT               = 8,
	PF_Unknown,//DXGI_FORMAT_R16G16B16A16_TYPELESS        = 9,
	PF_RGBA16F,//DXGI_FORMAT_R16G16B16A16_FLOAT           = 10,
	PF_RGBA16_UNORM,//DXGI_FORMAT_R16G16B16A16_UNORM           = 11,
	PF_RGBA16U,//DXGI_FORMAT_R16G16B16A16_UINT            = 12,
	PF_RGBA16_SNORM,//DXGI_FORMAT_R16G16B16A16_SNORM           = 13,
	PF_RGBA16I,//DXGI_FORMAT_R16G16B16A16_SINT            = 14,
	PF_Unknown,//DXGI_FORMAT_R32G32_TYPELESS              = 15,
	PF_RG32F,//DXGI_FORMAT_R32G32_FLOAT                 = 16,
	PF_RG32U,//DXGI_FORMAT_R32G32_UINT                  = 17,
	PF_RG32I,//DXGI_FORMAT_R32G32_SINT                  = 18,
	PF_Unknown,//DXGI_FORMAT_R32G8X24_TYPELESS            = 19,
	PF_D32FS8X24,//DXGI_FORMAT_D32_FLOAT_S8X24_UINT         = 20,
	PF_Unknown,//DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS     = 21,
	PF_Unknown,//DXGI_FORMAT_X32_TYPELESS_G8X24_UINT      = 22,
	PF_Unknown,//DXGI_FORMAT_R10G10B10A2_TYPELESS         = 23,
	PF_RGB10A2,//DXGI_FORMAT_R10G10B10A2_UNORM            = 24,
	PF_RGB10A2,//DXGI_FORMAT_R10G10B10A2_UINT             = 25,
	PF_RG11B10F,//DXGI_FORMAT_R11G11B10_FLOAT              = 26,
	PF_Unknown,//DXGI_FORMAT_R8G8B8A8_TYPELESS            = 27,
	PF_RGBA8_UNORM,//DXGI_FORMAT_R8G8B8A8_UNORM               = 28,
	PF_SRGB8_ALPHA8_UNORM,//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB          = 29,
	PF_RGBA8U,//DXGI_FORMAT_R8G8B8A8_UINT                = 30,
	PF_RGBA8_SNORM,//DXGI_FORMAT_R8G8B8A8_SNORM               = 31,
	PF_RGBA8I,//DXGI_FORMAT_R8G8B8A8_SINT                = 32,
	PF_Unknown,//DXGI_FORMAT_R16G16_TYPELESS              = 33,
	PF_RG16F,//DXGI_FORMAT_R16G16_FLOAT                 = 34,
	PF_RG16_UNORM,//DXGI_FORMAT_R16G16_UNORM                 = 35,
	PF_RG16U,//DXGI_FORMAT_R16G16_UINT                  = 36,
	PF_RG16_SNORM,//DXGI_FORMAT_R16G16_SNORM                 = 37,
	PF_RG16I,//DXGI_FORMAT_R16G16_SINT                  = 38,
	PF_Unknown,//DXGI_FORMAT_R32_TYPELESS                 = 39,
	PF_D32F,//DXGI_FORMAT_D32_FLOAT                    = 40,
	PF_R32F,//DXGI_FORMAT_R32_FLOAT                    = 41,
	PF_R32U,//DXGI_FORMAT_R32_UINT                     = 42,
	PF_R32I,//DXGI_FORMAT_R32_SINT                     = 43,
	PF_Unknown,//DXGI_FORMAT_R24G8_TYPELESS               = 44,
	PF_D24S8,//DXGI_FORMAT_D24_UNORM_S8_UINT            = 45,
	PF_Unknown,//DXGI_FORMAT_R24_UNORM_X8_TYPELESS        = 46,
	PF_Unknown,//DXGI_FORMAT_X24_TYPELESS_G8_UINT         = 47,
	PF_Unknown,//DXGI_FORMAT_R8G8_TYPELESS                = 48,
	PF_RG8_UNORM,//DXGI_FORMAT_R8G8_UNORM                   = 49,
	PF_RG8U,//DXGI_FORMAT_R8G8_UINT                    = 50,
	PF_RG8_SNORM,//DXGI_FORMAT_R8G8_SNORM                   = 51,
	PF_RG8I,//DXGI_FORMAT_R8G8_SINT                    = 52,
	PF_Unknown,//DXGI_FORMAT_R16_TYPELESS                 = 53,
	PF_R16F,//DXGI_FORMAT_R16_FLOAT                    = 54,
	PF_D16,//DXGI_FORMAT_D16_UNORM                    = 55,
	PF_R16_UNORM,//DXGI_FORMAT_R16_UNORM                    = 56,
	PF_R16U,//DXGI_FORMAT_R16_UINT                     = 57,
	PF_R16_SNORM,//DXGI_FORMAT_R16_SNORM                    = 58,
	PF_R16I,//DXGI_FORMAT_R16_SINT                     = 59,
	PF_Unknown,//DXGI_FORMAT_R8_TYPELESS                  = 60,
	PF_R8_UNORM,//DXGI_FORMAT_R8_UNORM                     = 61,
	PF_R8U,//DXGI_FORMAT_R8_UINT                      = 62,
	PF_R8_SNORM,//DXGI_FORMAT_R8_SNORM                     = 63,
	PF_R8I,//DXGI_FORMAT_R8_SINT                      = 64,
	PF_Unknown,//DXGI_FORMAT_A8_UNORM                     = 65,
	PF_Unknown,//DXGI_FORMAT_R1_UNORM                     = 66,
	PF_RGB9E5,//DXGI_FORMAT_R9G9B9E5_SHAREDEXP           = 67,
	PF_Unknown,//DXGI_FORMAT_R8G8_B8G8_UNORM              = 68,
	PF_Unknown,//DXGI_FORMAT_G8R8_G8B8_UNORM              = 69,
	PF_Unknown,//DXGI_FORMAT_BC1_TYPELESS                 = 70,
	PF_RGB_DXT1_UNORM,//DXGI_FORMAT_BC1_UNORM                    = 71,
	PF_SRGB_DXT1_UNORM,//DXGI_FORMAT_BC1_UNORM_SRGB               = 72,
	PF_Unknown,//DXGI_FORMAT_BC2_TYPELESS                 = 73,
	PF_RGBA_DXT3_UNORM,//DXGI_FORMAT_BC2_UNORM                    = 74,
	PF_SRGB_ALPHA_DXT3_UNORM,//DXGI_FORMAT_BC2_UNORM_SRGB               = 75,
	PF_Unknown,//DXGI_FORMAT_BC3_TYPELESS                 = 76,
	PF_RGBA_DXT5_UNORM,//DXGI_FORMAT_BC3_UNORM                    = 77,
	PF_SRGB_ALPHA_DXT5_UNORM,//DXGI_FORMAT_BC3_UNORM_SRGB               = 78,
	PF_Unknown,//DXGI_FORMAT_BC4_TYPELESS                 = 79,
	PF_R_ATI1N_UNORM,//DXGI_FORMAT_BC4_UNORM                    = 80,
	PF_R_ATI1N_SNORM,//DXGI_FORMAT_BC4_SNORM                    = 81,
	PF_Unknown,//DXGI_FORMAT_BC5_TYPELESS                 = 82,
	PF_RG_ATI2N_UNORM,//DXGI_FORMAT_BC5_UNORM                    = 83,
	PF_RG_ATI2N_SNORM,//DXGI_FORMAT_BC5_SNORM                    = 84,
	PF_B5G6R5,//DXGI_FORMAT_B5G6R5_UNORM                 = 85,
	PF_BGR5A1,//DXGI_FORMAT_B5G5R5A1_UNORM               = 86,
	PF_BGRA8_UNORM,//DXGI_FORMAT_B8G8R8A8_UNORM               = 87,
	PF_BGRX8_UNORM,//DXGI_FORMAT_B8G8R8X8_UNORM               = 88,
	PF_Unknown,//DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM   = 89,
	PF_Unknown,//DXGI_FORMAT_B8G8R8A8_TYPELESS            = 90,
	PF_SBGR8_ALPHA8_UNORM,//DXGI_FORMAT_B8G8R8A8_UNORM_SRGB          = 91,
	PF_Unknown,//DXGI_FORMAT_B8G8R8X8_TYPELESS            = 92,
	PF_SBGRX8_UNORM,//DXGI_FORMAT_B8G8R8X8_UNORM_SRGB          = 93,
	PF_Unknown,//DXGI_FORMAT_BC6H_TYPELESS                = 94,
	PF_RGB_BP_UNSIGNED_FLOAT,//DXGI_FORMAT_BC6H_UF16                    = 95,
	PF_RGB_BP_SIGNED_FLOAT,//DXGI_FORMAT_BC6H_SF16                    = 96,
	PF_Unknown,//DXGI_FORMAT_BC7_TYPELESS                 = 97,
	PF_RGB_BP_UNORM,//DXGI_FORMAT_BC7_UNORM                    = 98,
	PF_SRGB_BP_UNORM,//DXGI_FORMAT_BC7_UNORM_SRGB               = 99,

	PF_Unknown,//DXGI_FORMAT_AYUV                        = 100,
	PF_Unknown,//DXGI_FORMAT_Y410                        = 101,
	PF_Unknown,//DXGI_FORMAT_Y416                        = 102,
	PF_Unknown,//DXGI_FORMAT_NV12                        = 103,
	PF_Unknown,//DXGI_FORMAT_P010                        = 104,
	PF_Unknown,//DXGI_FORMAT_P016                        = 105,
	PF_Unknown,//DXGI_FORMAT_420_OPAQUE                  = 106,
	PF_Unknown,//DXGI_FORMAT_YUY2                        = 107,
	PF_Unknown,//DXGI_FORMAT_Y210                        = 108,
	PF_Unknown,//DXGI_FORMAT_Y216                        = 109,
	PF_Unknown,//DXGI_FORMAT_NV11                        = 110,
	PF_Unknown,//DXGI_FORMAT_AI44                        = 111,
	PF_Unknown,//DXGI_FORMAT_IA44                        = 112,
	PF_Unknown,//DXGI_FORMAT_P8                          = 113,
	PF_Unknown,//DXGI_FORMAT_A8P8                        = 114,
	PF_Unknown,//DXGI_FORMAT_B4G4R4A4_UNORM              = 115,

	PF_RGB8_UNORM,
	PF_BGR8_UNORM,
};

//--------------------------------------------------------------------------------------
// Macro
//--------------------------------------------------------------------------------------
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)									 \
((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |        \
((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

//--------------------------------------------------------------------------------------
// DDS file structure definitions
//--------------------------------------------------------------------------------------
#pragma pack(push,1)

const uint32_t DDS_MAGIC = 0x20534444; // "DDS "

struct DDS_PIXELFORMAT
{
	uint32_t    size;
	uint32_t    flags;
	uint32_t    fourCC;
	uint32_t    RGBBitCount;
	uint32_t    RBitMask;
	uint32_t    GBitMask;
	uint32_t    BBitMask;
	uint32_t    ABitMask;
};

struct DDS_HEADER
{
	uint32_t        size;
	uint32_t        flags;
	uint32_t        height;
	uint32_t        width;
	uint32_t        pitchOrLinearSize;
	uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	uint32_t        mipMapCount;
	uint32_t        reserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t        caps;
	uint32_t        caps2;
	uint32_t        caps3;
	uint32_t        caps4;
	uint32_t        reserved2;
};

struct DDS_HEADER_DXT10
{
	DXGI_FORMAT     dxgiFormat;
	uint32_t        resourceDimension;
	uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
	uint32_t        arraySize;
	uint32_t        miscFlags2;
};

#pragma pack(pop)

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH  0x00000004 // DDSD_WIDTH

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define	D3D11_REQ_SAMPLER_OBJECT_COUNT_PER_DEVICE			( 4096 )
#define	D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION			( 2048 )
#define	D3D11_REQ_TEXTURE1D_U_DIMENSION						( 16384 )
#define	D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION			( 2048 )
#define	D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION				( 16384 )
#define	D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION				( 2048 )
#define	D3D11_REQ_TEXTURECUBE_DIMENSION						( 16384 )
#define	D3D11_RESINFO_INSTRUCTION_MISSING_COMPONENT_RETVAL	( 0 )
#define	D3D11_REQ_MIP_LEVELS								( 15 )

#define D3D11_RESOURCE_MISC_TEXTURECUBE						(0x4L)

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
	DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
	DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

enum DDS_MISC_FLAGS2
{
	DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L,
};


//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
static size_t BitsPerPixel(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_B8G8R8_UNORM:
	case DXGI_FORMAT_R8G8B8_UNORM:
		return 24;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

	default:
		return 0;
	}
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
static void GetSurfaceInfo(
	size_t width,
	size_t height,
	DXGI_FORMAT fmt,
	size_t* outNumBytes,
	size_t* outRowBytes,
	size_t* outNumRows
	)
{
	size_t numBytes = 0;
	size_t rowBytes = 0;
	size_t numRows = 0;

	bool bc = false;
	bool packed  = false;
	size_t bcnumBytesPerBlock = 0;
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bc = true;
		bcnumBytesPerBlock = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		bc = true;
		bcnumBytesPerBlock = 16;
		break;

	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		packed = true;
		break;
	}

	if (bc)
	{
		size_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
		}
		size_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
		}
		rowBytes = numBlocksWide * bcnumBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else if (packed)
	{
		rowBytes = ((width + 1) >> 1) * 4;
		numRows = height;
	}
	else
	{
		size_t bpp = BitsPerPixel(fmt);
		rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
	}

	numBytes = rowBytes * numRows;
	if (outNumBytes)
	{
		*outNumBytes = numBytes;
	}
	if (outRowBytes)
	{
		*outRowBytes = rowBytes;
	}
	if (outNumRows)
	{
		*outNumRows = numRows;
	}
}


//--------------------------------------------------------------------------------------
#define ISBITMASK(r, g, b, a) (ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a)

static DXGI_FORMAT GetDXGIFormat(const DDS_PIXELFORMAT& ddpf)
{
	if (ddpf.flags & DDS_RGB)
	{
		// Note that sRGB formats are written using the "DX10" extended header

		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
			{
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
			{
				return DXGI_FORMAT_B8G8R8X8_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000) aka D3DFMT_X8B8G8R8

			// Note that many common DDS reader/writers (including D3DX) swap the
			// the RED/BLUE masks for 10:10:10:2 formats. We assumme
			// below that the 'backwards' header mask is being used since it is most
			// likely written by D3DX. The more robust solution is to use the 'DX10'
			// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

			// For 'correct' writers, this should be 0x000003ff, 0x000ffc00, 0x3ff00000 for RGB data
			if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
			{
				return DXGI_FORMAT_R10G10B10A2_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) aka D3DFMT_A2R10G10B10

			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16G16_UNORM;
			}

			if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
			{
				// Only 32-bit color channel format in D3D9 was R32F
				return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
			}
			break;

		case 24:
			// No 24bpp DXGI formats aka D3DFMT_R8G8B8
			if (ddpf.RBitMask  == 0x00FF0000 && ddpf.GBitMask == 0x0000FF00 && ddpf.BBitMask == 0x000000FF)
				return DXGI_FORMAT_B8G8R8_UNORM;

			if (ddpf.RBitMask  == 0x000000FF && ddpf.GBitMask == 0x0000FF00 && ddpf.BBitMask == 0x00FF0000)
				return DXGI_FORMAT_R8G8B8_UNORM;

			break;
		case 16:
			if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
			{
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			}
			if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
			{
				return DXGI_FORMAT_B5G6R5_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x0000) aka D3DFMT_X1R5G5B5
			if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
			{
				return DXGI_FORMAT_B4G4R4A4_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x0f00, 0x00f0, 0x000f, 0x0000) aka D3DFMT_X4R4G4B4

			// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
			break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
			}

			// No DXGI format maps to ISBITMASK(0x0f, 0x00, 0x00, 0xf0) aka D3DFMT_A4L4
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
			}
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
			{
				return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
			}
		}
	}
	else if (ddpf.flags & DDS_ALPHA)
	{
		if (8 == ddpf.RGBBitCount)
		{
			return DXGI_FORMAT_A8_UNORM;
		}
	}
	else if (ddpf.flags & DDS_FOURCC)
	{
		if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC1_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC2_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC3_UNORM;
		}

		// While pre-mulitplied alpha isn't directly supported by the DXGI formats,
		// they are basically the same as these BC formats so they can be mapped
		if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC2_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC3_UNORM;
		}

		if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_SNORM;
		}

		if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_SNORM;
		}

		// BC6H and BC7 are written using the "DX10" extended header

		if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
		{
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
		}
		if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
		{
			return DXGI_FORMAT_G8R8_G8B8_UNORM;
		}

		// Check for D3DFORMAT enums being set here
		switch (ddpf.fourCC)
		{
		case 36: // D3DFMT_A16B16G16R16
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case 110: // D3DFMT_Q16W16V16U16
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case 111: // D3DFMT_R16F
			return DXGI_FORMAT_R16_FLOAT;

		case 112: // D3DFMT_G16R16F
			return DXGI_FORMAT_R16G16_FLOAT;

		case 113: // D3DFMT_A16B16G16R16F
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case 114: // D3DFMT_R32F
			return DXGI_FORMAT_R32_FLOAT;

		case 115: // D3DFMT_G32R32F
			return DXGI_FORMAT_R32G32_FLOAT;

		case 116: // D3DFMT_A32B32G32R32F
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}


bool Image::LoadImageFromDDS( const String& filename )
{
	// clear any previously loaded images
	Clear();

	FileStream stream;
	if (stream.Open(filename, FILE_READ) == false)
		return false;

	// Need at least enough data to fill the header and magic number to be a valid DDS
	if (stream.GetSize() < (sizeof(DDS_HEADER) + sizeof(uint32_t)))
		return false;

	// DDS files always start with the same magic number ("DDS ")
	if (stream.ReadUInt() != DDS_MAGIC)
		return false;

	// read in DDS header
	DDS_HEADER ddsHeader;
	stream.Read(&ddsHeader, sizeof(DDS_HEADER));

	// Verify header to validate DDS file
	if (ddsHeader.size != sizeof(DDS_HEADER) || ddsHeader.ddspf.size != sizeof(DDS_PIXELFORMAT))
		return false;

	// Check for DX10 extension
	bool bDXT10Header = false;
	if ((ddsHeader.ddspf.flags & DDS_FOURCC) && (MAKEFOURCC( 'D', 'X', '1', '0' ) == ddsHeader.ddspf.fourCC))
	{
		// Must be long enough for both headers and magic value
		if ( stream.GetSize() < ( sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10) ) )
			return false;

		bDXT10Header = true;
	}

	// Get texture info
	mWidth = ddsHeader.width;
	mHeight = ddsHeader.height;
	mDepth = ddsHeader.depth;
	mLevels = std::max(1U, ddsHeader.mipMapCount);
	mLayers = 1; 

	mType = TT_Texture2D;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	if (bDXT10Header)
	{
		DDS_HEADER_DXT10 dds10Ext;
		stream.Read(&dds10Ext, sizeof(DDS_HEADER_DXT10));

		mLayers = dds10Ext.arraySize;
		if (mLayers == 0)
		{
			return false;
		}

		switch( dds10Ext.dxgiFormat )
		{
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
		case DXGI_FORMAT_A8P8:
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

		default:
			if ( BitsPerPixel( dds10Ext.dxgiFormat ) == 0 )
			{
				return false;
				//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
			}
		}

		format = dds10Ext.dxgiFormat;

		switch ( dds10Ext.resourceDimension )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			// D3DX writes 1D textures with a fixed Height of 1
			if ((ddsHeader.flags & DDS_HEIGHT) && ddsHeader.height != 1)
			{
				return false;
				//return HRESULT_FROM_WIN32( ERROR_INVALID_DATA );
			}

			mType = TT_Texture1D;
			mHeight = mDepth = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			if (dds10Ext.miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				mType = TT_TextureCube;
			}
			else
			{
				mType = TT_Texture2D;
			}
			mDepth = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			if (!(ddsHeader.flags & DDS_HEADER_FLAGS_VOLUME))
			{
				return false;
				//return HRESULT_FROM_WIN32( ERROR_INVALID_DATA );
			}

			if (mLevels > 1)
			{
				return false;
				//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
			}

			mType = TT_Texture3D;
			break;

		default:
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
	}
	else
	{
		format = GetDXGIFormat( ddsHeader.ddspf );

		if (format == DXGI_FORMAT_UNKNOWN)
		{
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}

		if (ddsHeader.flags & DDS_HEADER_FLAGS_VOLUME)
		{
			mType = TT_Texture3D;
		}
		else 
		{
			if (ddsHeader.caps2 & DDS_CUBEMAP)
			{
				// We require all six faces to be defined
				if ((ddsHeader.caps2 & DDS_CUBEMAP_ALLFACES ) != DDS_CUBEMAP_ALLFACES)
				{
					return false;
					//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
				}

				mType = TT_TextureCube;
			}
			else
			{
				mType = TT_Texture2D;
			}

			mDepth = 1;
			// Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
		}

		assert( BitsPerPixel( format ) != 0 );
	}

	// Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
	if (mLevels > D3D11_REQ_MIP_LEVELS)
	{
		return false;
		//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
	}

	switch ( mType )
	{
	case TT_Texture1D:
		if ((mLevels > D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION) || (mWidth > D3D11_REQ_TEXTURE1D_U_DIMENSION) )
		{
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
		break;
	case TT_Texture2D:
		if ((mLevels > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
			(mWidth > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) ||
			(mHeight > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION))
		{
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
		break;
	case TT_Texture3D:
		if ((mLevels > 1) ||
			(mWidth > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
			(mHeight > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
			(mDepth > D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) )
		{
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
		break;
	case TT_TextureCube:
		// This is the right bound because we set arraySize to (NumCubes*6) above
		if ((mLevels * 6 > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
			(mWidth > D3D11_REQ_TEXTURECUBE_DIMENSION)||
			(mHeight > D3D11_REQ_TEXTURECUBE_DIMENSION))
		{
			return false;
			//return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
		break;
	}

	// Unmaping to Engine Pixel Format
	mFormat = DXGI2PixelFormat[format];

	uint32_t imageDataSize = stream.GetSize() - stream.GetPosition();
	uint8_t* pData = new uint8_t[imageDataSize];
	stream.Read(pData, imageDataSize);

	size_t NumBytes = 0;
	size_t RowBytes = 0;

	uint8_t* pSrcBits = pData;
	uint8_t* pEndBits = pData + imageDataSize;

	// load all surfaces for the image (6 surfaces for cubemaps)
	for (uint32_t j = 0; j < uint32_t(mType == TT_TextureCube ? mLayers * 6 : mLayers); j++) 
	{
		size_t w = mWidth;
		size_t h = mHeight;
		size_t d = mDepth;

		for (uint32_t i = 0; i < mLevels; ++i)
		{
			GetSurfaceInfo(w, h, format, &NumBytes, &RowBytes, nullptr);

			size_t totalSize = NumBytes*d;

			SurfaceInfo surface = { pSrcBits, RowBytes, NumBytes };
			mSurfaces.push_back(surface);

			if (pSrcBits + totalSize > pEndBits)
			{
				return false;
				// return HRESULT_FROM_WIN32( ERROR_HANDLE_EOF );
			}

			pSrcBits += totalSize;

			w = std::max(1U, w >> 1);
			h = std::max(1U, h >> 1);
			d = std::max(1U, d >> 1);
		}

	}

	assert(stream.GetPosition() == stream.GetSize());

	mValid = true;
	return mValid;
}

//bool Image::SaveImageToDDS( const String& filename )
//{
//	// Setup header
//	const size_t MAX_HEADER_SIZE = sizeof(uint32_t) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
//	uint8_t fileHeader[ MAX_HEADER_SIZE ];
//
//	*reinterpret_cast<uint32_t*>(&fileHeader[0]) = DDS_MAGIC;
//
//	auto header = reinterpret_cast<DDS_HEADER*>( &fileHeader[0] + sizeof(uint32_t) );
//	size_t headerSize = sizeof(uint32_t) + sizeof(DDS_HEADER);
//	memset( header, 0, sizeof(DDS_HEADER) );
//	header->size = sizeof( DDS_HEADER );
//	header->flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
//	header->height = mWidth;
//	header->width = mHeight;
//	header->mipMapCount = 1;
//	header->caps = DDS_SURFACE_FLAGS_TEXTURE;
//
//	// Try to use a legacy .DDS pixel format for better tools support, otherwise fallback to 'DX10' header extension
//	DDS_HEADER_DXT10* extHeader = nullptr;
//	switch(mFormat)
//	{
//	case PF_RGBA8_UNORM:					memcpy(&header->ddspf,  &DDSPF_A8B8G8R8, sizeof(DDS_PIXELFORMAT) );    break;
//	case PF_RG16_UNORM:						memcpy( &header->ddspf, &DDSPF_G16R16, sizeof(DDS_PIXELFORMAT) );      break;
//	case PF_RG8_UNORM:						memcpy( &header->ddspf, &DDSPF_A8L8, sizeof(DDS_PIXELFORMAT) );        break;
//	case PF_R16_UNORM:						memcpy( &header->ddspf, &DDSPF_L16, sizeof(DDS_PIXELFORMAT) );         break;
//	case PF_R8_UNORM:						memcpy( &header->ddspf, &DDSPF_L8, sizeof(DDS_PIXELFORMAT) );          break;
//	//case PF_A8_UNORM:						memcpy( &header->ddspf, &DDSPF_A8, sizeof(DDS_PIXELFORMAT) );          break;
//	//case PF_R8G8_B8G8_UNORM:				memcpy( &header->ddspf, &DDSPF_R8G8_B8G8, sizeof(DDS_PIXELFORMAT) );   break;
//	//case PF_G8R8_G8B8_UNORM:				memcpy( &header->ddspf, &DDSPF_G8R8_G8B8, sizeof(DDS_PIXELFORMAT) );   break;
//	case PF_RGBA_DXT1_UNORM:				memcpy( &header->ddspf, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT) );        break;
//	case PF_RGBA_DXT3_UNORM:				memcpy( &header->ddspf, &DDSPF_DXT3, sizeof(DDS_PIXELFORMAT) );        break;
//	case PF_RGBA_DXT5_UNORM:				memcpy( &header->ddspf, &DDSPF_DXT5, sizeof(DDS_PIXELFORMAT) );        break;
//	case PF_R_ATI1N_UNORM:					memcpy( &header->ddspf, &DDSPF_BC4_UNORM, sizeof(DDS_PIXELFORMAT) );   break;
//	case PF_R_ATI1N_SNORM:					memcpy( &header->ddspf, &DDSPF_BC4_SNORM, sizeof(DDS_PIXELFORMAT) );   break;
//	case PF_RG_ATI2N_UNORM:					memcpy( &header->ddspf, &DDSPF_BC5_UNORM, sizeof(DDS_PIXELFORMAT) );   break;
//	case PF_RG_ATI2N_SNORM:					memcpy( &header->ddspf, &DDSPF_BC5_SNORM, sizeof(DDS_PIXELFORMAT) );   break;
//	case PF_B5G6R5:							memcpy( &header->ddspf, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT) );      break;
//	case PF_BGR5A1:							memcpy( &header->ddspf, &DDSPF_A1R5G5B5, sizeof(DDS_PIXELFORMAT) );    break;
//	case PF_BGRA8_UNORM:					memcpy( &header->ddspf, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT) );    break; // DXGI 1.1
//	case PF_BGRX8_UNORM:					memcpy( &header->ddspf, &DDSPF_X8R8G8B8, sizeof(DDS_PIXELFORMAT) );    break; // DXGI 1.1
//	//case PF_YUY2:							memcpy( &header->ddspf, &DDSPF_YUY2, sizeof(DDS_PIXELFORMAT) );        break; // DXGI 1.2
//	//case PF_B4G4R4A4_UNORM:				memcpy( &header->ddspf, &DDSPF_A4R4G4B4, sizeof(DDS_PIXELFORMAT) );    break; // DXGI 1.2
//		
//	// Legacy D3DX formats using D3DFMT enum value as FourCC
//	case PF_RGBA32F:					    header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 116; break; // D3DFMT_A32B32G32R32F
//	case PF_RGBA16F:					    header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 113; break; // D3DFMT_A16B16G16R16F
//	case PF_RGBA16_UNORM:				    header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 36;  break; // D3DFMT_A16B16G16R16
//	case PF_RGBA16_SNORM:					header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 110; break; // D3DFMT_Q16W16V16U16
//	case PF_RG32F:							header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 115; break; // D3DFMT_G32R32F
//	case PF_RG16F:							header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 112; break; // D3DFMT_G16R16F
//	case PF_R32F:							header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 114; break; // D3DFMT_R32F
//	case PF_R16F:							header->ddspf.size = sizeof(DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 111; break; // D3DFMT_R16F
//
//	//case PF_AI44:
//	//case PF_IA44:
//	//case PF_P8:
//	//case PF_A8P8:
//	//	return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
//
//	default:
//		memcpy_s( &header->ddspf, sizeof(header->ddspf), &DDSPF_DX10, sizeof(DDS_PIXELFORMAT) );
//
//		headerSize += sizeof(DDS_HEADER_DXT10);
//		extHeader = reinterpret_cast<DDS_HEADER_DXT10*>( reinterpret_cast<uint8_t*>(&fileHeader[0]) + sizeof(uint32_t) + sizeof(DDS_HEADER) );
//		memset( extHeader, 0, sizeof(DDS_HEADER_DXT10) );
//		extHeader->dxgiFormat = desc.Format;
//		extHeader->resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
//		extHeader->arraySize = 1;
//		break;
//	}
//
//	//size_t rowPitch, slicePitch, rowCount;
//	//GetSurfaceInfo( desc.Width, desc.Height, desc.Format, &slicePitch, &rowPitch, &rowCount );
//
//	//if ( IsCompressed( desc.Format ) )
//	//{
//	//	header->flags |= DDS_HEADER_FLAGS_LINEARSIZE;
//	//	header->pitchOrLinearSize = static_cast<uint32_t>( slicePitch );
//	//}
//	//else
//	//{
//	//	header->flags |= DDS_HEADER_FLAGS_PITCH;
//	//	header->pitchOrLinearSize = static_cast<uint32_t>( rowPitch );
//	//}
//
//	//// Setup pixels
//	//std::unique_ptr<uint8_t[]> pixels( new (std::nothrow) uint8_t[ slicePitch ] );
//	//if (!pixels)
//	//	return E_OUTOFMEMORY;
//
//	//D3D11_MAPPED_SUBRESOURCE mapped;
//	//hr = pContext->Map( pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped );
//	//if ( FAILED(hr) )
//	//	return hr;
//
//	//auto sptr = reinterpret_cast<const uint8_t*>( mapped.pData );
//	//if ( !sptr )
//	//{
//	//	pContext->Unmap( pStaging.Get(), 0 );
//	//	return E_POINTER;
//	//}
//
//	//uint8_t* dptr = pixels.get();
//
//	//size_t msize = std::min<size_t>( rowPitch, mapped.RowPitch );
//	//for( size_t h = 0; h < rowCount; ++h )
//	//{
//	//	memcpy_s( dptr, rowPitch, sptr, msize );
//	//	sptr += mapped.RowPitch;
//	//	dptr += rowPitch;
//	//}
//
//	//pContext->Unmap( pStaging.Get(), 0 );
//
//	//// Write header & pixels
//	//DWORD bytesWritten;
//	//if ( !WriteFile( hFile.get(), fileHeader, static_cast<DWORD>( headerSize ), &bytesWritten, 0 ) )
//	//	return HRESULT_FROM_WIN32( GetLastError() );
//
//	//if ( bytesWritten != headerSize )
//	//	return E_FAIL;
//
//	//if ( !WriteFile( hFile.get(), pixels.get(), static_cast<DWORD>( slicePitch ), &bytesWritten, 0 ) )
//	//	return HRESULT_FROM_WIN32( GetLastError() );
//
//	//if ( bytesWritten != slicePitch )
//	//	return E_FAIL;
//
//	//return S_OK;
//}

}