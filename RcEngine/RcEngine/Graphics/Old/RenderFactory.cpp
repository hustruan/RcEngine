#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Shader.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/VertexDeclaration.h>
#include <IO/FileStream.h>
#include <IO/FileSystem.h>
#include <Core/Exception.h>
#include <Core/Context.h>


namespace RcEngine {

static String ShaderDirectory[RD_Count][2] = { 
	{"GLSL", "GLBC"},
	{"GLSL", "GLBC"},
	{"HLSL", "HLBC"}
};

static String ShaderSuffix[RD_Count][2] = { 
	{".glsl", ".glbc"},
	{".glsl", ".glbc"},
	{".hlsl", ".hlbc"}
};

RenderFactory::RenderFactory(void)
{
}


RenderFactory::~RenderFactory(void)
{

}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return std::make_shared<VertexDeclaration>(elems, count);
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( const std::vector<VertexElement>& elems )
{
	return std::make_shared<VertexDeclaration>(elems);
}

shared_ptr<DepthStencilState> RenderFactory::CreateDepthStencilState( const DepthStencilStateDesc& desc )
{
	shared_ptr<DepthStencilState> retVal;

	auto found = mDepthStecilStatePool.find(desc);
	if (found == mDepthStecilStatePool.end())
	{
		retVal = CreateDepthStencilStateImpl(desc);
		mDepthStecilStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}
	

shared_ptr<BlendState> RenderFactory::CreateBlendState( const BlendStateDesc& desc )
{
	shared_ptr<BlendState> retVal;

	auto found = mBlendStatePool.find(desc);
	if (found == mBlendStatePool.end())
	{
		retVal = CreateBlendStateImpl(desc);
		mBlendStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}


shared_ptr<RasterizerState> RenderFactory::CreateRasterizerState( const RasterizerStateDesc& desc )
{
	shared_ptr<RasterizerState> retVal;

	auto found = mRasterizerStatePool.find(desc);
	if (found == mRasterizerStatePool.end())
	{
		retVal = CreateRasterizerStateImpl(desc);
		mRasterizerStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}

shared_ptr<SamplerState> RenderFactory::CreateSamplerState( const SamplerStateDesc& desc )
{
	if (mSamplerStatePool.find(desc) == mSamplerStatePool.end())
		mSamplerStatePool.insert(std::make_pair(desc, CreateSamplerStateImpl(desc)));

	return mSamplerStatePool[desc];
}

shared_ptr<Shader> RenderFactory::CreateShader( ShaderType shaderType, const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	String fullShaderName = filename;
	for (const ShaderMacro& macro : macros)
	{
		fullShaderName += macro.first + macro.second;
	}
	fullShaderName += entryPoint;

	RenderDeviceType deviceType = Context::GetSingleton().GetRenderDevice().GetRenderDeviceType();

	if (mShaderPool.find(fullShaderName) == mShaderPool.end())
	{
		shared_ptr<Shader> shader = CreateShaderImpl(shaderType);

		if (mShaderBinaryHint)
		{
			// Try to load binary shader code
			String byteCodeFile = ShaderDirectory[deviceType][1] + fullShaderName + ShaderSuffix[deviceType][1];
			String filepath = FileSystem::GetSingleton().Locate(byteCodeFile);

			shader->LoadFromByteCode(filepath);
		}
		else
		{
			// Try to load binary shader code
			String byteCodeFile = ShaderDirectory[deviceType][0] + filename + ShaderSuffix[deviceType][0];
			String filepath = FileSystem::GetSingleton().Locate(byteCodeFile);

			shader->LoadFromFile(filepath, macros, entryPoint);
		}

		mShaderPool.insert(std::make_pair(fullShaderName, shader));
	}

	return mShaderPool[fullShaderName];
}


} // Namespace RcEngine
