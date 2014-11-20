#include "D3D11Factory.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"
#include "D3D11Texture.h"
#include "D3D11FrameBuffer.h"
#include "D3D11View.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "D3D11VertexDeclaration.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Factory::D3D11Factory()
	: RenderFactory()
{

}

shared_ptr<GraphicsBuffer> D3D11Factory::CreateVertexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<GraphicsBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Vertex | createFlags), 0, initData) );
}

shared_ptr<GraphicsBuffer> D3D11Factory::CreateIndexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return shared_ptr<GraphicsBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Index | createFlags), 0, initData) );
}

shared_ptr<GraphicsBuffer> D3D11Factory::CreateConstantBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return shared_ptr<GraphicsBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Constant | createFlags), 0, initData) );
}

shared_ptr<GraphicsBuffer> D3D11Factory::CreateTextureBuffer( PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(format);
	return shared_ptr<GraphicsBuffer>(
		new D3D11Buffer(texelSize * elementCount, accessHint, (BufferCreate_Texture | createFlags), texelSize, initData) );
}

shared_ptr<GraphicsBuffer> D3D11Factory::CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData)
{
	return shared_ptr<GraphicsBuffer>(
		new D3D11Buffer(strutureStride * elementCount, accessHint, (BufferCreate_Structured | createFlags), strutureStride, initData) );
}

shared_ptr<Texture> D3D11Factory::CreateTexture1D( uint32_t width, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<Texture>(
		new D3D11Texture1D(format, arrSize, numMipMaps, width, accessHint, createFlags, initData) );
}

shared_ptr<Texture> D3D11Factory::CreateTexture2D( uint32_t width, uint32_t height, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<Texture>(
		new D3D11Texture2D(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData) );
}

shared_ptr<Texture> D3D11Factory::CreateTexture3D( uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<Texture>(
		new D3D11Texture3D(format, numMipMaps, width, height, depth, accessHint, createFlags, initData) );
}

shared_ptr<Texture> D3D11Factory::CreateTextureCube( uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<Texture>(
		new D3D11TextureCube(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData) );
}

shared_ptr<Shader> D3D11Factory::CreateShader( ShaderType type )
{
	switch (type)
	{
	case ST_Vertex:				return shared_ptr<Shader>( new D3D11VertexShader() );
	case ST_TessControl:		return shared_ptr<Shader>( new D3D11HullShader() );
	case ST_TessEval:			return shared_ptr<Shader>( new D3D11DomainShader() );
	case ST_Geomerty:			return shared_ptr<Shader>( new D3D11GeometryShader() );
	case ST_Pixel:				return shared_ptr<Shader>( new D3D11PixelShader() );
	case ST_Compute:			return shared_ptr<Shader>( new D3D11ComputeShader() );
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Undefined shader type", "D3D11Factory::CreateShader");
	}
}

shared_ptr<ShaderPipeline> D3D11Factory::CreateShaderPipeline( Effect& effect )
{
	return shared_ptr<ShaderPipeline>( new D3D11ShaderPipeline(effect) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateStructuredBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride )
{
	return shared_ptr<ShaderResourceView>( new D3D11StructuredBufferSRV(buffer, elementOffset, elementCount) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateTextureBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<ShaderResourceView>( new D3D11TextureBufferSRV(buffer, elementOffset, elementCount, format) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateTexture1DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<ShaderResourceView>( new D3D11Texture1DSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateTexture2DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<ShaderResourceView>( new D3D11Texture2DSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateTexture3DSRV( const shared_ptr<Texture>& texture )
{
	return shared_ptr<ShaderResourceView>( new D3D11Texture3DSRV(texture, 0, texture->GetMipLevels()) );
}

shared_ptr<ShaderResourceView> D3D11Factory::CreateTextureCubeSRV( const shared_ptr<Texture>& texture )
{
	return shared_ptr<ShaderResourceView>( new D3D11TextureCubeSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateStructuredBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride )
{
	return shared_ptr<UnorderedAccessView>( new D3D11StructuredBufferUAV(buffer, elementOffset, elementCount) );
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateTextureBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<UnorderedAccessView>( new D3D11TextureBufferUAV(buffer, elementOffset, elementCount, format) );
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateTexture1DUAV( const shared_ptr<Texture>& texture, uint32_t level, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<UnorderedAccessView>( new D3D11Texture1DUAV(texture, level, firstArraySlice, arraySize) );
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateTexture2DUAV( const shared_ptr<Texture>& texture, uint32_t level, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<UnorderedAccessView>( new D3D11Texture2DUAV(texture, level, firstArraySlice, arraySize) );
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateTexture3DUAV( const shared_ptr<Texture>& texture )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "3D UAV not implemented!", "D3D11Factory::CreateTexture3DUAV");
}

shared_ptr<UnorderedAccessView> D3D11Factory::CreateTextureCubeUAV( const shared_ptr<Texture>& texture )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "CubeMap UAV not implemented!", "D3D11Factory::CreateTextureCubeUAV");
}

shared_ptr<RenderView> D3D11Factory::CreateDepthStencilView( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level, uint32_t flags )
{
	return shared_ptr<RenderView>( new D3D11DepthStencilView(texture, arrayIndex, level, flags) );
}

shared_ptr<RenderView> D3D11Factory::CreateRenderTargetView2D( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return shared_ptr<RenderView>( new D3D11RenderTargetView2D(texture, arrayIndex, level) );
}

shared_ptr<RenderView> D3D11Factory::CreateRenderTargetViewArray( const shared_ptr<Texture>& texture, uint32_t level )
{
	return shared_ptr<RenderView>( new D3D11RenderTargetViewArray(texture, level) );
}

shared_ptr<BlendState> D3D11Factory::CreateBlendStateImpl( const BlendStateDesc& desc )
{
	return shared_ptr<BlendState>( new D3D11BlendState(desc) );
}

shared_ptr<SamplerState> D3D11Factory::CreateSamplerStateImpl( const SamplerStateDesc& desc )
{
	return shared_ptr<SamplerState>( new D3D11SamplerState(desc) );
}

shared_ptr<RasterizerState> D3D11Factory::CreateRasterizerStateImpl( const RasterizerStateDesc& desc )
{
	return shared_ptr<RasterizerState>( new D3D11RasterizerState(desc) );
}

shared_ptr<DepthStencilState> D3D11Factory::CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc )
{
	return shared_ptr<DepthStencilState>( new D3D11DepthStencilState(desc) );
}

shared_ptr<VertexDeclaration> D3D11Factory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return shared_ptr<VertexDeclaration>( new D3D11VertexDeclaration(elems, count) );
}

shared_ptr<FrameBuffer> D3D11Factory::CreateFrameBuffer( uint32_t width, uint32_t height )
{
	return shared_ptr<FrameBuffer>( new D3D11FrameBuffer(width, height) );
}


}
