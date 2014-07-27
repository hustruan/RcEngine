#ifndef D3D11FrameBuffer_h__
#define D3D11FrameBuffer_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/FrameBuffer.h>

namespace RcEngine {

class _D3D11Export D3D11FrameBuffer : public FrameBuffer
{
public:
	D3D11FrameBuffer(uint32_t width, uint32_t height);

	virtual void SwapBuffers();
	virtual void OnBind();
	virtual void OnUnbind();

protected:

};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11DepthStencilView : public RenderView
{
public:
	D3D11DepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level, uint32_t flags);
	virtual ~D3D11DepthStencilView();

	virtual void ClearColor(const ColorRGBA& clr);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void ClearDepthStencil(float depth, uint32_t stencil);

protected:
	virtual void OnAttach(FrameBuffer& fb, Attachment attr) {}
	virtual void OnDetach(FrameBuffer& fb, Attachment attr) {}

public:
	ID3D11DepthStencilView* DepthStencilViewD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11TargetView : public RenderView
{
public:
	D3D11TargetView(const shared_ptr<Texture>& texture);
	virtual ~D3D11TargetView();

	virtual void ClearColor(const ColorRGBA& clr);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void ClearDepthStencil(float depth, uint32_t stencil);

protected:
	virtual void OnAttach(FrameBuffer& fb, Attachment attr) {}
	virtual void OnDetach(FrameBuffer& fb, Attachment attr) {}

public:
	ID3D11RenderTargetView* RenderTargetViewD3D11;
};

class _D3D11Export D3D11RenderTargetView2D : public D3D11TargetView
{
public:
	D3D11RenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level);
	
	// For back color buffer
	D3D11RenderTargetView2D(ID3D11RenderTargetView* rtv);
};

class _D3D11Export D3D11RenderTargetViewArray : public D3D11TargetView
{
public:
	D3D11RenderTargetViewArray(const shared_ptr<Texture>& texture, uint32_t level);
};

}


#endif // D3D11FrameBuffer_h__