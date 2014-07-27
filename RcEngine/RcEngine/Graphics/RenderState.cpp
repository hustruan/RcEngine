#include <Graphics/RenderState.h>

namespace RcEngine {

DepthStencilStateDesc::DepthStencilStateDesc()
	: DepthEnable(true), 
	  DepthWriteMask(true), 
	  DepthFunc(CF_Less),
	  StencilEnable(false),
	  StencilReadMask(DEFAULT_STENCIL_READ_MASK), 
	  StencilWriteMask(DEFAULT_STENCIL_WRITE_MASK), 
	  FrontStencilFunc(CF_Always),
	  FrontStencilFailOp(SOP_Keep), 
	  FrontStencilDepthFailOp(SOP_Keep), 
	  FrontStencilPassOp(SOP_Keep), 
	  BackStencilFunc(CF_Always),
	  BackStencilFailOp(SOP_Keep), 
	  BackStencilDepthFailOp(SOP_Keep), 
	  BackStencilPassOp(SOP_Keep)
{

}

bool operator< (const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
BlendStateDesc::RenderTargetBlendDesc::RenderTargetBlendDesc()
	: BlendEnable(false), 
	  SrcBlend(ABF_One), 
	  DestBlend(ABF_Zero), 
	  BlendOp(BOP_Add),
	  SrcBlendAlpha(ABF_One), 
	  DestBlendAlpha(ABF_Zero), 
	  BlendOpAlpha(BOP_Add),
	  ColorWriteMask(CWM_All)
{

}

BlendStateDesc::BlendStateDesc()
	: AlphaToCoverageEnable(false), 
	  IndependentBlendEnable(false)
{
}

bool operator< (const BlendStateDesc& lhs, const BlendStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
RasterizerStateDesc::RasterizerStateDesc()
	: PolygonFillMode(FM_Solid), 
	  PolygonCullMode(CM_Back), 
	  FrontCounterClockwise(false),
	  DepthBias(0.0f), 
	  SlopeScaledDepthBias(0.0f),
	  DepthClipEnable(true), 
	  ScissorEnable(false), 
	  MultisampleEnable(false)
{

}

bool operator< (const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
SamplerStateDesc::SamplerStateDesc()
	: AddressU(TAM_Clamp), 
	  AddressV(TAM_Clamp), 
	  AddressW(TAM_Clamp), 
	  Filter(TF_Min_Mag_Mip_Linear),
	  BorderColor(1, 1, 1, 1),
	  MipLODBias(0),
	  MinLOD(-FLT_MAX),
	  MaxLOD(FLT_MAX),
	  MaxAnisotropy(16),
	  ComparisonFunc(CF_Never),
	  //BindStage(ST_Pixel),
	  CompareSampler(false)
{

}

bool operator< (const SamplerStateDesc& lhs, const SamplerStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

}


