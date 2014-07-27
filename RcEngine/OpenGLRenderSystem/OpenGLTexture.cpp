#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture::OpenGLTexture( TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint, flags), 
	  mPixelBufferID(0),
	  mTextureOGL(0),
	  mTextureTarget(0)
{
	mTextureArraySize = (std::max)(arraySize, 1U);

	if ( (mTextureArraySize > 1) && !GLEW_EXT_texture_array)
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Array is not supported!", "OpenGLTexture::OpenGLTexture");
}

OpenGLTexture::~OpenGLTexture(void)
{
	if (mPixelBufferID > 0)
		glDeleteBuffers(1, &mPixelBufferID);

	glDeleteRenderbuffers(1, &mTextureOGL);	
}

void* OpenGLTexture::Map1D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map1D");
}

void* OpenGLTexture::Map2D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map2D");
}

void* OpenGLTexture::Map3D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::Map3D");
}

void* OpenGLTexture::MapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::MapCube");
}

void OpenGLTexture::Unmap1D(uint32_t arrayIndex,   uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap1D");
}

void OpenGLTexture::Unmap2D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap2D");
}

void OpenGLTexture::Unmap3D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::Unmap3D");
}

void OpenGLTexture::UnmapCube(uint32_t arrayIndex,   CubeMapFace face, uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::UnmapCube");
}

void OpenGLTexture::CopyToTexture( Texture& destTexture )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::CopyToTexture");
}

void OpenGLTexture::BuildMipMap()
{
	if (GLEW_EXT_framebuffer_object)
	{
		glBindTexture(mTextureTarget, mTextureOGL);
		glGenerateMipmapEXT(mTextureTarget);
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Unsupported!", "OpenGLTexture::BuildMipMap");
	}
}

}





