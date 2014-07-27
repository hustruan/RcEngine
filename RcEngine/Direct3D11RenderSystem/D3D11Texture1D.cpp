#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11Texture1D::D3D11Texture1D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: D3D11Texture(TT_Texture1D, format, arraySize, numMipMaps, 1, 0, accessHint, flags),
	  TextureD3D11(nullptr)
{
	mWidth = width;

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.ArraySize = mTextureArraySize;
	texDesc.Format = D3D11Mapping::Mapping(format);
	
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	D3D11Mapping::Mapping(accessHint, texDesc.Usage, texDesc.CPUAccessFlags);

	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	
	// Generate mipmaps if enable
	if (mCreateFlags & TexCreate_GenerateMipmaps)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		mMipLevels = Texture::CalculateMipmapLevels(mWidth);
	}
	else
		mMipLevels = numMipMaps;

	if (mCreateFlags & TexCreate_UAV)
		texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	if (mCreateFlags & TexCreate_RenderTarget)
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

	if (initData)
	{
		uint32_t numSubResource = mTextureArraySize * numMipMaps;
		vector<D3D11_SUBRESOURCE_DATA> subResourceData(numSubResource);

		for (uint32_t arrayIdx = 0; arrayIdx < mTextureArraySize; ++arrayIdx)
		{
			for (uint32_t level = 0; level < numMipMaps; ++level)
			{
				uint32_t index = arrayIdx*numMipMaps+level;
				subResourceData[index].pSysMem = initData[index].pData;
			}
		}

		D3D11_VERRY(gD3D11Device->DeviceD3D11->CreateTexture1D( &texDesc, &subResourceData[0], &TextureD3D11));
	}
	else 
		D3D11_VERRY(gD3D11Device->DeviceD3D11->CreateTexture1D( &texDesc, NULL, &TextureD3D11));


	/*if (mCreateFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = texDesc.Format;

		if (mTextureArraySize <= 1)
		{
			viewDesc.Texture1D.MostDetailedMip = 0;
			viewDesc.Texture1D.MipLevels = numMipMaps;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		}



		else
		{
			viewDesc.Texture1DArray.FirstArraySlice = 0;
			viewDesc.Texture1DArray.ArraySize = mTextureArraySize;
			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.MipLevels = numMipMaps;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		}
	
		D3D11_VERRY(pd3dDevice->CreateShaderResourceView(TextureD3D11, &viewDesc, &ShaderResourceViewD3D11));
	}*/
}

D3D11Texture1D::~D3D11Texture1D()
{
	SAFE_RELEASE(TextureD3D11);
}

void* D3D11Texture1D::Map1D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType )
{
	return 0;
}

void D3D11Texture1D::Unmap1D( uint32_t arrayIndex, uint32_t level )
{

}

}
