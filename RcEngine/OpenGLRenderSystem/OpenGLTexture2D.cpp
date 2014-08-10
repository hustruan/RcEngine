#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLView.h"
#include "OpenGLDevice.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags)
{
	mWidth = width;
	mHeight = height;

	// Generate mipmaps if enable
	if (mCreateFlags & TexCreate_GenerateMipmaps)
	{
		// numMipMap == 0, will generate mipmap levels automatically
		assert(numMipMaps == 0);
		mMipLevels = Texture::CalculateMipmapLevels((std::max)(width, height));
	}
	else
	{
		mMipLevels = numMipMaps;
	}

	// OpenGL Texture target type
	if (sampleCount <= 1)
		mTextureTarget = (mTextureArraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
	else
	{
		assert(mMipLevels <= 1);
		mTextureTarget =  (mTextureArraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
	}
	
	glGenTextures(1, &mTextureOGL);
	glBindTexture(mTextureTarget, mTextureOGL);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);

	// Use texture storage to init, faster
	if (GLEW_ARB_texture_storage)
		CreateWithImmutableStorage(initData);
	else
		CreateWithMutableStorage(initData);

	if (mCreateFlags & TexCreate_ShaderResource)
	{
		mTextureSRV = std::make_shared<OpenGLTextureSRV>(mTextureOGL, mTextureTarget);
	}
}

void OpenGLTexture2D::CreateWithImmutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	if (mTextureArraySize > 1)
	{
		glTexStorage3D(mTextureTarget, mMipLevels, internalFormat, mWidth, mHeight, mTextureArraySize);
		if (initData)
		{
			for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for (uint32_t level = 0; level < mMipLevels; ++ level)
				{
					uint32_t levelWidth = (std::max)(1U, mWidth >> level);
					uint32_t levelHeight = (std::max)(1U, mHeight >> level);

					if (PixelFormatUtils::IsCompressed(mFormat))
					{
						int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
						uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
						
						glCompressedTexSubImage3D(mTextureTarget,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(0),
							externFormat, 
							static_cast<GLsizei>(imageSize),
							initData[arrIndex * mMipLevels + level].pData);
					}
					else
					{
						glTexSubImage3D(mTextureTarget,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(1),
							externFormat,
							formatType,
							initData[arrIndex * mMipLevels + level].pData);
					}
				}
			}
		}
	}
	else
	{
		glTexStorage2D(mTextureTarget, mMipLevels, internalFormat, mWidth, mHeight);
		if (initData)
		{
			for (uint32_t level = 0; level < mMipLevels; ++ level)
			{
				uint32_t levelWidth = (std::max)(1U, mWidth >> level);
				uint32_t levelHeight = (std::max)(1U, mHeight >> level);

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

					glCompressedTexSubImage2D(mTextureTarget,
						static_cast<GLint>(level), 0, 0,
						static_cast<GLsizei>(levelWidth),
						static_cast<GLsizei>(levelHeight),
						externFormat,
						static_cast<GLsizei>(imageSize),
						initData[level].pData);

					OGL_ERROR_CHECK();
				}
				else
				{
					glTexSubImage2D(mTextureTarget,
						static_cast<GLint>(level), 0, 0,
						static_cast<GLsizei>(levelWidth),
						static_cast<GLsizei>(levelHeight),
						externFormat,
						formatType,
						initData[level].pData);
				}
			}
		}
	}

	OGL_ERROR_CHECK();
}

void OpenGLTexture2D::CreateWithMutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
	{
		for (uint32_t level = 0; level < mMipLevels; ++ level)
		{
			uint32_t levelWidth = (std::max)(1U, mWidth >> level);
			uint32_t levelHeight = (std::max)(1U, mHeight >> level);

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				// Need check
				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

				if (mTextureArraySize > 1)
				{	
					if (0 == arrIndex)
					{
						glCompressedTexImage3D(mTextureTarget, level, internalFormat, levelWidth, levelHeight, 
							mTextureArraySize, 0, imageSize, NULL);
					}

					glCompressedTexSubImage3D(mTextureTarget, level, 0, 0, arrIndex, levelWidth, levelHeight, 1, internalFormat, 
							imageSize, initData[arrIndex * mMipLevels + level].pData);
				}
				else
				{
					glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, levelWidth, levelHeight, 0,
						imageSize, (NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
			}
			else
			{
				if (mTextureArraySize > 1)
				{
					if (0 == arrIndex)
					{
						glTexImage3D(mTextureTarget, level, internalFormat, levelWidth, levelHeight,
							mTextureArraySize, 0, externFormat, formatType, NULL);
					}

					// OpenGL bugs. init texture array mipmaps with NULL storage cause crash.
					if (initData)
					{
						glTexSubImage3D(mTextureTarget, level, 0, 0, arrIndex, levelWidth, levelHeight, 1,
							externFormat, formatType, (NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
					}
					else
					{
						if (mMipLevels > 1)
							glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
					}
					
				}
				else
				{
					glTexImage2D(mTextureTarget, level, internalFormat, levelWidth, levelHeight, 0, externFormat, formatType,
						(NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
			}
		}
	}
}

void* OpenGLTexture2D::Map2D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	// Not work for texture array
	assert( mTextureArraySize == 1 );

	void* pMappedData;
	mTextureMapAccess = mapType;

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	
	uint32_t imageSize;
	uint32_t levelWidth = CalculateLevelSize(mWidth, level);
	uint32_t levelHeight = CalculateLevelSize(mHeight, level);

	// Compute image size
	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		uint32_t blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
		imageSize = ((levelWidth+3)/4)*((levelWidth+3)/4)*blockSize; 	
	}
	else
	{
		uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);
		imageSize = levelWidth * levelHeight * texelSize;
	}

	rowPitch = imageSize / levelHeight;

	GLbitfield mapUsage = OpenGLMapping::Mapping(mapType);
	GLenum bufferUsage = (mapType == RMA_Read_Only) ? GL_STREAM_READ : GL_STREAM_DRAW;

	assert(GLEW_ARB_pixel_buffer_object);
	if (!mPixelBufferID)
		glGenBuffers(1, &mPixelBufferID);

	switch(mapType)
	{
	case RMA_Read_Only:
	case RMA_Read_Write:
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBufferID);
			glBufferData(GL_PIXEL_PACK_BUFFER, imageSize, NULL, bufferUsage);
			
			glBindTexture(mTextureTarget, mTextureOGL);
			if (PixelFormatUtils::IsCompressed(mFormat))
				glGetCompressedTexImage(mTextureTarget, level, NULL);
			else
				glGetTexImage(mTextureTarget, level, externFormat, formatType, NULL);

			pMappedData =  glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, imageSize, mapUsage);
		}
		break;
	case RMA_Write_Only:
	case RMA_Write_Discard:
	case RMA_Write_No_Overwrite:
		{
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mPixelBufferID);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, imageSize, NULL, GL_STREAM_DRAW);

			pMappedData =  glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, imageSize, mapUsage);
		}
		break;
	default:
		assert(false);
	}

	return pMappedData;
}

