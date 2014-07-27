#ifndef D3D11Texture_h__
#define D3D11Texture_h__

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _D3D11Export D3D11Texture : public Texture
{
public:
	D3D11Texture(
		TextureType type,
		PixelFormat format, 
		uint32_t arraySize, 
		uint32_t numMipMaps,
		uint32_t sampleCount, 
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t flags);
	virtual ~D3D11Texture() {}

	virtual void BuildMipMap();

	virtual void* Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType);
	virtual void* Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);
	virtual void* Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch);
	virtual void* MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level);
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

	virtual void CopyToTexture(Texture& destTexture);

protected:
	// Used for read 
	ID3D11Resource* mStagingTextureD3D11; 
	ResourceMapAccess mTextureMapAccess;
};

class _D3D11Export D3D11Texture1D : public D3D11Texture
{
public:
	D3D11Texture1D(
		PixelFormat format,
		uint32_t arraySize,
		uint32_t numMipMaps,
		uint32_t width, 
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);
	~D3D11Texture1D();

	void* Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType);
	void Unmap1D(uint32_t arrayIndex, uint32_t level);

public:
	ID3D11Texture1D* TextureD3D11;
};

class _D3D11Export D3D11Texture2D : public D3D11Texture
{
public:
	D3D11Texture2D(
		PixelFormat format,
		uint32_t arraySize, 
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t height,
		uint32_t sampleCount, 
		uint32_t sampleQuality, 
		uint32_t accessHint,
		uint32_t flag,
		ElementInitData* initData);

	// For Back Depth Buffer
	D3D11Texture2D(PixelFormat format,uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality);

	~D3D11Texture2D();

	ID3D11Texture2D* GetTexture() const { return TextureD3D11; }

	void* Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);
	void Unmap2D(uint32_t arrayIndex, uint32_t level);

public:
	ID3D11Texture2D* TextureD3D11;
};

class _D3D11Export D3D11Texture3D : public D3D11Texture
{
public:
	D3D11Texture3D(
		PixelFormat format, 
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t height, 
		uint32_t depth, 
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);
	~D3D11Texture3D();

	//void* Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch);
	//void Unmap3D(uint32_t arrayIndex, uint32_t level);

public:
	ID3D11Texture3D* TextureD3D11;
};

class _D3D11Export D3D11TextureCube : public D3D11Texture
{
public:
	D3D11TextureCube(
		PixelFormat format, 
		uint32_t arraySize, 
		uint32_t numMipMaps, 
		uint32_t width, 
		uint32_t height, 
		uint32_t sampleCount, 
		uint32_t sampleQuality,
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);

	~D3D11TextureCube();

	//void* MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);
	//void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

public:
	ID3D11Texture2D* TextureD3D11;
};

}


#endif // D3D11Texture_h__
