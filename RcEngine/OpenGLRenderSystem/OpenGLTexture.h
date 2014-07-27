#ifndef OpenGLTexture_h__
#define OpenGLTexture_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLTexture : public Texture
{
public:
	OpenGLTexture(
		TextureType type, 
		PixelFormat format,
		uint32_t arraySize, 
		uint32_t numMipMaps, 
		uint32_t sampleCount, 
		uint32_t sampleQuality, 
		uint32_t accessHint,
		uint32_t flags);
	virtual ~OpenGLTexture(void);

	inline GLuint GetTextureOGL() const		    { return mTextureOGL; }
	inline GLuint GetTextureTarget() const		{ return mTextureTarget; }

	virtual void* Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType);
	virtual void* Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);
	virtual void* Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch);
	virtual void* MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level);
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

	virtual void BuildMipMap();
	virtual void CopyToTexture(Texture& destTexture);

protected:

	/** Texture object if not use render buffer */
	GLuint mTextureOGL;
	GLenum mTextureTarget;

	GLuint mPixelBufferID;

	ResourceMapAccess mTextureMapAccess;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture1D : public OpenGLTexture
{
public:
	OpenGLTexture1D(
		PixelFormat format, 
		uint32_t arraySize, 
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);

	virtual void Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma,
		uint32_t xOffset, uint32_t width, void*& data);

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level);
};


//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture2D : public OpenGLTexture
{
public:
	OpenGLTexture2D(
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

	virtual void* Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch);
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level);

	virtual void CopyToTexture(Texture& destTexture);

private:
	// use texture storage if supported
	void CreateWithImmutableStorage(ElementInitData* initData);
	void CreateWithMutableStorage(ElementInitData* initData);
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture3D : public OpenGLTexture
{
public:
	OpenGLTexture3D(
		PixelFormat format,
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t height, 
		uint32_t depth, 
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);

	//virtual void Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma,
	//	uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
	//	uint32_t width, uint32_t height, uint32_t depth,
	//	void*& data, uint32_t& rowPitch, uint32_t& slicePitch);

	//virtual void Unmap3D(uint32_t arrayIndex, uint32_t level);
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTextureCube : public OpenGLTexture
{
public:
	OpenGLTextureCube(
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

	//virtual void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess tma,
	//	uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
	//	void*& data, uint32_t& rowPitch);

	//virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

private:
	// use texture storage if supported
	void CreateWithImmutableStorage(ElementInitData* initData);
	void CreateWithMutableStorage(ElementInitData* initData);
};

}

#endif // OpenGLTexture_h__
