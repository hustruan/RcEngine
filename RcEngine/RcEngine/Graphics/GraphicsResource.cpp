#include <Graphics/GraphicsResource.h>

namespace RcEngine {

GraphicsResouce::GraphicsResouce()
{

}


GraphicsBuffer::GraphicsBuffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags )
	: mBufferSize(bufferSize),
	  mAccessHint(accessHint),
	  mCreateFlags(flags)
{

}

Texture::Texture( TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: mType(type),
	  mFormat(format),
	  mSampleCount(sampleQuality),
	  mSampleQuality(sampleQuality),
	  mMipLevels(numMipMaps),
	  mAccessHint(accessHint),
	  mCreateFlags(flags),
	  mTextureArraySize(1),
	  mDepth(1)
{

}

uint32_t Texture::CalculateMipmapLevels( uint32_t n )
{
	//return int(ceil( log( float(_n) ) / log( 2.f ) )) + 1;
	//return uint32_t(1.0 + floor(log(float(n)/log(2.f))));

	uint32_t levels = 1;
	while (n >> levels) levels++;
	return levels;
}

uint32_t Texture::CalculateLevelSize( uint32_t s, uint32_t level )
{
	return std::max(1U, s >> level);
}

//////////////////////////////////////////////////////////////////////////
Shader::Shader( ShaderType shaderType )
	: mShaderType(shaderType)
{

}

ShaderPipeline::ShaderPipeline( Effect& effect )
	: mEffect(effect)
{

}

void ShaderPipeline::AttachShader( const shared_ptr<Shader>& shader )
{
	assert(shader);
	mShaderStages[shader->GetShaderType()] = shader;
}

void ShaderPipeline::DetachShader( const shared_ptr<Shader>& shader )
{
	mShaderStages[shader->GetShaderType()] = nullptr;
}

}