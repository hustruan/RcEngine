#include "D3D11Texture.h"
#include "D3D11Device.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Texture::D3D11Texture( TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint, flags),
	  mStagingTextureD3D11(nullptr)
{
	mTextureArraySize = (std::max)(arraySize, 1U);
}


void D3D11Texture::BuildMipMap()
{
	SAFE_RELEASE(mStagingTextureD3D11);
}

void* D3D11Texture::Map1D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map1D");
}

void* D3D11Texture::Map2D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map2D");
}

void* D3D11Texture::Map3D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::Map3D");
}

void* D3D11Texture::MapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::MapCube");
}

void D3D11Texture::Unmap1D(uint32_t arrayIndex,   uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap1D");
}

void D3D11Texture::Unmap2D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap2D");
}

void D3D11Texture::Unmap3D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::Unmap3D");
}

void D3D11Texture::UnmapCube(uint32_t arrayIndex,   CubeMapFace face, uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Shoudn't be here!", "OpenGLTexture::UnmapCube");
}

void D3D11Texture::CopyToTexture( Texture& destTexture )
{
	ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;
	
	ID3D11Resource* destResourceD3D11;
	ID3D11Resource* srcResourceD3D11;

	assert(mFormat == destTexture.GetTextureFormat() && mTextureArraySize == destTexture.GetTextureArraySize() && 
		  mMipLevels == destTexture.GetMipLevels());

	switch (destTexture.GetTextureType())
	{
	case TT_Texture1D:
		{
			assert(mType == TT_Texture1D && mWidth == destTexture.GetWidth());
			destResourceD3D11 = static_cast_checked<D3D11Texture1D*>(&destTexture)->TextureD3D11;
			srcResourceD3D11 = static_cast_checked<D3D11Texture1D*>(this)->TextureD3D11;
		}
		break;
	case TT_Texture2D:
		{
			assert(mType == TT_Texture2D && mWidth == destTexture.GetWidth() && mHeight == destTexture.GetHeight());
			destResourceD3D11 = static_cast_checked<D3D11Texture2D*>(&destTexture)->TextureD3D11;
			srcResourceD3D11 = static_cast_checked<D3D11Texture2D*>(this)->TextureD3D11;
		}
		break;
	default:
		ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Unsupported texture type copy!", "D3D11Texture::CopyToTexture");
	}
	
	deviceContext->CopyResource(destResourceD3D11, srcResourceD3D11);
}

}