#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Resource/ResourceManager.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <Core/Environment.h>
#include <Core/XMLDom.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Graphics/GraphicsScriptLoader.h>

namespace RcEngine {

Effect::Effect( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Effect, creator, handle, name, group), mCurrTechnique(nullptr)
{

}

Effect::~Effect()
{
	for (auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
		delete (iter->second);

	for (auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
		delete *iter;

	for (auto iter = mConstantBuffers.begin(); iter != mConstantBuffers.end(); ++iter)
		delete *iter;
}

EffectTechnique* Effect::GetTechniqueByIndex( uint32_t index ) const
{
	assert(index >=0 && index < mTechniques.size());
	return mTechniques[index];
}

EffectTechnique* Effect::GetTechniqueByName( const String& techName ) const
{
	for (EffectTechnique* technique : mTechniques)
	{
		if (technique->mName == techName)
			return technique;
	}

	return nullptr;
}

void Effect::SetCurrentTechnique( const String& techName )
{
	std::vector<EffectTechnique*>::const_iterator it;

	it = std::find_if(mTechniques.begin(), mTechniques.end(), [&](EffectTechnique* tech) { 
						return tech->GetTechniqueName() == techName;} );
	
	if (it == mTechniques.end())
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, techName + " deosn't exit in" + mEffectName, "Effect::SetCurrentTechnique");

	mCurrTechnique = *it;
}

void Effect::SetCurrentTechnique( uint32_t index )
{
	assert(index < mTechniques.size());
	mCurrTechnique = mTechniques[index];	
}

EffectParameter* Effect::GetParameterByName( const String& paraName ) const
{
	std::map<String, EffectParameter*>::const_iterator it;
	
	it = mParameters.find(paraName);
	return (it != mParameters.end()) ? it->second : nullptr;
}

EffectParameter* Effect::GetParameterByUsage( EffectParameterUsage usage ) const
{
	for (auto& kv : mParameters)
	{
		if (kv.second->GetParameterUsage() == usage)
			return kv.second;
	}
	
	return nullptr;
}

EffectParameter* Effect::FetchSRVParameter( const String& name, EffectParameterType effectType )
{
	switch (effectType)
	{
	case EPT_Texture1D:
	case EPT_Texture2D:
	case EPT_Texture3D:
	case EPT_TextureCube:
	case EPT_Texture1DArray:
	case EPT_Texture2DArray:
	case EPT_Texture3DArray:
	case EPT_TextureCubeArray:
	case EPT_TextureBuffer:
	case EPT_StructureBuffer:
		{
			EffectParameter* srvParam;

			if (mParameters.find(name) == mParameters.end())
			{
				srvParam = new EffectSRVParameter(name, effectType);
				mParameters[name] = srvParam;
			}
			else
			{
				srvParam = mParameters[name];
				assert(srvParam->GetParameterType() == effectType);
			}

			return srvParam;
		}
		break;
	default:
		assert(false);
	}

	return NULL;
}

EffectParameter* Effect::FetchUAVParameter( const String& name, EffectParameterType effectType )
{
	switch (effectType)
	{
	case EPT_Texture1D:
	case EPT_Texture2D:
	case EPT_Texture3D:
	case EPT_TextureCube:
	case EPT_Texture1DArray:
	case EPT_Texture2DArray:
	case EPT_Texture3DArray:
	case EPT_TextureCubeArray:
	case EPT_TextureBuffer:
	case EPT_StructureBuffer:
		{
			EffectParameter* uavParam;

			if (mParameters.find(name) == mParameters.end())
			{
				uavParam = new EffectUAVParameter(name, effectType);
				mParameters[name] = uavParam;
			}
			else
			{
				uavParam = mParameters[name];
				assert(uavParam->GetParameterType() == effectType);
			}

			return uavParam;
		}
		break;
	default:
		assert(false);
	}

	return NULL;
}

EffectParameter* Effect::FetchUniformParameter( const String& name, EffectParameterType type, uint32_t elementSize )
{
	std::map<String, EffectParameter*>::const_iterator it = mParameters.find(name);
	
	EffectParameter* uniformParam;

	if (it != mParameters.end())
	{
		uniformParam = it->second;

		assert(uniformParam->GetParameterType() == type);
		assert(uniformParam->GetElementSize() == elementSize);

		return uniformParam;
	}

	// Not exit, create one
	switch(type)
	{
	case EPT_Boolean:
		{
			assert(elementSize <= 1);
			uniformParam = new EffectParameterBool(name, type);
		}
		break;
	case EPT_Float:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterFloatArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterFloat(name, type);
		}
		break;
	case EPT_Float2:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector2Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector2(name, type);
		}
		break;
	case EPT_Float3:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector3Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector3(name, type);
		}
		break;
	case EPT_Float4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector4Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector4(name, type);
		}
		break;
	case EPT_Int:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterIntArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt(name, type);
		}
		break;
	case EPT_Int2:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt2Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt2(name, type);
		}
		break;
	case EPT_Int3:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt3Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt3(name, type);
		}
		break;
	case EPT_Int4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt4Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt4(name, type);
		}
		break;
	case EPT_UInt:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterUIntArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterUInt(name, type);
		}
		break;
	case EPT_UInt2:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterUInt2Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterUInt2(name, type);
		}
		break;
	case EPT_UInt3:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterUInt3Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterUInt3(name, type);
		}
		break;
	case EPT_UInt4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterUInt4Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterUInt4(name, type);
		}
		break;
	case EPT_Matrix4x4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterMatrixArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterMatrix(name, type);
		}
		break;
	default:
		assert(false);
	}

	mParameters[name] = uniformParam;

	return uniformParam;
}

