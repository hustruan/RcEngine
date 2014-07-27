#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>
#include <math.h>

namespace RcEngine {

OpenGLTextureCube::OpenGLTextureCube( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: OpenGLTexture(TT_TextureCube, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags)
{
	assert(height == width);
	mMipLevels = (numMipMaps > 0) ? numMipMaps : Texture::CalculateMipmapLevels(width);
	mWidth = mHeight = width;

	mTextureTarget = (mTextureArraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
	glGenTextures(1, &mTextureOGL);
	glBindTexture(mTextureTarget, mTextureOGL);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);

	//if (GLEW_ARB_texture_storage)
	//	CreateWithImmutableStorage(initData);
	//else
	CreateWithMutableStorage(initData);
}


void OpenGLTextureCube::CreateWithImmutableStorage( ElementInitData* initData )
{
	/*GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	if (mTextureArraySize > 1)
	{
		glTexStorage3D(mTextureTarget, mMipMaps, internalFormat, mSizes[0], mSizes[0], mTextureArraySize);
		if (initData)
		{
			for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for(uint32_t face = 0; face < 6; ++face)
				{
					for (uint32_t level = 0; level < mMipMaps; ++ level)
					{
						uint32_t levelWidth = mSizes[level];
						uint32_t levelHeight = mSizes[level];
						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
							uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
							glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
								static_cast<GLint>(level),
								0, 0, static_cast<GLint>(arrIndex),
								static_cast<GLsizei>(levelWidth),
								static_cast<GLsizei>(levelHeight),
								static_cast<GLsizei>(0),
								externFormat, 
								static_cast<GLsizei>(imageSize),
								initData[arrIndex * mMipMaps + level].pData);
						}
						else
						{
							glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
								static_cast<GLint>(level),
								0, 0, static_cast<GLint>(arrIndex),
								static_cast<GLsizei>(levelWidth),
								static_cast<GLsizei>(levelHeight),
								static_cast<GLsizei>(1),
								externFormat,
								formatType,
								initData[arrIndex * mMipMaps + level].pData);
						}
					}
				}
			}
		}
	}
	else
	{

	}*/
}

void OpenGLTextureCube::CreateWithMutableStorage( ElementInitData* initData )
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++arrIndex)
	{
		for(uint32_t face = 0; face < 6; ++face)
		{
			for (uint32_t level = 0; level < mMipLevels; ++level)
			{
				uint32_t levelSize= (std::max)(1U, mWidth >> level);

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelSize+3)/4)*((levelSize+3)/4)*blockSize; 

					uint32_t imageIndex =  arrIndex*mMipLevels*6 + face*mMipLevels + level;

					if (mTextureArraySize > 1)
					{	
						if (0 == arrIndex)
						{
							glCompressedTexImage3D(mTextureTarget, level, internalFormat, levelSize, levelSize, mTextureArraySize,
								0, imageSize, NULL);
						}
						glCompressedTexSubImage3D(mTextureTarget, level, 0, 0, arrIndex, levelSize, levelSize, 1, internalFormat, 
							imageSize, (NULL == initData) ? NULL : initData[imageIndex].pData);

					}
					else
					{

						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, internalFormat, levelSize, levelSize, 0,
							imageSize, (NULL == initData) ? NULL : initData[imageIndex].pData);
					}


				}
				else
				{
					uint32_t imageSize = levelSize * levelSize * texelSize;

					uint32_t imageIndex =  arrIndex * mMipLevels * 6 + face * mMipLevels + level;

					if(mTextureArraySize > 1)
					{
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported CubeMap Texture Array",
							"OpenGLTextureCube::OpenGLTextureCube");
					}
					else
					{
						ElementInitData id = initData[imageIndex];
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, internalFormat, levelSize, levelSize, 0, externFormat, formatType,
							(NULL == initData) ? NULL : initData[imageIndex].pData);
					}
				}	
			}
		}
	}
}

//void OpenGLTextureCube::MapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
//{
//
//}
//
//void OpenGLTextureCube::UnmapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level )
//{
//
//}

}

