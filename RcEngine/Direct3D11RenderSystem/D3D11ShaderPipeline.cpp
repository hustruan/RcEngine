#include "D3D11Shader.h"
#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "D3D11View.h"
#include <Graphics/EffectParameter.h>
#include <Graphics/Effect.h>
#include <Core/Exception.h>

namespace RcEngine {

#define HLSL_ARRAY_STRIDE (sizeof(float4))
#define HLSL_MATRIX_STRIDE (sizeof(float4))

template<typename T>
struct ShaderParameterCommit
{
	ShaderParameterCommit(EffectConstantBuffer* globalCB, EffectParameter* param, uint32_t offset) 
		: Param(param), GlobalCBuffer(globalCB), Offset(offset), UpdateTimeStamp(0) {}
	
	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			T value;  Param->GetValue(value);
	
			*(reinterpret_cast<T*>(GlobalCBuffer->GetRawData(Offset))) = value;
			
			GlobalCBuffer->MakeDirty();
			UpdateTimeStamp = Param->GetTimeStamp();
		} 
	}
	
private:
	uint32_t Offset;
	EffectParameter* Param;
	EffectConstantBuffer* GlobalCBuffer;
	TimeStamp UpdateTimeStamp;
};
	
template<>
struct ShaderParameterCommit<float4x4>
{
	ShaderParameterCommit(EffectConstantBuffer* globalCB, EffectParameter* param, uint32_t offset) 
		: Param(param), GlobalCBuffer(globalCB), Offset(offset), UpdateTimeStamp(0) {}
	
	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4 pValue;
			Param->GetValue(pValue);
	
			*(reinterpret_cast<float4x4*>(GlobalCBuffer->GetRawData(Offset))) = pValue.Transpose();	
	
			GlobalCBuffer->MakeDirty();
			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}
	
private:
	uint32_t Offset;
	EffectParameter* Param;
	EffectConstantBuffer* GlobalCBuffer;
	TimeStamp UpdateTimeStamp;
};
	
template<typename T>
struct ShaderParameterCommit<T*>
{
	ShaderParameterCommit(EffectConstantBuffer* globalCB, EffectParameter* param, uint32_t offset) 
		: Param(param), GlobalCBuffer(globalCB), Offset(offset), UpdateTimeStamp(0) {}
	
	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			T* pValue;
			Param->GetValue(pValue);
	
			uint8_t* pData = GlobalCBuffer->GetRawData(Offset);
			for (uint32_t i = 0; i < Param->GetElementSize(); ++i)
			{
				*(reinterpret_cast<T*>(pData + HLSL_ARRAY_STRIDE)) = pValue[i]; 
			}
	
			GlobalCBuffer->MakeDirty();
			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}
	
private:
	uint32_t Offset;
	EffectParameter* Param;
	EffectConstantBuffer* GlobalCBuffer;
	TimeStamp UpdateTimeStamp;
};
	
template<>
struct ShaderParameterCommit<float4x4*>
{
	ShaderParameterCommit(EffectConstantBuffer* globalCB, EffectParameter* param, uint32_t offset) 
		: Param(param), GlobalCBuffer(globalCB), Offset(offset), UpdateTimeStamp(0) {}
	
	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4* pValue;
			Param->GetValue(pValue);
	
			float4x4* pMatrix = reinterpret_cast<float4x4*>( GlobalCBuffer->GetRawData(Offset) );
			for (uint32_t i = 0; i < Param->GetElementSize(); ++i)
			{
				pMatrix[i] = pValue[i].Transpose();		
			}
	
			GlobalCBuffer->MakeDirty();
			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}
	
private:
	uint32_t Offset;
	EffectParameter* Param;
	EffectConstantBuffer* GlobalCBuffer;
	TimeStamp UpdateTimeStamp;
};

struct SRVBindHelper
{
	SRVBindHelper(EffectParameter* param, uint32_t binding, ShaderType shaderStage)
		: Param(param), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		weak_ptr<ShaderResourceView> srv;
		Param->GetValue(srv);

