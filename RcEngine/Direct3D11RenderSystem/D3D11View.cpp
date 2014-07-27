#include "D3D11View.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11ShaderResouceView::D3D11ShaderResouceView()
	: ShaderResourceViewD3D11(nullptr)
{

}

D3D11ShaderResouceView::~D3D11ShaderResouceView()
{
	SAFE_RELEASE(ShaderResourceViewD3D11);
}


//////////////////////////////////////////////////////////////////////////
D3D11StructuredBufferSRV::D3D11StructuredBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.ElementOffset = elementOffset;
	desc.Buffer.ElementWidth = elementWidth;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	D3D11_VERRY(deviceD3D11->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureBufferSRV::D3D11TextureBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.ElementOffset = elementOffset;
	desc.Buffer.ElementWidth = elementWidth;
	
	//assert(PixelFormatUtils::GetNumElemBytes(format) * elementCount == buffer->GetBufferSize());
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;

	D3D11_BUFFER_DESC bdesc;
	bufferD3D11->GetDesc(&bdesc);

	D3D11_VERRY(deviceD3D11->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11));
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture1DSRV::D3D11Texture1DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11Texture1D* textureD3D11 = static_cast_checked<D3D11Texture1D*>(texture.get());

	D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{	
		viewDesc.Texture1D.MostDetailedMip = mostDetailedMip;
		viewDesc.Texture1D.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;	
	}
	else
	{
		viewDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
		viewDesc.Texture1DArray.ArraySize = arraySize;
		viewDesc.Texture1DArray.MostDetailedMip = mostDetailedMip;
		viewDesc.Texture1DArray.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture2DSRV::D3D11Texture2DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11Texture2D* textureD3D11 = static_cast_checked<D3D11Texture2D*>(texture.get());

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	if (createFlags & TexCreate_DepthStencilTarget)
	{
		// Use depth shader resource format
		viewDesc.Format = D3D11Mapping::GetDepthShaderResourceFormat(texture->GetTextureFormat());
	}
	else
		viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{
		if (textureD3D11->GetSampleCount() <= 1)
		{
			viewDesc.Texture2D.MostDetailedMip = mostDetailedMip;
			viewDesc.Texture2D.MipLevels = mipLevels;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			assert(createFlags & TexCreate_RenderTarget);
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
	}
	else
	{
		if (textureD3D11->GetSampleCount() <= 1)
		{
			viewDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
			viewDesc.Texture2DArray.ArraySize = arraySize;
			viewDesc.Texture2DArray.MostDetailedMip = mostDetailedMip;
			viewDesc.Texture2DArray.MipLevels = mipLevels;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		}
		else
		{
			viewDesc.Texture2DMSArray.ArraySize = firstArraySlice;
			viewDesc.Texture2DMSArray.FirstArraySlice = arraySize;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		}
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11) );
}

D3D11Texture2DSRV::D3D11Texture2DSRV( ID3D11ShaderResourceView* srvD3D11 )
{
	ShaderResourceViewD3D11 = srvD3D11;
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture3DSRV::D3D11Texture3DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels )
{
	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	viewDesc.Texture3D.MostDetailedMip = mostDetailedMip;
	viewDesc.Texture3D.MipLevels = mipLevels;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11Texture3D* textureD3D11 = static_cast_checked<D3D11Texture3D*>(texture.get());
	D3D11_VERRY( deviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureCubeSRV::D3D11TextureCubeSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);
	
	D3D11TextureCube* textureD3D11 = static_cast_checked<D3D11TextureCube*>(texture.get());
	
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	
	if (textureD3D11->GetTextureArraySize() <= 1)
	{
		viewDesc.TextureCube.MostDetailedMip = mostDetailedMip;
		viewDesc.TextureCube.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	}
	else
	{
		// Need test
		assert(false);
		viewDesc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
		viewDesc.TextureCubeArray.MipLevels = mipLevels;
		viewDesc.TextureCubeArray.First2DArrayFace = firstArraySlice;
		viewDesc.TextureCubeArray.NumCubes = arraySize;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	}
	
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11) );
}

D3D11TextureCubeSRV::D3D11TextureCubeSRV( ID3D11ShaderResourceView* srvD3D11 )
{
	ShaderResourceViewD3D11 = srvD3D11;
}


//bool CreateTexture1DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11Texture1D* textureD3D11 = static_cast_checked<D3D11Texture1D*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{	
//		viewDesc.Texture1D.MostDetailedMip = mostDetailedMip;
//		viewDesc.Texture1D.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;	
//	}
//	else
//	{
//		viewDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
//		viewDesc.Texture1DArray.ArraySize = arraySize;
//		viewDesc.Texture1DArray.MostDetailedMip = mostDetailedMip;
//		viewDesc.Texture1DArray.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
//	}
//
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	
//	return SUCCEEDED(hr);
//}
//
//bool CreateTexture2DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11Texture2D* textureD3D11 = static_cast_checked<D3D11Texture2D*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{
//		if (textureD3D11->GetSampleCount() <= 1)
//		{
//			viewDesc.Texture2D.MostDetailedMip = mostDetailedMip;
//			viewDesc.Texture2D.MipLevels = mipLevels;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		}
//		else
//		{
//			assert(createFlags & TexCreate_RenderTarget);
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
//		}
//	}
//	else
//	{
//		if (textureD3D11->GetSampleCount() <= 1)
//		{
//			viewDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
//			viewDesc.Texture2DArray.ArraySize = arraySize;
//			viewDesc.Texture2DArray.MostDetailedMip = mostDetailedMip;
//			viewDesc.Texture2DArray.MipLevels = mipLevels;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
//		}
//		else
//		{
//			viewDesc.Texture2DMSArray.ArraySize = firstArraySlice;
//			viewDesc.Texture2DMSArray.FirstArraySlice = arraySize;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
//		}
//	}
//
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}
//
//bool CreateTexture3DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//	viewDesc.Texture3D.MostDetailedMip = mostDetailedMip;
//	viewDesc.Texture3D.MipLevels = mipLevels;
//	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
//
//	D3D11Texture3D* textureD3D11 = static_cast_checked<D3D11Texture3D*>(texture.get());
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}
//
//bool CreateTextureCubeSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11TextureCube* textureD3D11 = static_cast_checked<D3D11TextureCube*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{
//		viewDesc.TextureCube.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCube.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	}
//	else
//	{
//		// Need test
//		assert(false);
//		viewDesc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCubeArray.MipLevels = mipLevels;
//		viewDesc.TextureCubeArray.First2DArrayFace = firstArraySlice;
//		viewDesc.TextureCubeArray.NumCubes = arraySize;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
//	}
//
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}

