#ifndef D3D11RenderState_h__
#define D3D11RenderState_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RenderState.h>

namespace RcEngine {

class _D3D11Export D3D11DepthStencilState : public DepthStencilState
{
public:
	D3D11DepthStencilState(const DepthStencilStateDesc& desc);
	~D3D11DepthStencilState();

public:
	ID3D11DepthStencilState* StateD3D11;
};

class _D3D11Export D3D11BlendState : public BlendState
{
public:
	D3D11BlendState(const BlendStateDesc& desc);
	~D3D11BlendState();

public:
	ID3D11BlendState* StateD3D11;
};

class _D3D11Export D3D11RasterizerState : public RasterizerState
{
public:
	D3D11RasterizerState(const RasterizerStateDesc& desc);
	~D3D11RasterizerState();

public:
	ID3D11RasterizerState* StateD3D11;
};

class _D3D11Export D3D11SamplerState : public SamplerState
{
public:
	D3D11SamplerState(const SamplerStateDesc& desc);
	~D3D11SamplerState();

public:
	ID3D11SamplerState* StateD3D11;
};
	
}



#endif // D3D11RenderState_h__