		if (auto spt = srv.lock())
		{
			ID3D11ShaderResourceView* srvD3D11 = static_cast_checked<D3D11ShaderResouceView*>(spt.get())->ShaderResourceViewD3D11;;

			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;

			switch (ShaderStage)
			{
			case ST_Vertex:
				deviceContext->VSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_TessControl:
				deviceContext->HSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_TessEval:
				deviceContext->DSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Geomerty:
				deviceContext->GSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Pixel:
				deviceContext->PSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Compute:
				deviceContext->CSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			default:
				break;
			}
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
	ShaderType ShaderStage;
};

struct UAVBindHelper
{
	UAVBindHelper(EffectParameter* param, uint32_t binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		weak_ptr<UnorderedAccessView> uav;
		Param->GetValue(uav);

		if (auto spt = uav.lock())
		{
			ID3D11UnorderedAccessView* uavD3D11 = static_cast_checked<D3D11UnorderedAccessView*>(spt.get())->UnorderedAccessViewD3D11;;

			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;
			deviceContext->CSSetUnorderedAccessViews(Binding, 1, &uavD3D11, nullptr);
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
};

struct SamplerBindHelper
{
	SamplerBindHelper(EffectParameter* param, uint32_t binding, ShaderType shaderStage)
		: Param(param), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		weak_ptr<SamplerState> sampler;
		Param->GetValue(sampler);

		if (auto spt = sampler.lock())
		{
			ID3D11SamplerState* samplerStateD3D11 = static_cast_checked<D3D11SamplerState*>(spt.get())->StateD3D11;;

			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;

			switch (ShaderStage)
			{
			case ST_Vertex:
				deviceContext->VSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_TessControl:
				deviceContext->HSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_TessEval:
				deviceContext->DSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Geomerty:
				deviceContext->GSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Pixel:
				deviceContext->PSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Compute:
				deviceContext->CSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			default:
				break;
			}
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
	ShaderType ShaderStage;
};

struct UniformBindHelper
{
	UniformBindHelper(EffectConstantBuffer* buffer, uint32_t binding, ShaderType shaderStage)
		: Buffer(buffer), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		// Update uniform buffer if changed
		Buffer->UpdateBuffer();

		ID3D11Buffer* bufferD3D11 = static_cast_checked<D3D11Buffer*>(Buffer->GetBuffer().get())->BufferD3D11;

		ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;
		switch (ShaderStage)
		{
		case ST_Vertex:
			deviceContext->VSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_TessControl:
			deviceContext->HSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_TessEval:
			deviceContext->DSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Geomerty:
			deviceContext->GSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Pixel:
			deviceContext->PSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Compute:
			deviceContext->CSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		default:
			break;
		}
	}

private:
	EffectConstantBuffer* Buffer;
	uint32_t Binding;
	ShaderType ShaderStage;
};

//////////////////////////////////////////////////////////////////////////
D3D11ShaderPipeline::D3D11ShaderPipeline(Effect& effect)
	: ShaderPipeline(effect)
{

}

bool D3D11ShaderPipeline::LinkPipeline()
{
	for (int i = 0; i < ST_Count; ++i)
	{
		if (mShaderStages[i])
		{
			D3D11Shader* shaderD3D11 = static_cast_checked<D3D11Shader*>(mShaderStages[i].get());

			EffectConstantBuffer* constantBuffer = nullptr;

			// Constant Buffer
			for (const auto& cbufferParam : shaderD3D11->ConstantBufferParams)
			{
				// Fetch a constant buffer if not exits
				if (cbufferParam.Name == "$Globals")
				{
					for (const auto& bufferVariable : cbufferParam.BufferVariables)
					{
						EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);				
						
						if (bufferVariable.ArraySize > 1)
							variable->SetArrayStride(HLSL_ARRAY_STRIDE); // always float4

						if (bufferVariable.Type >= EPT_Matrix2x2 && bufferVariable.Type <= EPT_Matrix4x4)
							variable->SetMatrixStride(HLSL_MATRIX_STRIDE);  // always float4

						AddGlobalUniformBind(shaderD3D11->GlobalCBuffer, variable, bufferVariable.Offset, bufferVariable.ArraySize);
					}

					constantBuffer = shaderD3D11->GlobalCBuffer;
				}
				else
				{
					constantBuffer = mEffect.FetchConstantBuffer(cbufferParam.Name, cbufferParam.BufferSize);
					if (constantBuffer->GetNumVariables() > 0)
					{
						assert(constantBuffer->GetNumVariables() == cbufferParam.BufferVariables.size());
						for (size_t i = 0; i < cbufferParam.BufferVariables.size(); ++i)
						{
							EffectParameter* variable = constantBuffer->GetVariable(i);
							if (variable->GetName() != cbufferParam.BufferVariables[i].Name ||
								variable->GetParameterType() != cbufferParam.BufferVariables[i].Type ||
								variable->GetElementSize() != cbufferParam.BufferVariables[i].ArraySize ||
								variable->GetOffset() != cbufferParam.BufferVariables[i].Offset)
							{
								ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Error: Same uniform buffer with different variables!", "D3D11ShaderPipeline::LinkPipeline");
							}
						}
					}
					else
					{
						for (const auto& bufferVariable : cbufferParam.BufferVariables)
						{
							EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);
							constantBuffer->AddVariable(variable, bufferVariable.Offset);

							if (bufferVariable.ArraySize > 1)
								variable->SetArrayStride(HLSL_ARRAY_STRIDE); // always float4

							if (bufferVariable.Type >= EPT_Matrix2x2 && bufferVariable.Type <= EPT_Matrix4x4)
								variable->SetMatrixStride(HLSL_MATRIX_STRIDE);  // always float4
						}
					}
				}

				mParameterBinds.push_back( UniformBindHelper(constantBuffer, cbufferParam.Binding, shaderD3D11->GetShaderType()));
			}

			// Add shader resouce bind
			for (const auto& param : shaderD3D11->ResourceInputParams)
			{
				switch (param.Class)
				{
				case Shader_Param_SRV:
					{
						EffectParameter* effectParam = mEffect.FetchSRVParameter(param.Name, param.Type);
						mParameterBinds.push_back( SRVBindHelper(effectParam, param.Binding, shaderD3D11->GetShaderType()) );
					}
					break;
				case Shader_Param_UAV:
					{
						// Only Compute has UAV
						if (mShaderStages[i]->GetShaderType() == ST_Compute)
						{
							EffectParameter* effectParam = mEffect.FetchUAVParameter(param.Name, param.Type);
							mParameterBinds.push_back( UAVBindHelper(effectParam, param.Binding) );
						}
					}
					break;
				case Shader_Param_Sampler:
					{
						EffectParameter* effectParam = mEffect.FetchSamplerParameter(param.Name);
						mParameterBinds.push_back( SamplerBindHelper(effectParam, param.Binding, shaderD3D11->GetShaderType()) );
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return true;
}

void D3D11ShaderPipeline::OnBind()
{
	// Commit all shader resource
	for ( auto& paramBindCommit : mParameterBinds) 
		paramBindCommit();
}

void D3D11ShaderPipeline::OnUnbind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mShaderStages[ST_Vertex])			deviceContextD3D11->VSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_TessControl])      deviceContextD3D11->HSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_TessEval])			deviceContextD3D11->DSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Geomerty])			deviceContextD3D11->GSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Pixel])			deviceContextD3D11->PSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Compute])			deviceContextD3D11->CSSetShader(nullptr, nullptr, 0);
}

