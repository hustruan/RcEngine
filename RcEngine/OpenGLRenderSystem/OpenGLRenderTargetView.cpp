#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLDevice.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLRenderTargetView2D::OpenGLRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level)
	: OpenGLRenderView(texture),
	  mArrIndex(arrIndex),
	  mLevel(level)
{

}

void OpenGLRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnAttach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnAttach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(mTexture.get());

	//if (GLEW_EXT_direct_state_access)
	//{
	//	if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
	//	{
	//		//glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel);
	//		glNamedFramebufferTexture2DEXT(mFrameBufferOGL, attachment, GL_TEXTURE_2D, pTextureOGL->GetTextureOGL(), mLevel);
	//	}
	//	else 
	//	{
	//		glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
	//	}
	//}
	//else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
			{
				//glFramebufferTexture(GL_FRAMEBUFFER, attachment, pTextureOGL->GetTextureOGL(), mLevel);
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, pTextureOGL->GetTextureOGL(), mLevel);
			}
			else 
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
			}
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnDetach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);
	//if (GLEW_EXT_direct_state_access)
	//{
	//	if (mTexture->GetTextureArraySize() <= 1)
	//	{
	//		//glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, 0, 0);
	//		glNamedFramebufferTexture2DEXT(mFrameBufferOGL, attachment, GL_TEXTURE_2D, 0, 0);
	//	}
	//	else 
	//	{
	//		glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, attachment, 0, 0, 0);
	//	}
	//}
	//else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			if (mTexture->GetTextureArraySize() <= 1)
			{
				//glFramebufferTexture(GL_FRAMEBUFFER, attachment, 0, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, 0, 0, 0);
			}
			else 
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, 0, 0, 0);
			}
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

//////////////////////////////////////////////////////////////////////////
OpenGLRenderTargetArrayView::OpenGLRenderTargetArrayView( const shared_ptr<Texture>& texture, uint32_t level )
	: OpenGLRenderView(texture),
	  mLevel(level)
{

}

void OpenGLRenderTargetArrayView::OnAttach( FrameBuffer& fb, Attachment attr )
{
	OpenGLRenderView::OnAttach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(mTexture.get());

	if (GLEW_EXT_direct_state_access)
	{
		glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel);
	}
	else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, pTextureOGL->GetTextureOGL(), mLevel);
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetArrayView::OnDetach( FrameBuffer& fb, Attachment attr )
{
	OpenGLRenderView::OnDetach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);
	if (GLEW_EXT_direct_state_access)
	{
		glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, 0, 0);
	}
	else
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();

		gOpenGLDevice->BindFBO(mFrameBufferOGL);
		{
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, 0, 0);
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

//////////////////////////////////////////////////////////////////////////
OpenGLScreenRenderTargetView2D::OpenGLScreenRenderTargetView2D()
	: OpenGLRenderView(nullptr)
{

}

void OpenGLScreenRenderTargetView2D::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnAttach(fb, attr);

	if(mFrameBufferOGL != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenRenderTargetView2D::OnAttach");
	}
}

void OpenGLScreenRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnDetach(fb, attr);
}

void OpenGLScreenRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
}

}