//////////////////////////////////////////////////////////////////////////
D3D11UnorderedAccessView::D3D11UnorderedAccessView()
	: UnorderedAccessViewD3D11(nullptr)
{

}

//////////////////////////////////////////////////////////////////////////
D3D11UnorderedAccessView::~D3D11UnorderedAccessView()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}

void D3D11UnorderedAccessView::Clear( const float4& clearData )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	float clrData[4] = { clearData[0], clearData[1], clearData[2], clearData[3] };
	deviceContextD3D11->ClearUnorderedAccessViewFloat(UnorderedAccessViewD3D11, clrData);
}

void D3D11UnorderedAccessView::Clear( const uint4& clearData )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	uint32_t clrData[4] = { clearData[0], clearData[1], clearData[2], clearData[3] };
	deviceContextD3D11->ClearUnorderedAccessViewUint(UnorderedAccessViewD3D11, clrData);
}


//////////////////////////////////////////////////////////////////////////
D3D11TextureBufferUAV::D3D11TextureBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.FirstElement = elementOffset;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	D3D11_VERRY( deviceD3D11->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
D3D11StructuredBufferUAV::D3D11StructuredBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = elementOffset;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	D3D11_VERRY( deviceD3D11->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
//D3D11Texture2DUAV::D3D11Texture2DUAV( const shared_ptr<Texture>& texture, uint32_t level )
//	: D3D11TextureUAV(texture)
//{
//	D3D11Texture2D* textureD3D11 = (static_cast<D3D11Texture2D*>(texture.get()));
//
//	uint32_t createFlags = textureD3D11->GetCreateFlags();
//
//	assert(textureD3D11->GetTextureArraySize() <= 1);
//	assert(createFlags & TexCreate_UAV);
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
//
//	desc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
//	desc.Texture2D.MipSlice = level;
//
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &desc, &UnorderedAccessViewD3D11);
//}

D3D11Texture1DUAV::D3D11Texture1DUAV( const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize )
{
	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_UAV);

	D3D11Texture1D* textureD3D11 = static_cast_checked<D3D11Texture1D*>(texture.get());

	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{	
		viewDesc.Texture1D.MipSlice = mipSlice;
		viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;	
	}
	else
	{
		viewDesc.Texture1DArray.MipSlice = mipSlice;
		viewDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
		viewDesc.Texture1DArray.ArraySize = arraySize;
		viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &viewDesc, &UnorderedAccessViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture2DUAV::D3D11Texture2DUAV( const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize )
{
	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_UAV);
	assert(texture->GetSampleCount() <= 1);

	D3D11Texture2D* textureD3D11 = static_cast_checked<D3D11Texture2D*>(texture.get());

	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof(viewDesc) );

	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{
		viewDesc.Texture2D.MipSlice = mipSlice;
		viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	}
	else
	{
		viewDesc.Texture2DArray.MipSlice = mipSlice;
		viewDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
		viewDesc.Texture2DArray.ArraySize = arraySize;
		viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY( deviceD3D11->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &viewDesc, &UnorderedAccessViewD3D11) );
}

//////////////////////////////////////////////////////////////////////////
//D3D11Texture3DUAV::D3D11Texture3DUAV( const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstWSlice, uint32_t wSize )
//	: D3D11TextureUAV(texture)
//{
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_UAV);
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//	viewDesc.Texture3D.FirstWSlice = firstWSlice;
//	viewDesc.Texture3D.WSize = wSize;
//	viewDesc.Texture3D.MipSlice = mipSlice;
//	viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
//
//	D3D11Texture3D* textureD3D11 = static_cast_checked<D3D11Texture3D*>(texture.get());
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &viewDesc, &UnorderedAccessViewD3D11);
//}

//D3D11TextureCubeSRV::D3D11TextureCubeSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
//	: D3D11TextureSRV(texture)
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11TextureCube* textureD3D11 = static_cast_checked<D3D11TextureCube*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{
//		viewDesc.TextureCube.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCube.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	}
//	else
//	{
//		// Need test
//		assert(false);
//		viewDesc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCubeArray.MipLevels = mipLevels;
//		viewDesc.TextureCubeArray.First2DArrayFace = firstArraySlice;
//		viewDesc.TextureCubeArray.NumCubes = arraySize;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
//	}
//
//	HRESULT hr = gD3D11Device->DeviceD3D11->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11);
//}

}