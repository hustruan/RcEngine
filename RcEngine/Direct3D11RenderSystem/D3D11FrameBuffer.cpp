#include "D3D11FrameBuffer.h"
#include "D3D11Device.h"
#include "D3D11View.h"

namespace RcEngine {

D3D11FrameBuffer::D3D11FrameBuffer( uint32_t width, uint32_t height )
	:FrameBuffer(width, height)
{

}

void D3D11FrameBuffer::SwapBuffers()
{

}

void D3D11FrameBuffer::OnBind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	vector<ID3D11RenderTargetView*> rtvD3D11;
	for (const auto& colorView : mColorViews)
	{
		if (colorView)
		{
			rtvD3D11.push_back((static_cast_checked<D3D11TargetView*>(colorView.get())->RenderTargetViewD3D11));
		}
	}

	ID3D11RenderTargetView** pRTVD3D11 = rtvD3D11.size() ? (&rtvD3D11[0]) : nullptr;

	ID3D11DepthStencilView* dsvD3D11 = nullptr;
	if (mDepthStencilView)
		dsvD3D11 = (static_cast_checked<D3D11DepthStencilView*>(mDepthStencilView.get())->DepthStencilViewD3D11);

	if (mUnorderedAccessViews.size())
	{		
		vector<ID3D11UnorderedAccessView*> uavD3D11;
		for (const auto& uavView : mUnorderedAccessViews)
		{
			uavD3D11.push_back(static_cast_checked<D3D11UnorderedAccessView*>(uavView.get())->UnorderedAccessViewD3D11);
		}
		
		ID3D11UnorderedAccessView** pUAVD3D11 = uavD3D11.size() ? (&uavD3D11[0]) : nullptr;

		deviceContextD3D11->OMSetRenderTargetsAndUnorderedAccessViews(rtvD3D11.size(), pRTVD3D11, dsvD3D11,
			0, uavD3D11.size(), pUAVD3D11, nullptr);
	}
	else
	{
		deviceContextD3D11->OMSetRenderTargets(rtvD3D11.size(), pRTVD3D11, dsvD3D11);
	}

	// Set viewports
	gD3D11Device->SetViewports(mViewports);
}

void D3D11FrameBuffer::OnUnbind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	
	static ID3D11RenderTargetView* NullRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };

	if (mUnorderedAccessViews.size())
	{
		static ID3D11UnorderedAccessView* NullUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT] = { nullptr };
		 
		deviceContextD3D11->OMSetRenderTargetsAndUnorderedAccessViews(
			mColorViews.size(), 
			nullptr,
			nullptr,
			0,
			mUnorderedAccessViews.size(),
			NullUAVs,
			nullptr);
	}
	else
	{
		deviceContextD3D11->OMSetRenderTargets(mColorViews.size(), NullRTVs, nullptr);
	}
}








}


