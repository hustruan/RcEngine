#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLDevice.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLDepthStencilView::OpenGLDepthStencilView( const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level )
	: OpenGLRenderView(texture),
	  mArrIndex(arrIndex),
	  mLevel(level)
{
	assert(texture->GetTextureType() == TT_Texture2D);
	assert(PixelFormatUtils::IsDepth(texture->GetTextureFormat()));
}


void OpenGLDepthStencilView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnAttach(fb, attr);

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(mTexture.get());

	GLenum attachment = GL_DEPTH_ATTACHMENT;
	if (PixelFormatUtils::IsStencil(pTextureOGL->GetTextureFormat()))
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;

	if (GLEW_EXT_direct_state_access)
	{
		if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
			glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel);
		else 
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::Attach");
			glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
		}
	}
	else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
				glFramebufferTexture(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel);
			else 
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::Attach");
				glFramebufferTextureLayer(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
			}
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLDepthStencilView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnDetach(fb, attr);

	if (GLEW_EXT_direct_state_access)
	{
		if (mTexture->GetTextureArraySize() <= 1)
			glNamedFramebufferTextureEXT(mFrameBufferOGL, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
		else 
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::OnDetach");
			glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0, 0);
		}
	}
	else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			if (mTexture->GetTextureArraySize() <= 1)
				glFramebufferTexture(mFrameBufferOGL, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
			else 
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::OnDetach");
				glFramebufferTextureLayer(mFrameBufferOGL, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0, 0);
			}
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLDepthStencilView::ClearDepth(float depth)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), depth, 0);
}

void OpenGLDepthStencilView::ClearStencil(uint32_t stencil)
{
	DoClear(GL_STENCIL_BUFFER_BIT, ColorRGBA(), 0, stencil);
}

void OpenGLDepthStencilView::ClearDepthStencil(float depth, uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, ColorRGBA(), depth, stencil);
}

//////////////////////////////////////////////////////////////////////////
OpenGLScreenDepthStencilView::OpenGLScreenDepthStencilView( PixelFormat fmt )
	: OpenGLRenderView(nullptr),
	  mFormat(fmt)
{

}

void OpenGLScreenDepthStencilView::OnAttach( FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnAttach(fb, attr);

	if(mFrameBufferOGL != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnAttach");
	}
}

void OpenGLScreenDepthStencilView::OnDetach( FrameBuffer& fb, Attachment attr )
{
	OpenGLRenderView::OnDetach(fb, attr);
	
	if(mFrameBufferOGL != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnDetached");
	}
}

void OpenGLScreenDepthStencilView::ClearDepth(float depth)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), depth, 0);
}

void OpenGLScreenDepthStencilView::ClearStencil(uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), 0, stencil);
}

void OpenGLScreenDepthStencilView::ClearDepthStencil(float depth, uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, ColorRGBA(), depth, stencil);
}

}