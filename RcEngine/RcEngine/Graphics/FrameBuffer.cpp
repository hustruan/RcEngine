#include <Graphics/FrameBuffer.h>
#include <Graphics/GraphicsResource.h>
#include <Core/Exception.h>
#include <Graphics/Camera.h>

namespace RcEngine {

//////////////////////////////////////////////////////////////////////////
RenderView::RenderView( const shared_ptr<Texture>& texture )
	: mTexture(texture)
{

}

//////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer( uint32_t width, uint32_t height )
	: mWidth(width),
	  mHeight(height)
{
	mViewports.push_back(Viewport(0.f, 0.f, float(width), float(height)));
}

FrameBuffer::~FrameBuffer()
{

}

void FrameBuffer::SetViewport( const Viewport& vp )
{
	mViewports[0] = vp;
}

void FrameBuffer::SetViewport( uint32_t index, const Viewport& vp )
{
	if (mViewports.size() < index + 1)
		mViewports.resize(index + 1);

	mViewports[index] = vp;
}

shared_ptr<RenderView> FrameBuffer::GetRTV( Attachment att ) const
{
	switch(att)
	{
	case ATT_DepthStencil:
		return mDepthStencilView;

	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1)
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Try to get render target view which it not exit!", "RHFrameBuffer::GetRenderTargetView");
		}
		else
			return mColorViews[index];
	}
}

shared_ptr<UnorderedAccessView> FrameBuffer::GetUAV( uint32_t index ) const
{
	if (index >= mUnorderedAccessViews.size())
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Try to get unordered access view which it not exit!", "RHFrameBuffer::GetUnorderedAccessView");
	
	return mUnorderedAccessViews[index];
}

void FrameBuffer::AttachRTV( Attachment attchment, const shared_ptr<RenderView>& view )
{
	switch(attchment)
	{
	case ATT_DepthStencil:
		{
			if(mDepthStencilView)
			{
				DetachRTV(ATT_DepthStencil);
			}
			mDepthStencilView = view;
		}
		break;
	default:
		{
			uint32_t index = attchment - ATT_Color0;

			// if it already has an render target attach it, detach if first. 
			if(index < mColorViews.size() && mColorViews[index])
				DetachRTV(attchment);

			if(mColorViews.size() < index + 1)
				mColorViews.resize(index + 1);

			mColorViews[index] = view;
		}
	}

	if (view)
		view->OnAttach(*this, attchment);
}

void FrameBuffer::AttachUAV( uint32_t index, const shared_ptr<UnorderedAccessView>& uav )
{
	if(mUnorderedAccessViews.size() < index + 1)
		mUnorderedAccessViews.resize(index + 1);

	mUnorderedAccessViews[index] = uav;
}

void FrameBuffer::DetachUAV( uint32_t index )
{
	if(mUnorderedAccessViews.size() < index + 1)
		mUnorderedAccessViews.resize(index + 1);

	mUnorderedAccessViews[index] = nullptr;
}

void FrameBuffer::DetachRTV( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		if(mDepthStencilView)
		{
			mDepthStencilView->OnDetach(*this, att);
			mDepthStencilView = nullptr;
		}
		break;
	default:
		uint32_t index = att - ATT_Color0;

		if(mColorViews.size() < index + 1)
			mColorViews.resize(index + 1);

		if(mColorViews[index])
		{
			mColorViews[index]->OnDetach(*this, att);
			mColorViews[index] = nullptr;
		}
	}
}

void FrameBuffer::DetachAll()
{
	for (size_t i = 0; i < mColorViews.size(); ++i)
	{
		if (mColorViews[i])
		{
			DetachRTV((Attachment)(ATT_DepthStencil + i));
		}
	}
	mColorViews.clear();

	if (mDepthStencilView)
		DetachRTV(ATT_DepthStencil);

	mUnorderedAccessViews.clear();
}

void FrameBuffer::Clear( uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	//RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// frame buffer must binded before clear
	//shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	//assert( this == currentFrameBuffer.get());

	if (flags & CF_Color)
	{
		for (size_t i = 0; i < mColorViews.size(); ++i)
		{
			if (mColorViews[i])
				mColorViews[i]->ClearColor(clr);	
		}
	}

	if ( mDepthStencilView )
	{
		if ( (flags & CF_Stencil) && (flags & CF_Depth) )
			mDepthStencilView->ClearDepthStencil(depth, stencil);
		else if (flags & CF_Depth)
			mDepthStencilView->ClearDepth(depth);
		else if (flags & CF_Stencil)
			mDepthStencilView->ClearStencil(stencil);
	}
}

void FrameBuffer::Resize( uint32_t width, uint32_t height )
{
	mWidth = width;
	mHeight = height;
}

}