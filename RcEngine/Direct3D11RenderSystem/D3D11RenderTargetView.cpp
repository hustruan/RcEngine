#include "D3D11FrameBuffer.h"
#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11TargetView::D3D11TargetView( const shared_ptr<Texture>& texture )
	: RenderView(texture),
	  RenderTargetViewD3D11(nullptr)
{

}

D3D11TargetView::~D3D11TargetView()
{
	SAFE_RELEASE(RenderTargetViewD3D11);
}


void D3D11TargetView::ClearDepth( float depth )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoundn't call here!", "D3D11DepthStencilView::ClearColor");
}

void D3D11TargetView::ClearStencil( uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoundn't call here!", "D3D11DepthStencilView::ClearColor");
}

void D3D11TargetView::ClearDepthStencil( float depth, uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoundn't call here!", "D3D11DepthStencilView::ClearColor");
}

void D3D11TargetView::ClearColor( const ColorRGBA& clr )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	deviceContextD3D11->ClearRenderTargetView(RenderTargetViewD3D11, clr());
}

//////////////////////////////////////////////////////////////////////////
D3D11RenderTargetView2D::D3D11RenderTargetView2D( ID3D11RenderTargetView* rtv )
	: D3D11TargetView(nullptr)
{
	RenderTargetViewD3D11 = rtv;
}

D3D11RenderTargetView2D::D3D11RenderTargetView2D( const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level )
	: D3D11TargetView(texture)
{
	assert(texture->GetCreateFlags() & TexCreate_RenderTarget);

	D3D11_RENDER_TARGET_VIEW_DESC  viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	uint32_t textureArraySize = texture->GetTextureArraySize();
	if (textureArraySize <= 1)
	{
		if (texture->GetSampleCount() > 1)
		{
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			viewDesc.Texture2D.MipSlice = level;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
	}
	else
	{
		if (texture->GetSampleCount() > 1)
		{
			viewDesc.Texture2DMSArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DMSArray.ArraySize = 1;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		}
		else
		{
			viewDesc.Texture2DArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DArray.ArraySize = 1;
			viewDesc.Texture2DArray.MipSlice = level;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		}
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Texture2D* textureD3D11 = (static_cast_checked<D3D11Texture2D*>(texture.get()))->TextureD3D11;
	D3D11_VERRY( deviceD3D11->CreateRenderTargetView(textureD3D11, &viewDesc, &RenderTargetViewD3D11) );
}

D3D11RenderTargetViewArray::D3D11RenderTargetViewArray( const shared_ptr<Texture>& texture, uint32_t level )
	: D3D11TargetView(texture)
{
	assert(texture->GetCreateFlags() & TexCreate_RenderTarget);
	assert(texture->GetTextureArraySize() > 1);

	D3D11_RENDER_TARGET_VIEW_DESC  viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (texture->GetSampleCount() > 1)
	{
		viewDesc.Texture2DMSArray.FirstArraySlice = 0;
		viewDesc.Texture2DMSArray.ArraySize = texture->GetTextureArraySize();
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
	}
	else
	{
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = texture->GetTextureArraySize();
		viewDesc.Texture2DArray.MipSlice = level;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Texture2D* textureD3D11 = (static_cast_checked<D3D11Texture2D*>(texture.get()))->TextureD3D11;
	D3D11_VERRY( deviceD3D11->CreateRenderTargetView(textureD3D11, &viewDesc, &RenderTargetViewD3D11) );
}

}