void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
{
	uint32_t levelWidth = CalculateLevelSize(mWidth, level);
	uint32_t levelHeight = CalculateLevelSize(mHeight, level);

	assert(mPixelBufferID != 0);
	switch(mTextureMapAccess)
	{
	case RMA_Read_Only:
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBufferID);
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}
		break;

	case RMA_Write_Only:
	case RMA_Read_Write:
		{
			GLenum internalFormat, externFormat, formatType;
			OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
			uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);
			
			uint32_t imageSize = 0;
			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
			}

			glBindTexture(mTextureTarget, mTextureOGL);

			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mPixelBufferID);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				if (mTextureArraySize > 1)
					glCompressedTexSubImage3D(mTextureTarget, level, 0, 0, arrayIndex,levelWidth, levelHeight, 1, externFormat, imageSize, NULL);
				else
					glCompressedTexSubImage2D(mTextureTarget, level, 0, 0, levelWidth, levelHeight, externFormat, imageSize, NULL);
			}
			else
			{
				if (mTextureArraySize > 1)
					glTexSubImage3D(mTextureTarget, level, 0, 0, arrayIndex, levelWidth, levelHeight, 1, externFormat, formatType, NULL);
				else
					glTexSubImage2D(mTextureTarget, level, 0, 0, levelWidth, levelHeight, externFormat, formatType, NULL);
			}
		}
	}
}

void OpenGLTexture2D::CopyToTexture( Texture& destTexture )
{
	assert(mFormat == destTexture.GetTextureFormat() && mType == destTexture.GetTextureType());
	OpenGLTexture2D& destTextureOGL = *(static_cast<OpenGLTexture2D*>(&destTexture));

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++arrIndex)
	{
		for (uint32_t level = 0; level < mMipLevels; ++level)
		{
			uint32_t levelWidth = CalculateLevelSize(mWidth, level);
			uint32_t levelHeight = CalculateLevelSize(mHeight, level);

			GLuint srtTex = mTextureOGL;
			GLuint dstTex = destTextureOGL.mTextureOGL;

			GLenum srcTarget = mTextureTarget;
			GLenum dstTarget = destTextureOGL.mTextureTarget;

			if (GLEW_ARB_copy_image)
			{
				glCopyImageSubData(srtTex, srcTarget, level, 0, 0, arrIndex,
					dstTex, dstTarget, level, 0, 0, arrIndex, 
					levelWidth, levelHeight, 1);
			}
			else
			{
				GLuint oldFBO = gOpenGLDevice->GetCurrentFBO();
				{
					GLuint srcFBO, dstFBO;
					gOpenGLDevice->GetBlitFBO(srcFBO, dstFBO);

					GLenum attachment, bufferBit;
					bool depth = PixelFormatUtils::IsDepth(GetTextureFormat());
					bool stencil = PixelFormatUtils::IsStencil(GetTextureFormat());
					if (depth || stencil)
					{
						if (depth && stencil)
						{
							attachment = GL_DEPTH_STENCIL_ATTACHMENT;
							bufferBit = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
						}
						else if (depth)
						{
							attachment = GL_DEPTH_ATTACHMENT;
							bufferBit = GL_DEPTH_BUFFER_BIT;
						}
						else if (stencil)
						{
							attachment = GL_STENCIL_ATTACHMENT;
							bufferBit = GL_STENCIL_BUFFER_BIT;
						}
					}
					else 
					{
						attachment = GL_COLOR_ATTACHMENT0;
						bufferBit = GL_COLOR_BUFFER_BIT;
					}

					glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
					if (mTextureArraySize > 1)
						glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, attachment, srtTex, level, arrIndex);
					else 
						glFramebufferTexture2D(GL_READ_FRAMEBUFFER, attachment, GL_TEXTURE_2D, srtTex, level);

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);
					if (mTextureArraySize > 1)
						glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attachment, dstTex, level, arrIndex);
					else 
						glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, dstTex, level);

					glBlitFramebuffer(0, 0, levelWidth, levelHeight, 0, 0, levelWidth, levelHeight, bufferBit, GL_NEAREST);
				}
				gOpenGLDevice->BindFBO(oldFBO);
			}
		}
	}

	OGL_ERROR_CHECK();
}



}


