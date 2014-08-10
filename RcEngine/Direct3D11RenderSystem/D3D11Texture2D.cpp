#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"
#include "D3D11View.h"

namespace RcEngine {

D3D11Texture2D::D3D11Texture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
							   uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint,
							   uint32_t flags, ElementInitData* initData )
	: D3D11Texture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags),
	  TextureD3D11(nullptr)
{
	mWidth = width;
	mHeight = height;
	
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.ArraySize = arraySize;
	texDesc.SampleDesc.Quality = sampleQuality;
	texDesc.SampleDesc.Count = sampleCount;
	D3D11Mapping::Mapping(accessHint, texDesc.Usage, texDesc.CPUAccessFlags);

	if (mCreateFlags & TexCreate_DepthStencilTarget)
	{
		texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = D3D11Mapping::GetDepthTextureFormat(format);
	}
	else
		texDesc.Format = D3D11Mapping::Mapping(format);

	// Generate mipmaps if enable
	if (mCreateFlags & TexCreate_GenerateMipmaps)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		assert(numMipMaps == 0);

		// May defer it to DeviceContext::GenerateMips called.
		mMipLevels = Texture::CalculateMipmapLevels((std::max)(width, height));
		texDesc.MipLevels = mMipLevels;
	}
	else
	{
		mMipLevels = numMipMaps;
		texDesc.MipLevels = mMipLevels;
	}

	if (mCreateFlags & TexCreate_UAV)				texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	if (mCreateFlags & TexCreate_ShaderResource)	texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (mCreateFlags & TexCreate_RenderTarget)		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;

	if (initData)
	{
		uint32_t numSubResource = mTextureArraySize * mMipLevels;
		vector<D3D11_SUBRESOURCE_DATA> subResourceData(numSubResource);

		for (uint32_t arrayIdx = 0; arrayIdx < mTextureArraySize; ++arrayIdx)
		{
			for (uint32_t level = 0; level < mMipLevels; ++level)
			{
				uint32_t index = arrayIdx * mMipLevels + level;
				
				subResourceData[index].pSysMem = initData[index].pData;
				subResourceData[index].SysMemPitch = initData[index].rowPitch;
				subResourceData[index].SysMemSlicePitch = 0;

				//uint32_t levelWidth = (std::max)(1U, mWidth >> level);
				//uint32_t levelHeight = (std::max)(1U, mHeight >> level);
				/*if (PixelFormatUtils::IsCompressed(format))
				{
				uint32_t blockSize = (texDesc.Format == DXGI_FORMAT_BC1_UNORM ? 8 : 16);
				subResourceData[index].SysMemPitch = blockSize * ((levelWidth+3)/4);
				assert(initData[index].rowPitch == subResourceData[index].SysMemPitch);
				}
				else
				{
				subResourceData[index].SysMemPitch = initData[index].rowPitch;
				}*/
			}
		}

		D3D11_VERRY( deviceD3D11->CreateTexture2D( &texDesc, &subResourceData[0], &TextureD3D11) );
	}
	else 
	{
		D3D11_VERRY( deviceD3D11->CreateTexture2D( &texDesc, NULL, &TextureD3D11) );
	}
	
	// Create shader resource view
	if (mCreateFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;

		if (mCreateFlags & TexCreate_DepthStencilTarget)
		{
			// Use depth shader resource format
			viewDesc.Format = D3D11Mapping::GetDepthShaderResourceFormat(format);
		}
		else
			viewDesc.Format = texDesc.Format;

		if (mTextureArraySize <= 1)
		{
			if (mSampleCount > 1 && (mCreateFlags & TexCreate_RenderTarget))
			{
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				viewDesc.Texture2D.MostDetailedMip = 0;
				viewDesc.Texture2D.MipLevels = mMipLevels;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			}
		}
		else
		{
			if (sampleCount > 1 && (mCreateFlags & TexCreate_RenderTarget))
			{
				viewDesc.Texture2DMSArray.ArraySize = mTextureArraySize;
				viewDesc.Texture2DMSArray.FirstArraySlice = 0;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			}
			else
			{
				viewDesc.Texture2DArray.FirstArraySlice = 0;
				viewDesc.Texture2DArray.ArraySize = mTextureArraySize;
				viewDesc.Texture2DArray.MostDetailedMip = 0;
				viewDesc.Texture2DArray.MipLevels = mMipLevels;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			}	
		}
		
		ID3D11ShaderResourceView* srvD3D11;
		D3D11_VERRY( deviceD3D11->CreateShaderResourceView(TextureD3D11, &viewDesc, &srvD3D11) );

		mTextureSRV = std::make_shared<D3D11Texture2DSRV>(srvD3D11);
	}
}

