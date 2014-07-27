#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture3D::OpenGLTexture3D( PixelFormat format, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t depth, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: OpenGLTexture(TT_Texture3D, format, 1, numMipMaps, 1, 0, accessHint, flags)	  
{
	mTextureTarget = GL_TEXTURE_3D;
	mWidth = width; 
	mHeight = height;
	mDepth = depth;

	mMipLevels = (numMipMaps > 0) ? numMipMaps : Texture::CalculateMipmapLevels((std::max)(width, (std::max)(height, depth)));

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	glGenTextures(1, &mTextureOGL);
	glBindTexture(mTextureTarget, mTextureOGL);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);

	for (uint32_t level = 0; level < mMipLevels; ++ level)
	{
		uint32_t levelWidth = (std::max)(1U, mWidth >> level);
		uint32_t levelHeight = (std::max)(1U, mHeight >> level);
		uint32_t levelDepth = (std::max)(1U, mDepth >> level);

		if (PixelFormatUtils::IsCompressed(mFormat))
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
				"OpenGLTexture1D::OpenGLTexture1D");
		}
		else
		{
			glTexImage3D(mTextureTarget, level, internalFormat, levelWidth, levelHeight, levelDepth, 0,
				externFormat, formatType, (NULL == initData) ? NULL : initData[level].pData);

		}
	}
}


//void OpenGLTexture3D::Map3D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
//{
//	
//}
//
//void OpenGLTexture3D::Unmap3D( uint32_t arrayIndex, uint32_t level )
//{
//
//}

}

