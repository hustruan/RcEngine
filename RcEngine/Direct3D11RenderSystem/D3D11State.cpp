#include "D3D11State.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11DepthStencilState::D3D11DepthStencilState( const DepthStencilStateDesc& desc )
	: DepthStencilState(desc),
	  StateD3D11(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	depthStencilDesc.DepthEnable = mDesc.DepthEnable;
	depthStencilDesc.DepthWriteMask = mDesc.DepthWriteMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11Mapping::Mapping(mDesc.DepthFunc);

	depthStencilDesc.StencilEnable = mDesc.StencilEnable;
	depthStencilDesc.StencilReadMask =  static_cast<UINT8>(mDesc.StencilReadMask);
	depthStencilDesc.StencilWriteMask = static_cast<UINT8>(mDesc.StencilWriteMask);
	
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11Mapping::Mapping(mDesc.FrontStencilDepthFailOp);
	depthStencilDesc.FrontFace.StencilFailOp = D3D11Mapping::Mapping(mDesc.FrontStencilFailOp);
	depthStencilDesc.FrontFace.StencilPassOp = D3D11Mapping::Mapping(mDesc.FrontStencilPassOp);
	depthStencilDesc.FrontFace.StencilFunc = D3D11Mapping::Mapping(mDesc.FrontStencilFunc);

	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11Mapping::Mapping(mDesc.BackStencilDepthFailOp);
	depthStencilDesc.BackFace.StencilFailOp = D3D11Mapping::Mapping(mDesc.BackStencilFailOp);
	depthStencilDesc.BackFace.StencilPassOp = D3D11Mapping::Mapping(mDesc.BackStencilPassOp);
	depthStencilDesc.BackFace.StencilFunc = D3D11Mapping::Mapping(mDesc.BackStencilFunc);

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;	
	D3D11_VERRY( deviceD3D11->CreateDepthStencilState(&depthStencilDesc, &StateD3D11) );
}
D3D11DepthStencilState::~D3D11DepthStencilState()
{
	SAFE_RELEASE(StateD3D11);
}

D3D11BlendState::D3D11BlendState( const BlendStateDesc& desc )
	: BlendState(desc),
	  StateD3D11(nullptr)
{
	CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

	blendDesc.AlphaToCoverageEnable = mDesc.AlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = mDesc.IndependentBlendEnable;
	for (int i = 0; i < 8; ++i)
	{
		blendDesc.RenderTarget[i].BlendEnable = mDesc.RenderTarget[i].BlendEnable;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = mDesc.RenderTarget[i].ColorWriteMask;
		blendDesc.RenderTarget[i].BlendOp = D3D11Mapping::Mapping(mDesc.RenderTarget[i].BlendOp);
		blendDesc.RenderTarget[i].BlendOpAlpha = D3D11Mapping::Mapping(mDesc.RenderTarget[i].BlendOpAlpha);
		blendDesc.RenderTarget[i].DestBlend = D3D11Mapping::Mapping(mDesc.RenderTarget[i].DestBlend);
		blendDesc.RenderTarget[i].DestBlendAlpha = D3D11Mapping::Mapping(mDesc.RenderTarget[i].DestBlendAlpha);
		blendDesc.RenderTarget[i].SrcBlend = D3D11Mapping::Mapping(mDesc.RenderTarget[i].SrcBlend);
		blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11Mapping::Mapping(mDesc.RenderTarget[i].SrcBlendAlpha);
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;	
	D3D11_VERRY( deviceD3D11->CreateBlendState(&blendDesc, &StateD3D11) );
}

D3D11BlendState::~D3D11BlendState()
{
	SAFE_RELEASE(StateD3D11);
}


D3D11RasterizerState::D3D11RasterizerState( const RasterizerStateDesc& desc )
	: RasterizerState(desc),
	  StateD3D11(nullptr)
{
	CD3D11_RASTERIZER_DESC rasDesc(D3D11_DEFAULT);

	rasDesc.FillMode = D3D11Mapping::Mapping(mDesc.PolygonFillMode);
	rasDesc.CullMode = D3D11Mapping::Mapping(mDesc.PolygonCullMode);
	rasDesc.FrontCounterClockwise = mDesc.FrontCounterClockwise;
	rasDesc.DepthBias = INT(mDesc.DepthBias);
	//rasDesc.DepthBiasClamp = 
	rasDesc.DepthClipEnable = mDesc.DepthClipEnable;
	rasDesc.ScissorEnable = mDesc.ScissorEnable;
	rasDesc.MultisampleEnable = mDesc.MultisampleEnable;
	rasDesc.SlopeScaledDepthBias  = mDesc.SlopeScaledDepthBias;
	//rasDesc.AntialiasedLineEnable;
	
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;	
	D3D11_VERRY(deviceD3D11->CreateRasterizerState(&rasDesc, &StateD3D11));
}

D3D11RasterizerState::~D3D11RasterizerState()
{
	SAFE_RELEASE(StateD3D11);
}

D3D11SamplerState::D3D11SamplerState( const SamplerStateDesc& desc )
	: SamplerState(desc),
	  StateD3D11(nullptr)
{
	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);

	samplerDesc.AddressU = D3D11Mapping::Mapping(mDesc.AddressU);
	samplerDesc.AddressV = D3D11Mapping::Mapping(mDesc.AddressV);
	samplerDesc.AddressW = D3D11Mapping::Mapping(mDesc.AddressW);
	
	samplerDesc.BorderColor[0] =  mDesc.BorderColor[0];
	samplerDesc.BorderColor[1] =  mDesc.BorderColor[1];
	samplerDesc.BorderColor[2] =  mDesc.BorderColor[2];
	samplerDesc.BorderColor[3] =  mDesc.BorderColor[3];

	samplerDesc.ComparisonFunc = D3D11Mapping::Mapping(mDesc.ComparisonFunc);
	samplerDesc.Filter = D3D11Mapping::Mapping(mDesc.Filter);

	samplerDesc.MinLOD = mDesc.MinLOD;
	samplerDesc.MaxLOD = mDesc.MaxLOD;
	samplerDesc.MaxAnisotropy = mDesc.MaxAnisotropy;
	
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;	
	D3D11_VERRY( deviceD3D11->CreateSamplerState(&samplerDesc, &StateD3D11) );
}

D3D11SamplerState::~D3D11SamplerState()
{
	SAFE_RELEASE(StateD3D11);
}

}