D3D11Texture2D::D3D11Texture2D(PixelFormat format,uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality)
	: D3D11Texture(TT_Texture2D, format, 1, 1, sampleCount, sampleQuality, EAH_GPU_Read | EAH_GPU_Write, TexCreate_DepthStencilTarget)
{
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = D3D11Mapping::Mapping(format);
	descDepth.SampleDesc.Count = sampleCount;
	descDepth.SampleDesc.Quality = sampleQuality;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateTexture2D( &descDepth, NULL, &TextureD3D11 ) );
}



D3D11Texture2D::~D3D11Texture2D()
{
	SAFE_RELEASE(TextureD3D11);
}

void* D3D11Texture2D::Map2D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	ID3D11Resource* resourceMapD3D11;
	uint32_t subResource = D3D11CalcSubresource(level, arrayIndex, mMipLevels);

	if (mapType  == RMA_Read_Only || mapType == RMA_Read_Write)
	{
		if (!mStagingTextureD3D11)
		{
			// Create staging texture if not created.
			D3D11_TEXTURE2D_DESC texDesc;
			TextureD3D11->GetDesc(&texDesc);

			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (mapType == RMA_Read_Write)
				texDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

			texDesc.BindFlags = 0;
			texDesc.Usage = D3D11_USAGE_STAGING;
			texDesc.MiscFlags = 0;

			ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
			D3D11_VERRY( deviceD3D11->CreateTexture2D( &texDesc, NULL, (ID3D11Texture2D**)&mStagingTextureD3D11) );
		}
		
		if (PixelFormatUtils::IsDepth(mFormat))
		{
			/**
			 * Note If you use CopySubresourceRegion with a depth-stencil buffer 
			  * or a multisampled resource, you must copy the whole subresource.
			  * In this situation, you must pass 0 to the DstX, DstY, and DstZ parameters 
			 *  and NULL to the pSrcBox parameter.
			 */
			deviceContextD3D11->CopySubresourceRegion(mStagingTextureD3D11, subResource, 0, 0, 0, TextureD3D11, subResource, nullptr);
		}
		else
		{
			CD3D11_BOX sourceRegion(0, 0, 0, CalculateLevelSize(mWidth, level), CalculateLevelSize(mHeight, level), 1);
			deviceContextD3D11->CopySubresourceRegion(mStagingTextureD3D11, subResource, 0, 0, 0, TextureD3D11, subResource, &sourceRegion);
		}

		resourceMapD3D11 = mStagingTextureD3D11;
	}
	else
	{
		resourceMapD3D11 = TextureD3D11;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D11_VERRY( deviceContextD3D11->Map(resourceMapD3D11, subResource, D3D11Mapping::Mapping(mapType), 0, &mapped) );

	// Keep map type
	mTextureMapAccess = mapType;

	rowPitch = mapped.RowPitch;
	return mapped.pData;
}

void D3D11Texture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	uint32_t subResource = D3D11CalcSubresource(level, arrayIndex, mMipLevels);

	if (mTextureMapAccess  == RMA_Read_Only || mTextureMapAccess == RMA_Read_Write)
	{
		deviceContextD3D11->Unmap(mStagingTextureD3D11, subResource);

		CD3D11_BOX sourceRegion(0, 0, 0, CalculateLevelSize(mWidth, level), CalculateLevelSize(mHeight, level), 1);
		
		if (PixelFormatUtils::IsDepth(mFormat))
		{
			deviceContextD3D11->CopySubresourceRegion(TextureD3D11, subResource, 0, 0, 0, mStagingTextureD3D11, subResource, nullptr);
		}
		else
		{
			CD3D11_BOX sourceRegion(0, 0, 0, CalculateLevelSize(mWidth, level), CalculateLevelSize(mHeight, level), 1);
			deviceContextD3D11->CopySubresourceRegion(TextureD3D11, subResource, 0, 0, 0, mStagingTextureD3D11, subResource, &sourceRegion);
		}
	}
	else
	{
		deviceContextD3D11->Unmap(TextureD3D11, subResource);
	}

}



}