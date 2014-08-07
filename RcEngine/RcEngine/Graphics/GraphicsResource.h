#ifndef GraphicsResource_h__
#define GraphicsResource_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Math/Vector.h>

namespace RcEngine {

#define MAP_ALL_BUFFER 0xFFFFFFFF

struct ElementInitData
{
	const void* pData;
	uint32_t rowPitch;
	uint32_t slicePitch;
};

struct ShaderMacro
{
	String Name;
	String Definition;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport GraphicsResouce
{
public:
	GraphicsResouce();
	virtual ~GraphicsResouce() {}

private:
	GraphicsResouce( const GraphicsResouce& );
	GraphicsResouce& operator= ( const GraphicsResouce& );
};

/** 
 * Resource view will not keep a copy of underlying resource,
 * Keep the underlying alive by yourself. 
 */
class _ApiExport ShaderResourceView
{
public:
	virtual ~ShaderResourceView() {}
};

class _ApiExport UnorderedAccessView 
{
public:
	virtual ~UnorderedAccessView() {}

	virtual void Clear(const float4& clearData) = 0;
	virtual void Clear(const uint4& clearData) = 0;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport GraphicsBuffer : public GraphicsResouce
{
public:
	GraphicsBuffer(uint32_t bufferSize, uint32_t accessHint, uint32_t flags);
	virtual ~GraphicsBuffer() {}

	inline uint32_t	GetAccessHint() const   { return mAccessHint; }
	inline uint32_t	GetBufferSize() const   { return mBufferSize; }
	inline uint32_t	GetCreateFlags() const	{ return mCreateFlags; }

	virtual void* Map(uint32_t offset, uint32_t length, ResourceMapAccess mapType) = 0;
	virtual void UnMap() = 0;

protected:
	uint32_t mBufferSize;
	uint32_t mAccessHint;
	uint32_t mCreateFlags;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport Texture : public GraphicsResouce
{
public:
	Texture(
		TextureType type, 
		PixelFormat format,
		uint32_t numMipMaps,
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t flags);
	virtual ~Texture() {}

	inline uint32_t GetSampleCount() const				{ return mSampleCount; }
	inline uint32_t GetSampleQuality() const			{ return mSampleQuality; }
	inline PixelFormat GetTextureFormat() const			{ return mFormat; }
	inline TextureType GetTextureType() const           { return mType; }
	inline uint32_t GetTextureArraySize() const			{ return mTextureArraySize; }
	inline uint32_t GetMipLevels() const				{ return mMipLevels; }
	inline uint32_t GetCreateFlags() const				{ return mCreateFlags; }

	inline shared_ptr<ShaderResourceView> GetShaderResourceView() const { return mTextureSRV; }

	inline uint32_t GetWidth() const		{ return mWidth; }
	inline uint32_t GetHeight() const		{ return mHeight; }
	inline uint32_t GetDepth() const		{ return mDepth; }

	virtual void BuildMipMap() = 0;

	virtual void* Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType) = 0;
	virtual void* Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch) = 0;
	virtual void* Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch, uint32_t& slicePitch) = 0;
	virtual void* MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType, uint32_t& rowPitch) = 0;

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level) = 0;

	virtual void CopyToTexture(Texture& destTexture) = 0;

protected:
	// Help function used to compute mipmap levels
	static uint32_t CalculateMipmapLevels( uint32_t n );
	static uint32_t CalculateLevelSize(uint32_t s, uint32_t level);

protected:
	TextureType mType;
	PixelFormat mFormat;
	uint32_t mSampleCount, mSampleQuality;
	uint32_t mMipLevels;
	uint32_t mTextureArraySize;
	uint32_t mWidth, mHeight, mDepth;
	uint32_t mAccessHint;
	uint32_t mCreateFlags;

	// SRV for the entire resource, only created with TexCreate_ShaderResource flag
	shared_ptr<ShaderResourceView> mTextureSRV;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport Shader : public GraphicsResouce
{
public:
	Shader(ShaderType shaderType);
	virtual ~Shader() {}

	inline ShaderType GetShaderType() const	{ return mShaderType; }

	virtual bool LoadFromByteCode(const String& filename) = 0;
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint = "") = 0;

protected:
	ShaderType mShaderType;
};

class _ApiExport ShaderPipeline : public GraphicsResouce
{
public:
	ShaderPipeline(Effect& effect);
	virtual~ ShaderPipeline() {}

	inline shared_ptr<Shader> GetShader(ShaderType shaderStage) const { return mShaderStages[shaderStage]; }

	void AttachShader(const shared_ptr<Shader>& shader);
	void DetachShader(const shared_ptr<Shader>& shader);
	
	virtual bool LinkPipeline() = 0;
	virtual void OnBind() = 0;
	virtual void OnUnbind() = 0;

protected:
	Effect& mEffect;
	shared_ptr<Shader> mShaderStages[ST_Count];
};

}

#endif // GraphicsResource_h__
