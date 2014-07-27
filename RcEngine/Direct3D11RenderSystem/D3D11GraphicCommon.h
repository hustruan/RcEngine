#ifndef D3D11GraphicCommon_h__
#define D3D11GraphicCommon_h__

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

class _D3D11Export D3D11Mapping
{
public:

	static void Mapping(uint32_t accessHint, D3D11_USAGE& usage, UINT& CPUAccessFlags); 
	static DXGI_FORMAT Mapping(VertexElementFormat format);
	static DXGI_FORMAT Mapping(PixelFormat inPixelFormat);
	static D3D11_COMPARISON_FUNC Mapping(CompareFunction cmpFunc);
	static D3D11_STENCIL_OP Mapping(StencilOperation sop);
	static D3D11_BLEND_OP Mapping(BlendOperation bop);
	static D3D11_BLEND Mapping(AlphaBlendFactor blend);
	static D3D11_CULL_MODE Mapping(CullMode mode); 
	static D3D11_FILL_MODE Mapping(FillMode mode);
	static D3D11_TEXTURE_ADDRESS_MODE Mapping(TextureAddressMode mode);
	static D3D11_FILTER Mapping(TextureFilter filter, bool compare = false);
	static uint32_t Mapping(uint32_t bufferCreateFlags); 
	static D3D10_PRIMITIVE_TOPOLOGY Mapping(PrimitiveType primType); 
	static D3D11_MAP Mapping(ResourceMapAccess map); 

	static DXGI_FORMAT GetDepthTextureFormat(PixelFormat inPixelFormat);
	static DXGI_FORMAT GetDepthShaderResourceFormat(PixelFormat inPixelFormat);
	static UINT GetCPUAccessFlags(ResourceMapAccess map);

	static void UnMapping(D3D10_SRV_DIMENSION dimension, EffectParameterType& oTexType);
};

}




#endif // D3D11GraphicCommon_h__