EffectParameter* Effect::FetchSamplerParameter( const String& name )
{
	EffectParameter* samplerParam;

	if (mParameters.find(name) == mParameters.end())
	{
		samplerParam = new EffectSamplerParameter(name);
		mParameters[name] = samplerParam;
	}
	else
	{
		samplerParam = mParameters[name];
	}

	return samplerParam;
}

EffectConstantBuffer* Effect::FetchConstantBuffer( const String& name, uint32_t bufferSize )
{
	for (EffectConstantBuffer* buffer : mConstantBuffers)
	{
		if (buffer->GetName() == name && buffer->GetBufferSize() == bufferSize)
			return buffer;
	}

	EffectConstantBuffer* buffer = new EffectConstantBuffer(name, bufferSize);
	mConstantBuffers.push_back(buffer);

	return buffer;
}

EffectConstantBuffer* Effect::CreateConstantBuffer( const String& name, uint32_t bufferSize )
{
	EffectConstantBuffer* buffer = new EffectConstantBuffer(name, bufferSize);
	mConstantBuffers.push_back(buffer);

	return buffer;
}

void Effect::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	// Effect flags used to build shader macro
	vector<String> effectFlags;

	// Split the effect name to get effect file and flags
	StringStream iss(mResourceName); 
	do 
	{ 
		String sub; 
		iss >> sub; 
		if (!sub.empty())
			effectFlags.push_back(sub);	
	} while (iss); 

	shared_ptr<Stream> effectStream = fileSystem.OpenStream(effectFlags[0], mGroup);
	Stream& source = *effectStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	// Effect name 
	mEffectName = root->AttributeString("name", "");

	// Parse techniques
	XMLNodePtr technqueNode, passNode, shaderNode;
	for (technqueNode = root->FirstNode("Technique");  technqueNode; technqueNode = technqueNode->NextSibling("Technique"))
	{
		EffectTechnique* technique = new EffectTechnique(*this);
		technique->mName = technqueNode->AttributeString("name", "");
		
		String filename, entryPoint;
		for (passNode = technqueNode->FirstNode("Pass");  passNode; passNode = passNode->NextSibling("Pass"))
		{
			EffectPass* pass = new EffectPass;
			pass->mName = passNode->AttributeString("name", "");
			pass->mShaderPipeline = factory->CreateShaderPipeline(*this);

			bool hasComputeShader = false;

			// Load shader 
			static const String ShaderNodeNames[] = {"VertexShader", "TessControlShader", "TessEvalShader", "GeometryShader", "PixelShader", "ComputeShader"};
			for (uint32_t i = 0; i < ST_Count; ++i)
			{
				if (shaderNode = passNode->FirstNode(ShaderNodeNames[i]))
				{
					filename = shaderNode->AttributeString("file", "");
					entryPoint = shaderNode->AttributeString("entry", "");

					vector<ShaderMacro> shaderMacros;
					Internal::CollectShaderMacro(shaderNode, shaderMacros);

					for (size_t j = 1; j < effectFlags.size(); ++j)
					{
						ShaderMacro macro = { effectFlags[j], "" };
						shaderMacros.push_back(macro);
					}

					pass->mShaderPipeline->AttachShader(
						factory->LoadShaderFromFile(
						ShaderType(ST_Vertex + i),
						filename, 
						shaderMacros.empty() ? nullptr : &shaderMacros[0],
						shaderMacros.size(),
						entryPoint) );

					// Compute pass
					if (i == ST_Compute)
						hasComputeShader = true;
				}
			}

			if (pass->mShaderPipeline->LinkPipeline() == false)
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Effect error!", "Effect::LoadForm");
			}

			// Compute shader pass has no render state
			if (!hasComputeShader)
			{
				DepthStencilStateDesc dsDesc;
				BlendStateDesc blendDesc;
				RasterizerStateDesc rasDesc;

				Internal::CollectRenderStates(passNode, dsDesc, blendDesc, rasDesc, pass->mBlendColor, pass->mSampleMask, pass->mFrontStencilRef, pass->mBackStencilRef);

				pass->mDepthStencilState = factory->CreateDepthStencilState(dsDesc);
				pass->mBlendState = factory->CreateBlendState(blendDesc);
				pass->mRasterizerState = factory->CreateRasterizerState(rasDesc);
			}

			technique->mPasses.push_back(pass);	
		}

		mTechniques.push_back(technique);
	}

	// Parse sampler states
	for (XMLNodePtr samplerNode = root->FirstNode("Sampler"); samplerNode; samplerNode = samplerNode->NextSibling("Sampler"))
	{
		String samplerName = samplerNode->AttributeString("name", "");
		EffectParameter* effectSamplerStateParam = GetParameterByName(samplerName);
		
		if (effectSamplerStateParam)
		{
			SamplerStateDesc desc;
			for (XMLNodePtr stateNode = samplerNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
			{	
				String stateName = stateNode->AttributeString("name", "");
				if (stateName == "Filter")
				{
					String value = stateNode->Attribute("value")->ValueString();
					desc.Filter = (TextureFilter)Internal::SamplerDefs::GetSingleton().GetSamplerState(value);
				}
				else if (stateName == "AddressU")
				{
					String value = stateNode->Attribute("value")->ValueString();
					desc.AddressU = (TextureAddressMode)Internal::SamplerDefs::GetSingleton().GetSamplerState(value);
				}
				else if (stateName == "AddressV")
				{
					String value = stateNode->Attribute("value")->ValueString();
					desc.AddressV = (TextureAddressMode)Internal::SamplerDefs::GetSingleton().GetSamplerState(value);
				}
				else if (stateName == "AddressW")
				{
					String value = stateNode->Attribute("value")->ValueString();
					desc.AddressW = (TextureAddressMode)Internal::SamplerDefs::GetSingleton().GetSamplerState(value);
				}
				else if (stateName == "MaxAnisotropy")
				{
					uint32_t value = stateNode->Attribute("value")->ValueUInt();
					if (value < 1 || value > 16)
						ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "MaxAnisotropy range invalid, only[1, 16] supported!",  "Material::LoadImpl");
					desc.MaxAnisotropy = value;
				}
				else if (stateName == "MinLOD")
				{
					float value = stateNode->Attribute("value")->ValueFloat();
					desc.MinLOD = value;
				}
				else if (stateName == "MaxLOD")
				{
					float value = stateNode->Attribute("value")->ValueFloat();
					desc.MaxLOD = value;
				}
				else if (stateName == "MipLODBias")
				{
					float value = stateNode->Attribute("value")->ValueFloat();
					desc.MipLODBias = value;
				}
				else if (stateName == "ComparisonFunc")
				{
					String value = stateNode->Attribute("value")->ValueString();
					desc.ComparisonFunc = (CompareFunction)Internal::SamplerDefs::GetSingleton().GetSamplerState(value);
					desc.CompareSampler = true;
				}
				else if (stateName == "BorderColor")
				{
					float r = stateNode->Attribute("r")->ValueFloat();
					float g = stateNode->Attribute("g")->ValueFloat();
					float b = stateNode->Attribute("b")->ValueFloat();
					float a = stateNode->Attribute("a")->ValueFloat();
					desc.BorderColor = ColorRGBA(r,g,b,a);
				}
				else
				{
					ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Unknown sampler state: " + stateName, 
						"Material::LoadFrom");
				}
			}

			shared_ptr<SamplerState> sampler = factory->CreateSamplerState(desc);
			mSamplerStates.insert( std::make_pair(samplerName, sampler) );

			effectSamplerStateParam->SetValue(mSamplerStates[samplerName]);
		}
	}

	// Auto-binding effect parameters
	for (XMLNodePtr paramNode = root->FirstNode("AutoBinding"); paramNode; paramNode = paramNode->NextSibling("AutoBinding"))
	{
		String paramName = paramNode->AttributeString("name", "");
		EffectParameter* effectParam = GetParameterByName(paramName);

		if (effectParam)
		{
			String semantic = paramNode->AttributeString("semantic", "");
			effectParam->mParameterUsage = Internal::EffectParamsUsageDefs::GetInstance().GetUsageType(semantic);

			// Validate effect type
		}
	}

	mCurrTechnique = mTechniques.front();
}

