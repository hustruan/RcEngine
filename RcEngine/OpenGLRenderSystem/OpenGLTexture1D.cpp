#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture1D::OpenGLTexture1D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: OpenGLTexture(TT_Texture1D, format, arraySize, numMipMaps, 1, 0, accessHint, flags)
{
	mTextureTarget = (mTextureArraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;

	// numMipMap == 0, will generate mipmap levels automatically
	mMipLevels = (numMipMaps > 0) ? numMipMaps : Texture::CalculateMipmapLevels(width);
	mWidth = width;

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	assert(mSampleCount <= 1);
	glGenTextures(1, &mTextureOGL);
	glBindTexture(mTextureTarget, mTextureOGL);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);

	// Only CPU side access can use Map
	bool cpuSideAccess = (accessHint & (EAH_CPU_Read | EAH_CPU_Write)) != 0;
	if ( cpuSideAccess && GLEW_ARB_pixel_buffer_object)
		glGenBuffers(1, &mPixelBufferID);  // use PBO to map if supported!

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
	{
		for (uint32_t level = 0; level < mMipLevels; ++ level)
		{
			uint32_t levelWidth = (std::max)(1U, mWidth >> level);

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
					"OpenGLTexture1D::OpenGLTexture1D");
			}
			else
			{
				if (mTextureArraySize > 1)
				{
					if (0 == arrIndex)
						glTexImage2D(mTextureTarget, level, internalFormat, levelWidth, mTextureArraySize, 0, externFormat, formatType, NULL);

					glTexSubImage2D(mTextureTarget, level, 0, arrIndex, levelWidth, 1,
						externFormat, formatType, (NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
				else
				{
					glTexImage1D(mTextureTarget, level, internalFormat, levelWidth, 0, externFormat, formatType,
						(NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
			}
		}
	}
}

void OpenGLTexture1D::Map1D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{

}

void OpenGLTexture1D::Unmap1D( uint32_t arrayIndex, uint32_t level )
{

}

}

