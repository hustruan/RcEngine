#ifndef D3D11ShaderResourceView_h__
#define D3D11ShaderResourceView_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class GraphicsBuffer;
class Texture;

// Shader Resource View
//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ShaderResouceView : public ShaderResourceView
{
public:
	D3D11ShaderResouceView();
	virtual ~D3D11ShaderResouceView();

public:
	ID3D11ShaderResourceView* ShaderResourceViewD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11StructuredBufferSRV : public D3D11ShaderResouceView
{
public:
	D3D11StructuredBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth);
};

class _D3D11Export D3D11TextureBufferSRV : public D3D11ShaderResouceView
{
public:
	D3D11TextureBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
};

// Texture SRV
//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11Texture1DSRV : public D3D11ShaderResouceView
{
public:
	D3D11Texture1DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
};

class _D3D11Export D3D11Texture2DSRV : public D3D11ShaderResouceView
{
public:
	D3D11Texture2DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	D3D11Texture2DSRV(ID3D11ShaderResourceView* srvD3D11);
};

class _D3D11Export D3D11Texture3DSRV : public D3D11ShaderResouceView
{
public:
	D3D11Texture3DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels);
};

class _D3D11Export D3D11TextureCubeSRV : public D3D11ShaderResouceView
{
public:
	D3D11TextureCubeSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	D3D11TextureCubeSRV(ID3D11ShaderResourceView* srvD3D11);
};

//// Texture shader resource view creator for each type
//typedef bool (TextureSRVCreator)(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//
//
//template<TextureType type, TextureSRVCreator CreateFunc>
//class _D3D11Export D3D11TextureSRVTemplate : public D3D11TextureSRV
//{
//public:
//	D3D11TextureSRVTemplate(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize)
//		: D3D11TextureSRV(texture)
//	{
//		bool success = CreateFunc(texture, &ShaderResourceViewD3D11);
//	}
//};
//
//// Helper function to create texture shader resource view
//bool CreateTexture1DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTexture2DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTexture3DSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTextureCubeSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//
//typedef D3D11TextureSRVTemplate<TT_Texture1D, CreateTexture1DSRV> D3D11Texture1DSRV;
//typedef D3D11TextureSRVTemplate<TT_Texture2D, CreateTexture2DSRV> D3D11Texture2DSRV;
//typedef D3D11TextureSRVTemplate<TT_Texture3D, CreateTexture3DSRV> D3D11Texture3DSRV;
//typedef D3D11TextureSRVTemplate<TT_TextureCube, CreateTextureCubeSRV> D3D11TextureCubeSRV;


// Unordered Access View
//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11UnorderedAccessView : public UnorderedAccessView
{
public:
	D3D11UnorderedAccessView();
	virtual ~D3D11UnorderedAccessView();

	void Clear(const float4& clearData);
	void Clear(const uint4& clearData);

public:
	ID3D11UnorderedAccessView* UnorderedAccessViewD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11TextureBufferUAV : public D3D11UnorderedAccessView
{
public:
	D3D11TextureBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format);
};

class _D3D11Export D3D11StructuredBufferUAV : public D3D11UnorderedAccessView
{
public:
	D3D11StructuredBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount);
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11Texture1DUAV  : public D3D11UnorderedAccessView
{
public:
	D3D11Texture1DUAV(const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
};

class _D3D11Export D3D11Texture2DUAV  : public D3D11UnorderedAccessView
{
public:
	D3D11Texture2DUAV(const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
};

//class _D3D11Export D3D11Texture3DUAV  : public D3D11TextureUAV
//{
//public:
//	D3D11Texture3DUAV(const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstWSlice, uint32_t wSize);
//};

//class _D3D11Export D3D11TextureCubeUAV  : public D3D11TextureUAV
//{
//public:
//	D3D11TextureCubeUAV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
//};

}

#endif // D3D11ShaderResourceView_h__