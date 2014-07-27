#pragma once
#ifndef OpenGLRenderFactory_h__
#define OpenGLRenderFactory_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/RenderFactory.h>

namespace RcEngine {

class _OpenGLExport OpenGLFactory : public RenderFactory
{
public:
	OpenGLFactory(void) {}
	~OpenGLFactory(void) {}
	
	virtual shared_ptr<VertexDeclaration> CreateVertexDeclaration(VertexElement* elems, uint32_t count);
	
	virtual shared_ptr<FrameBuffer> CreateFrameBuffer(uint32_t width, uint32_t height);

	// Buffer resource
	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateUniformBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateTextureBuffer(PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);

	// Shader
	virtual shared_ptr<Shader> CreateShader(ShaderType type);
	virtual shared_ptr<ShaderPipeline> CreateShaderPipeline(Effect& effect);

	// Texture resource
	virtual shared_ptr<Texture> CreateTexture1D(
		uint32_t width,
		PixelFormat format, 
		uint32_t arrSize, 
		uint32_t numMipMaps,
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData);

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
		ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTexture3D(
		uint32_t width, 
		uint32_t height,
		uint32_t depth, 
		PixelFormat format, 
		uint32_t numMipMaps, 
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData);

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
		ElementInitData* initData);

	// Create shader resource view from a subset of texture
	virtual shared_ptr<ShaderResourceView> CreateTexture1DSRV(const shared_ptr<Texture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<ShaderResourceView> CreateTexture2DSRV(const shared_ptr<Texture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<ShaderResourceView> CreateTexture3DSRV(const shared_ptr<Texture>& texture);
	
	virtual shared_ptr<ShaderResourceView> CreateTextureCubeSRV(const shared_ptr<Texture>& texture);

	virtual shared_ptr<UnorderedAccessView> CreateTexture1DUAV(const shared_ptr<Texture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<UnorderedAccessView> CreateTexture2DUAV(const shared_ptr<Texture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<UnorderedAccessView> CreateTexture3DUAV(const shared_ptr<Texture>& texture);
	
	virtual shared_ptr<UnorderedAccessView> CreateTextureCubeUAV(const shared_ptr<Texture>& texture);

	virtual shared_ptr<ShaderResourceView> CreateStructuredBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride);
	virtual shared_ptr<ShaderResourceView> CreateTextureBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format);

	// Unordered access view
	virtual shared_ptr<UnorderedAccessView> CreateStructuredBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride);
	virtual shared_ptr<UnorderedAccessView> CreateTextureBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format);

	// Render target view
	virtual shared_ptr<RenderView> CreateRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RenderView> CreateDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level, uint32_t flags = 0);
	virtual shared_ptr<RenderView> CreateRenderTargetViewArray(const shared_ptr<Texture>& texture, uint32_t level);

protected:
	virtual shared_ptr<BlendState> CreateBlendStateImpl(const BlendStateDesc& desc);
	virtual shared_ptr<SamplerState> CreateSamplerStateImpl(const SamplerStateDesc& desc);
	virtual shared_ptr<RasterizerState> CreateRasterizerStateImpl(const RasterizerStateDesc& desc);
	virtual shared_ptr<DepthStencilState> CreateDepthStencilStateImpl(const DepthStencilStateDesc& desc);
};


}

#endif // OpenGLRenderFactory_h__