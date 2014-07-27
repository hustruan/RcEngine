#ifndef RHRenderFactory_h__
#define RHRenderFactory_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/RenderState.h>

namespace RcEngine {

class ShaderResourceView;
class UnorderedAccessView;

struct ElementInitData;
struct ShaderMacro;
struct VertexElement;

class _ApiExport RenderFactory
{
public:
	RenderFactory();
	virtual ~RenderFactory() {}

	// States
	shared_ptr<BlendState> CreateBlendState( const BlendStateDesc& desc );
	shared_ptr<SamplerState> CreateSamplerState( const SamplerStateDesc& desc );
	shared_ptr<RasterizerState> CreateRasterizerState( const RasterizerStateDesc& desc );
	shared_ptr<DepthStencilState> CreateDepthStencilState( const DepthStencilStateDesc& desc ); 
	
	// Vertex Input Layout
	virtual shared_ptr<VertexDeclaration> CreateVertexDeclaration(VertexElement* elems, uint32_t count) = 0;

	// Frame buffer
	virtual shared_ptr<FrameBuffer> CreateFrameBuffer(uint32_t width, uint32_t height) = 0;

	// Buffer resource
	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateUniformBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateTextureBuffer(PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;

	// Texture resource
	virtual shared_ptr<Texture> CreateTexture1D(
		uint32_t width,
		PixelFormat format, 
		uint32_t arrSize, 
		uint32_t numMipMaps,
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTexture2D(
		uint32_t width,
		uint32_t height,
		PixelFormat format, 
		uint32_t arrSize,
		uint32_t numMipMaps,
		uint32_t sampleCount,
		uint32_t sampleQuality, 
		uint32_t accessHint,
		uint32_t createFlags,
		ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTexture3D(
		uint32_t width, 
		uint32_t height,
		uint32_t depth, 
		PixelFormat format, 
		uint32_t numMipMaps, 
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTextureCube(
		uint32_t width, 
		uint32_t height,
		PixelFormat format,
		uint32_t arraySize, 
		uint32_t numMipMaps, 
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t createFlags,
		ElementInitData* initData) = 0;

	// Shader resource
	virtual shared_ptr<Shader> CreateShader(ShaderType type) = 0;
	virtual shared_ptr<ShaderPipeline> CreateShaderPipeline(Effect& effect) = 0;
	
	// Utility function
	shared_ptr<Texture> LoadTextureFromFile(const String& filename);

	void SaveTextureToFile(const String& filename, const shared_ptr<Texture>& texture);
	void SaveLinearDepthTextureToFile(const String& filename, const shared_ptr<Texture>& texture, float projM33, float projM43);

	shared_ptr<Shader> LoadShaderFromFile(
		ShaderType shaderType, 
		const String& filename, 
		const ShaderMacro* macros,
		uint32_t macroCount,
		const String& entryPoint = "");

	// Shader resource view
	virtual shared_ptr<ShaderResourceView> CreateStructuredBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride) = 0;
	virtual shared_ptr<ShaderResourceView> CreateTextureBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format) = 0;

	virtual shared_ptr<UnorderedAccessView> CreateStructuredBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride) = 0;
	virtual shared_ptr<UnorderedAccessView> CreateTextureBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format) = 0;

	/**
	 * Create shader resource view from a subset of texture.
	 *
	 * Set mipLevels to -1 to indicate all the mipmap levels from mostDetailedMip on down to least detailed
	 */
	virtual shared_ptr<ShaderResourceView> CreateTexture1DSRV(const shared_ptr<Texture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize) = 0;
	
	virtual shared_ptr<ShaderResourceView> CreateTexture2DSRV(const shared_ptr<Texture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize) = 0;
	
	virtual shared_ptr<ShaderResourceView> CreateTexture3DSRV(const shared_ptr<Texture>& texture) = 0;
	
	virtual shared_ptr<ShaderResourceView> CreateTextureCubeSRV(const shared_ptr<Texture>& texture) = 0;

	virtual shared_ptr<UnorderedAccessView> CreateTexture1DUAV(const shared_ptr<Texture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize) = 0;

	virtual shared_ptr<UnorderedAccessView> CreateTexture2DUAV(const shared_ptr<Texture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize) = 0;

	virtual shared_ptr<UnorderedAccessView> CreateTexture3DUAV(const shared_ptr<Texture>& texture) = 0;
	
	virtual shared_ptr<UnorderedAccessView> CreateTextureCubeUAV(const shared_ptr<Texture>& texture) = 0;

	// Render target view
	virtual shared_ptr<RenderView> CreateDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level, uint32_t flags = 0) = 0;
	virtual shared_ptr<RenderView> CreateRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level) = 0;
	virtual shared_ptr<RenderView> CreateRenderTargetViewArray(const shared_ptr<Texture>& texture, uint32_t level) = 0;

protected:
	virtual shared_ptr<BlendState> CreateBlendStateImpl(const BlendStateDesc& desc) = 0;
	virtual shared_ptr<SamplerState> CreateSamplerStateImpl(const SamplerStateDesc& desc) = 0;
	virtual shared_ptr<RasterizerState> CreateRasterizerStateImpl(const RasterizerStateDesc& desc) = 0;
	virtual shared_ptr<DepthStencilState> CreateDepthStencilStateImpl(const DepthStencilStateDesc& desc) = 0;

protected:

	typedef size_t ShaderHashCode;
	std::map<ShaderHashCode, shared_ptr<Shader> > mShaderPool;
	std::map<BlendStateDesc, shared_ptr<BlendState> > mBlendStatePool;
	std::map<SamplerStateDesc, shared_ptr<SamplerState> > mSamplerStatePool;
	std::map<RasterizerStateDesc, shared_ptr<RasterizerState> > mRasterizerStatePool;
	std::map<DepthStencilStateDesc, shared_ptr<DepthStencilState> > mDepthStecilStatePool;
};









}


#endif // RHRenderFactory_h__
