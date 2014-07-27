#include "OpenGLView.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLDevice.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTextureBufferSRV::OpenGLTextureBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format )
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum internalFormat, externFormat, type;
	OpenGLMapping::Mapping(internalFormat, externFormat, type, format);

	mTargetOGL = GL_TEXTURE_BUFFER;

	// offset and size
	uint32_t elementStride = PixelFormatUtils::GetNumElemBytes(format);
	GLintptr bufferOffset = elementStride * elementOffset;
	GLsizeiptr bufferSize = elementStride * elementWidth ;

	glGenTextures(1, &mResourceOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mResourceOGL);
	glTexBufferRange(GL_TEXTURE_BUFFER, internalFormat, pTBO->GetBufferOGL(), bufferOffset, bufferSize);
}

OpenGLTextureBufferSRV::~OpenGLTextureBufferSRV()
{
	glDeleteTextures(1, &mResourceOGL);
}

void OpenGLTextureBufferSRV::BindSRV( GLuint textureUnit )
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(mTargetOGL, mResourceOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferSRV::OpenGLStructuredBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);
	
	mTargetOGL = GL_SHADER_STORAGE_BUFFER;
	mResourceOGL = pSSBO->GetBufferOGL();
	
	mBufferOffset = elementOffset * strutureStride;
	mBufferSize = elementWidth * strutureStride;
}

void OpenGLStructuredBufferSRV::BindSRV( GLuint bindingPoint )
{
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, mResourceOGL, mBufferOffset, mBufferSize);
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureSRV::OpenGLTextureSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	  : mNeedDelete(false)
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t numLevels = texture->GetMipLevels();
	uint32_t numLayers = texture->GetTextureArraySize();

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, texture->GetTextureFormat());

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(texture.get());

	if (mostDetailedMip == 0 && mipLevels == numLevels && firstArraySlice == 0 && arraySize == numLayers)
	{
		mNeedDelete = false;
		mTargetOGL = pTextureOGL->GetTextureTarget();
		mResourceOGL = pTextureOGL->GetTextureOGL();
	}
	else
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mResourceOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D:
			{
				mTargetOGL = GL_TEXTURE_1D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_1D_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D:
			{
				mTargetOGL = GL_TEXTURE_2D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			{
				mTargetOGL = GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_3D:
			{
				mTargetOGL = GL_TEXTURE_3D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP:
			{
				mTargetOGL = GL_TEXTURE_CUBE_MAP;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		default:
			assert(false);
			break;
		}
	}
}

OpenGLTextureSRV::OpenGLTextureSRV( GLuint textureOGL, GLenum targetOGL )
	: mNeedDelete(false)
{
	mTargetOGL = targetOGL;
	mResourceOGL = textureOGL;
}

OpenGLTextureSRV::~OpenGLTextureSRV()
{
	if (mNeedDelete)
		glDeleteTextures(1, &mResourceOGL);
}

void OpenGLTextureSRV::BindSRV( GLuint textureUnit )
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(mTargetOGL, mResourceOGL);
}

// UAV
//////////////////////////////////////////////////////////////////////////
void OpenGLUnorderedAccessView::Clear( const float4& clearData )
{
	ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Shoudn't Call this", "OpenGLUnorderedAccessView::Clear");
}

void OpenGLUnorderedAccessView::Clear( const uint4& clearData )
{
	ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Shoudn't Call this", "OpenGLUnorderedAccessView::Clear");
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureBufferUAV::OpenGLTextureBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format )
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum externFormat, type;
	OpenGLMapping::Mapping(mInternalFormat, externFormat, type, format);

	// offset and size
	uint32_t elementStride = PixelFormatUtils::GetNumElemBytes(format);
	GLintptr bufferOffset = elementStride * elementOffset;
	GLsizeiptr bufferSize = elementStride * elementWidth ;

	glGenTextures(1, &mResourceOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mResourceOGL);
	glTexBufferRange(GL_TEXTURE_BUFFER, mInternalFormat, pTBO->GetBufferOGL(), bufferOffset, bufferSize);
}

OpenGLTextureBufferUAV::~OpenGLTextureBufferUAV()
{
	glDeleteTextures(1, &mResourceOGL);
}

void OpenGLTextureBufferUAV::BindUAV( GLuint imageUnit )
{
	glBindImageTexture(imageUnit, mResourceOGL, 0, GL_FALSE, 0, GL_READ_WRITE, mInternalFormat);
}

////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferUAV::OpenGLStructuredBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride )
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);

	mTargetOGL = GL_SHADER_STORAGE_BUFFER;
	mResourceOGL = pSSBO->GetBufferOGL();

	// offset and size
	mBufferOffset = elementOffset * strutureStride;
	mBufferSize = elementWidth * strutureStride;
}

void OpenGLStructuredBufferUAV::BindUAV( GLuint bindingPoint )
{
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, mResourceOGL, mBufferOffset, mBufferSize);
}

////////////////////////////////////////////////////////////////////////
OpenGLTextureUAV::OpenGLTextureUAV( const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize )
{
	mLevel = (GLint)mipSlice;
	mFirstLayer = (GLint)firstArraySlice;
	mNumLayers = (GLint)arraySize;

	OpenGLMapping::Mapping(mInternalFormat, mExternalFormat, mFormatType, texture->GetTextureFormat());

	uint32_t numLayers = texture->GetTextureArraySize();
	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(texture.get());
	
	if (firstArraySlice != 0 && arraySize > 1)
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mResourceOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D_ARRAY:
			{
				glTextureView(mResourceOGL, GL_TEXTURE_1D_ARRAY, pTextureOGL->GetTextureOGL(), mInternalFormat, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				glTextureView(mResourceOGL, GL_TEXTURE_2D_ARRAY, pTextureOGL->GetTextureOGL(), mInternalFormat, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		default:
			assert(false);
			break;
		}

		// layer 0 of the new created texture view
		mFirstLayer = 0;
	}
	else
	{
		mResourceOGL = pTextureOGL->GetTextureOGL();
		mNeedDelete = false;
	}
}

OpenGLTextureUAV::~OpenGLTextureUAV()
{
	if (mNeedDelete)
		glDeleteTextures(1, &mResourceOGL);
}

void OpenGLTextureUAV::BindUAV( GLuint imageUnit )
{
	GLboolean layered = (mNumLayers > 1) ? GL_TRUE : GL_FALSE;
	glBindImageTexture(imageUnit, mResourceOGL, mLevel, layered, mFirstLayer, GL_READ_WRITE, mInternalFormat);
}

void OpenGLTextureUAV::Clear( const float4& clearData )
{
	if (GLEW_ARB_clear_texture)
	{
		glClearTexImage(mResourceOGL, mLevel, mExternalFormat, mFormatType, &clearData);
	}
	else 
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();
		{
			GLuint fbo, dummy;
			gOpenGLDevice->GetBlitFBO(fbo, dummy);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mResourceOGL, 0); //Only need to do this once.
			glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
			glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clearData);
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}
}

void OpenGLTextureUAV::Clear( const uint4& clearData )
{
	if (GLEW_ARB_clear_texture)
	{
		glClearTexImage(mResourceOGL, mLevel, mExternalFormat, mFormatType, &clearData);
	}
	else 
	{
		GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();
		{
			GLuint fbo, dummy;
			gOpenGLDevice->GetBlitFBO(fbo, dummy);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mResourceOGL, 0); //Only need to do this once.
			glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
			glClearBufferuiv(GL_COLOR, 0, (GLuint*)&clearData);
		}
		gOpenGLDevice->BindFBO(oldFBO);
	}
}

}