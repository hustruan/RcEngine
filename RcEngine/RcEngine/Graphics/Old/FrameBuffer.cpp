#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderView.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Camera.h>
#include <Core/Context.h>

namespace RcEngine {

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height,  bool offscreen /*= true*/ )
	: mWidth(width), 
	  mHeight(height), 
	  mDepthStencilView(0),
	  mDirty(true), 
	  mIsDepthBuffered(false),
	  mOffscreen(offscreen),
	  mViewport(0, 0, width, height)
{
	mCamera = std::make_shared<Camera>();
}

FrameBuffer::~FrameBuffer(void)
{
	
}

void FrameBuffer::SetViewport( const Viewport& vp )
{
	if (mViewport != vp)
	{
		mViewport = vp;
	}
}

shared_ptr<RenderView> FrameBuffer::GetAttachedView( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		return mDepthStencilView;

	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1)
			return NULL;
		else
			return mColorViews[index];
	}
}

void FrameBuffer::Attach( Attachment att, const shared_ptr<RenderView>& view)
{
	switch(att)
	{
	case ATT_DepthStencil:
		{
			if(mDepthStencilView)
			{
				Detach(ATT_DepthStencil);
			}
			mIsDepthBuffered = true;
			mDepthStencilView = view;
			PixelFormatUtils::GetNumDepthStencilBits(view->GetFormat(), mDepthBits, mStencilBits);
		}
		break;
	default:
		{
			uint32_t index = att - ATT_Color0;

			// if it already has an render target attach it, detach if first. 
			if(index < mColorViews.size() && mColorViews[index])
			{
				Detach(att);
			}

			if(mColorViews.size() < index + 1 )
			{
				mColorViews.resize(index + 1);
			}

			mColorViews[index] = view;

			// Frame buffer中所有的render target的大小都一样，以最小的一个作为参考
			size_t minColorView = index;
			for(size_t i = 0; i < mColorViews.size(); ++i)
			{
				if(mColorViews[i])
				{
					minColorView = i;
					break;
				}
			}

			if(minColorView == index && view)
			{
				mWidth = view->GetWidth();
				mHeight = view->GetHeight();
				mColorFormat = view->GetFormat();
			}
		}
	}

	if (view)
		view->OnAttach(*this, att);

	mActice = true;
	mDirty = true;
}

void FrameBuffer::Detach( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		if(mDepthStencilView)
		{
			mDepthStencilView->OnDetach(*this, att);
			mDepthStencilView = nullptr;
		}

		mIsDepthBuffered = false;
		mDepthBits = 0;
		mStencilBits = 0;
		break;
	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1 )
		{
			mColorViews.resize(index + 1);
		}

		if(mColorViews[index])
		{
			mColorViews[index]->OnDetach(*this, att);
			mColorViews[index] = nullptr;
		}
	}

	mActice = true;
	mDirty = true;
}

void FrameBuffer::OnBind()
{
	// Do Render API specify set
	DoBind();
	mDirty = false;
}

void FrameBuffer::OnUnbind()
{
	// Do Render API specify set
	DoUnbind();
	mDirty =  true;
}

void FrameBuffer::DetachAll()
{
	for (size_t i = 0; i < mColorViews.size(); ++i)
	{
		if (mColorViews[i])
		{
			Detach((Attachment)(ATT_DepthStencil + i));
		}
	}
	mColorViews.clear();

	if (mDepthStencilView)
		Detach(ATT_DepthStencil);
}

void FrameBuffer::Clear( uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// frame buffer must binded before clear
	shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	assert( this == currentFrameBuffer.get());

	if (flags & CF_Color)
	{
		for (size_t i = 0; i < mColorViews.size(); ++i)
		{
			if (mColorViews[i])
			{
				mColorViews[i]->ClearColor(clr);
			}		
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

bool FrameBuffer::CheckFramebufferStatus()
{
	return true;
}


} // Namespace RcEngine