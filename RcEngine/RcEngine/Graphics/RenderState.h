#ifndef RenderState_h__
#define RenderState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

#define	DEFAULT_STENCIL_READ_MASK	( 0xff )
#define	DEFAULT_STENCIL_WRITE_MASK	( 0xff )

#pragma pack(push, 1)

struct _ApiExport DepthStencilStateDesc
{
	DepthStencilStateDesc();

	bool                       DepthEnable;
	bool					   DepthWriteMask;
	CompareFunction		       DepthFunc;

	bool                       StencilEnable;
	uint16_t                   StencilReadMask;
	uint16_t                   StencilWriteMask;

	StencilOperation		   FrontStencilFailOp;
	StencilOperation		   FrontStencilDepthFailOp;
	StencilOperation		   FrontStencilPassOp;
	CompareFunction			   FrontStencilFunc;

	StencilOperation		   BackStencilFailOp;
	StencilOperation		   BackStencilDepthFailOp;
	StencilOperation		   BackStencilPassOp;
	CompareFunction			   BackStencilFunc;

	
	friend bool operator< (const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs);
};

struct _ApiExport BlendStateDesc
{
public:
	struct RenderTargetBlendDesc
	{
		RenderTargetBlendDesc();

		bool				BlendEnable;
		AlphaBlendFactor    SrcBlend;
		AlphaBlendFactor    DestBlend;
		BlendOperation		BlendOp;
		AlphaBlendFactor    SrcBlendAlpha;
		AlphaBlendFactor    DestBlendAlpha;
		BlendOperation		BlendOpAlpha;
		uint8_t				ColorWriteMask;
	};

public:
	BlendStateDesc();

	bool AlphaToCoverageEnable;
	bool IndependentBlendEnable;
	RenderTargetBlendDesc RenderTarget[8];

	friend bool operator< (const BlendStateDesc& lhs, const BlendStateDesc& rhs);
};

struct _ApiExport RasterizerStateDesc
{
public:
	RasterizerStateDesc();

	FillMode		PolygonFillMode;
	CullMode		PolygonCullMode;
	bool            FrontCounterClockwise;
	float           DepthBias;
	float           SlopeScaledDepthBias;
	bool            DepthClipEnable;
	bool            ScissorEnable;
	bool            MultisampleEnable;

	friend bool operator< (const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs);
};

struct _ApiExport SamplerStateDesc
{
public:
	SamplerStateDesc();

	//ShaderType			   BindStage;
	bool					   CompareSampler;

	TextureFilter              Filter;
	TextureAddressMode		   AddressU;
	TextureAddressMode		   AddressV;
	TextureAddressMode		   AddressW;
	float                      MipLODBias;
	uint8_t                    MaxAnisotropy;
	CompareFunction            ComparisonFunc;
	float                      MinLOD;
	float                      MaxLOD;

	ColorRGBA			       BorderColor;	

	friend bool operator< (const SamplerStateDesc& lhs, const SamplerStateDesc& rhs);
};

#pragma pack(pop)


class _ApiExport BlendState
{
public:
	BlendState(const BlendStateDesc& desc)  
		: mDesc(desc) {}
	virtual ~BlendState() { }

	inline const BlendStateDesc& GetDesc() const { return mDesc; }

protected:
	BlendStateDesc mDesc;
};

class _ApiExport DepthStencilState
{
public:
	DepthStencilState(const DepthStencilStateDesc& desc) 
		: mDesc(desc) { }
	virtual ~DepthStencilState() { }

	inline const DepthStencilStateDesc& GetDesc() const { return mDesc; } 

protected:
	DepthStencilStateDesc mDesc;
};

class _ApiExport RasterizerState
{
public:
	RasterizerState(const RasterizerStateDesc& desc)
		: mDesc(desc) { }
	virtual ~RasterizerState() { }

	inline const RasterizerStateDesc& GetDesc() const { return mDesc; }

protected:
	RasterizerStateDesc mDesc;
};

class _ApiExport SamplerState
{
public:
	SamplerState(const SamplerStateDesc& desc) 
		: mDesc(desc) { }
	virtual ~SamplerState() { }

	inline  const SamplerStateDesc& GetDesc( ) const { return mDesc; }

protected:
	SamplerStateDesc mDesc;
};

} // Namespace RcEngine

#endif // RenderState_h__