void Effect::UnloadImpl()
{

}

shared_ptr<Resource> Effect::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Effect>(creator, handle, name, group);
}

EffectConstantBuffer* Effect::GetConstantBuffer( const String& name ) const
{
	for (EffectConstantBuffer* cb : mConstantBuffers)
	{
		if (cb->GetName() == name)
			return cb;
	}

	ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "cbuffer: " + name + " not exits!", "Effect::GetConstantBuffer");
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

EffectTechnique::EffectTechnique(Effect& effect)
	: mEffect(effect),
	  mValid(true)
{

}

EffectTechnique::~EffectTechnique()
{
	for (EffectPass* pass : mPasses)
		delete pass;
}

EffectPass* EffectTechnique::GetPassByName( const String& name ) const
{
	for (EffectPass* pass : mPasses)
	{
		if (pass->GetPassName() == name)
			return pass;
	}

	return nullptr;
}

EffectPass* EffectTechnique::GetPassByIndex( uint32_t index ) const
{
	if (index >= 0 && index < mPasses.size())
		return mPasses[index];

	return nullptr;
}


//////////////////////////////////////////////////////////////////////////
EffectPass::EffectPass( )
	: mFrontStencilRef(0),
	  mBackStencilRef(0),
	  mBlendColor(0, 0, 0, 0),
	  mSampleMask(0xffffffff)
{

}


void EffectPass::BeginPass()
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();

	if (mDepthStencilState) device->SetDepthStencilState(mDepthStencilState, mFrontStencilRef, mBackStencilRef);
	if (mRasterizerState) device->SetRasterizerState(mRasterizerState);
	if (mBlendState) device->SetBlendState(mBlendState, mBlendColor, mSampleMask);
	device->BindShaderPipeline(mShaderPipeline);
}

void EffectPass::EndPass()
{
	//Environment::GetSingleton().GetRHDevice()->BindShaderPipeline(nullptr);
}


} // Namespace RcEngine