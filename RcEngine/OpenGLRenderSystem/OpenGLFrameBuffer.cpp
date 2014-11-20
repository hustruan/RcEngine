#include "OpenGLFrameBuffer.h"
#include "OpenGLDevice.h"
#include "OpenGLView.h"
#include <Graphics/RenderState.h>
#include <Core/Exception.h>
//#include <Core/Context.h>

namespace RcEngine {

OpenGLFrameBuffer::OpenGLFrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: FrameBuffer(width, height)
{
	if (offscreen)
		glGenFramebuffers(1, &mFrameBufferOGL);
	else
		mFrameBufferOGL = 0;
}
OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	DetachAll();

	if(mFrameBufferOGL != 0)
		glDeleteFramebuffers(1, &mFrameBufferOGL);
}

void OpenGLFrameBuffer::OnBind()
{
	gOpenGLDevice->BindFBO(mFrameBufferOGL);

	if (mFrameBufferOGL == 0)
	{
		GLenum targets[] = { GL_BACK };
		glDrawBuffers(1, &targets[0]);
	}
	else
	{
		if (mFrameBufferOGL != 0 && mColorViews.size())
		{
			assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

			std::vector<GLenum> targets(mColorViews.size());
			for (size_t i = 0; i < mColorViews.size(); ++ i)
			{
				if (mColorViews[i])
					targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
			}
			glDrawBuffers(static_cast<GLsizei>(targets.size()), &targets[0]);
		}
		
		if (mUnorderedAccessViews.size())
		{
			for (size_t i = 0; i < mUnorderedAccessViews.size(); ++i)
			{
				if (mUnorderedAccessViews[i])
				{
					OpenGLUnorderedAccessView* pOpenGLUAV = static_cast_checked<OpenGLUnorderedAccessView*>(mUnorderedAccessViews[i].get());
					pOpenGLUAV->BindUAV(i);
				}
			}
		}
	}

	// Set view ports
	gOpenGLDevice->SetViewports(mViewports);

	OGL_ERROR_CHECK();
}

void OpenGLFrameBuffer::OnUnbind()
{

}

void OpenGLFrameBuffer::SwapBuffers()
{

}

bool OpenGLFrameBuffer::CheckFramebufferStatus()
{
	OGL_ERROR_CHECK();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status == GL_FRAMEBUFFER_COMPLETE)
		return true;

	String errStr;
	switch (status)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		errStr = "The default framebuffer does not exist";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		errStr = "Combination of internal formats of the attached images violates an implementation-dependent set of restrictions";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		errStr = "Any of the framebuffer attachment points are framebuffer incomplete";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		errStr = "The framebuffer does not have at least one image attached to it";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :
		errStr = "Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target";
		break;
	default:
		break;
	}

	OGL_ERROR_CHECK();

	std::cout << errStr << std::endl;
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "OpenGL FrameBuffer Error", "OpenGLFrameBuffer::CheckFramebufferStatus");
	return false;
}

//////////////////////////////////////////////////////////////////////////
OpenGLRenderView::OpenGLRenderView(const shared_ptr<Texture>& texture)
	: RenderView(texture)
{
}

OpenGLRenderView::~OpenGLRenderView(void)
{
}

void OpenGLRenderView::ClearColor( const ColorRGBA& clearColor )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearColor");
}

void OpenGLRenderView::ClearDepth( float depth )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearDepth");
}

void OpenGLRenderView::ClearStencil( uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearStencil");
}

void OpenGLRenderView::ClearDepthStencil( float depth, uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearDepthStencil");
}

void OpenGLRenderView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	mFrameBufferOGL = (static_cast_checked<OpenGLFrameBuffer*>(&fb))->GetFrameBufferOGL();
	mAttachment = attr;
}

void OpenGLRenderView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(mAttachment == attr);
}

void OpenGLRenderView::DoClear( GLbitfield clearFlagOGL, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	/*shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	OpenGLFrameBuffer& frameBufferOGL= *static_cast_checked<OpenGLFrameBuffer*>(currentFrameBuffer.get());
	assert(mFrameBufferOGL == frameBufferOGL.GetFrameBufferOGL());*/

	assert(mFrameBufferOGL == gOpenGLDevice->GetCurrentFBO());

	const DepthStencilStateDesc& currDepthStencilDesc = gOpenGLDevice->GetCurrentDepthStencilState()->GetDesc();
	const BlendStateDesc& currBlendDesc = gOpenGLDevice->GetCurrentBlendState()->GetDesc();

	// mark all clear channel write mask true ,so we can clear it
	if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2 && mFrameBufferOGL != 0)
		{
			// separate render target blend enables and color write masks supported
			int32_t bufferIdx = mAttachment - ATT_Color0;
			if (currBlendDesc.RenderTarget[bufferIdx].ColorWriteMask != CWM_All)
				glColorMaskIndexedEXT(bufferIdx, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
		else
		{
			if (currBlendDesc.RenderTarget[0].ColorWriteMask != CWM_All)
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.DepthWriteMask)
			glDepthMask(GL_TRUE);
	}

	if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.StencilWriteMask)
			glStencilMask(GL_TRUE);
	}

	// do the clear
	if (GLEW_EXT_draw_buffers2 && mFrameBufferOGL != 0)
	{
		if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
		{
			GLint  bufferIndex = mAttachment - ATT_Color0;
			glClearBufferfv(GL_COLOR, bufferIndex, clr());
		}

		if ((clearFlagOGL & GL_DEPTH_BUFFER_BIT) && (clearFlagOGL & GL_STENCIL_BUFFER_BIT))
			glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
		else
		{
			if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
				glClearBufferfv(GL_DEPTH, 0, &depth);
			else
			{
				if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
				{
					GLint s = stencil;
					glClearBufferiv(GL_STENCIL, 0, &s);
				}
			}
		}
	}
	else
	{
		if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
			glClearColor(clr.R(), clr.G(), clr.B(), clr.A());

		if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
			glClearDepth(depth);

		if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
			glClearStencil(stencil);

		if (clearFlagOGL != 0)
			glClear(clearFlagOGL);
	}

	// set mask back
	if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2 && mFrameBufferOGL != 0)
		{
			// separate render target blend enables and color write masks supported
			int32_t bufferIdx = mAttachment - ATT_Color0;
			uint32_t writeMask = currBlendDesc.RenderTarget[bufferIdx].ColorWriteMask;
			glColorMaskIndexedEXT(bufferIdx, (writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
				(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
		}
		else
		{
			uint32_t writeMask = currBlendDesc.RenderTarget[0].ColorWriteMask;
			glColorMask((writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
				(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
		}
	}

	if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
	{
		if(!currDepthStencilDesc.DepthWriteMask)
			glDepthMask(GL_FALSE);
	}

	if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
		glStencilMask(currDepthStencilDesc.StencilWriteMask);

	OGL_ERROR_CHECK();
}


}