void D3D11ShaderPipeline::AddGlobalUniformBind( EffectConstantBuffer* cbuffer, EffectParameter* effectParam, uint32_t offset, uint32_t arrSize )
{
#define Add_ShaderParam_Commit(type, cbuffer, effectParam, offset) \
	if (arrSize > 1) mParameterBinds.push_back( ShaderParameterCommit<type*>(cbuffer, effectParam, offset) ); \
	else mParameterBinds.push_back( ShaderParameterCommit<type>(cbuffer, effectParam, offset) ); 

	switch(effectParam->GetParameterType())
	{
	case EPT_Boolean:	
		{
			assert(arrSize <= 1);
			mParameterBinds.push_back( ShaderParameterCommit<bool>(cbuffer, effectParam, offset) ); 
		}
		break;
	case EPT_Int:		{ Add_ShaderParam_Commit(int, cbuffer, effectParam, offset) } break;
	case EPT_Int2:		{ Add_ShaderParam_Commit(int2, cbuffer, effectParam, offset) } break;
	case EPT_Int3:		{ Add_ShaderParam_Commit(int3, cbuffer, effectParam, offset) } break;
	case EPT_Int4:		{ Add_ShaderParam_Commit(int4, cbuffer, effectParam, offset) } break;
	case EPT_UInt:		{ Add_ShaderParam_Commit(uint32_t, cbuffer, effectParam, offset) } break;
	case EPT_UInt2:		{ Add_ShaderParam_Commit(uint2, cbuffer, effectParam, offset) } break;
	case EPT_UInt3:		{ Add_ShaderParam_Commit(uint3, cbuffer, effectParam, offset) } break;
	case EPT_UInt4:		{ Add_ShaderParam_Commit(uint4, cbuffer, effectParam, offset) } break;
	case EPT_Float:		{ Add_ShaderParam_Commit(float, cbuffer, effectParam, offset) } break;
	case EPT_Float2:	{ Add_ShaderParam_Commit(float2, cbuffer, effectParam, offset) } break;
	case EPT_Float3:	{ Add_ShaderParam_Commit(float3, cbuffer, effectParam, offset) } break;
	case EPT_Float4:	{ Add_ShaderParam_Commit(float4, cbuffer, effectParam, offset) } break;
	case EPT_Matrix4x4: { Add_ShaderParam_Commit(float4x4, cbuffer, effectParam, offset) } break;
	default:
		assert(false);
	}

#undef Add_ShaderParam_Commit
}

}