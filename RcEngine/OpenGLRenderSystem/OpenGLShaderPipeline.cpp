#include "OpenGLShader.h"
#include "OpenGLDevice.h"
#include "OpenGLView.h"
#include "OpenGLBuffer.h"
#include <Graphics/EffectParameter.h>
#include <Graphics/Effect.h>
#include <Core/Exception.h>
#include <Core/Loger.h>

namespace RcEngine {

struct glProgramUniform
{
	inline static void Commit(GLuint shader, GLint loc, bool v)				{ glProgramUniform1i(shader, loc, v); }
					   
	inline static void Commit(GLuint shader, GLint loc,     int32_t v)		{ glProgramUniform1i(shader, loc, v); }
	inline static void Commit(GLuint shader, GLint loc, const int2& v)		{ glProgramUniform2iv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const int3& v)		{ glProgramUniform3iv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const int4& v)		{ glProgramUniform4iv(shader, loc, 1, v()); }
	 				   
	inline static void Commit(GLuint shader, GLint loc, uint32_t v)			{ glProgramUniform1ui(shader, loc, v); }
	inline static void Commit(GLuint shader, GLint loc, const uint2& v)		{ glProgramUniform2uiv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const uint3& v)		{ glProgramUniform3uiv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const uint4& v)		{ glProgramUniform4uiv(shader, loc, 1, v()); }
	 				   
	inline static void Commit(GLuint shader, GLint loc, float v)			{ glProgramUniform1f(shader, loc, v); }
	inline static void Commit(GLuint shader, GLint loc, const float2& v)	{ glProgramUniform2fv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const float3& v)	{ glProgramUniform3fv(shader, loc, 1, v()); }
	inline static void Commit(GLuint shader, GLint loc, const float4& v)	{ glProgramUniform4fv(shader, loc, 1, v()); }

	inline static void Commit(GLuint shader, GLint loc, const float4x4& v)	{ glProgramUniformMatrix4fv(shader, loc, 1, true, v()); }

	// Array
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, int32_t* v)		  { glProgramUniform1iv(shader, loc, count, v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const int2* v)	  { glProgramUniform2iv(shader, loc, count, (const int32_t*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const int3* v)	  { glProgramUniform3iv(shader, loc, count, (const int32_t*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const int4* v)	  { glProgramUniform4iv(shader, loc, count, (const int32_t*)v); }

	inline static void Commit(GLuint shader, GLint loc, GLsizei count, uint32_t* v)		  { glProgramUniform1uiv(shader, loc, count, v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const uint2* v)	  { glProgramUniform2uiv(shader, loc, count, (const uint32_t*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const uint3* v)	  { glProgramUniform3uiv(shader, loc, count, (const uint32_t*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const uint4* v)	  { glProgramUniform4uiv(shader, loc, count, (const uint32_t*)v); }
												 		
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, float* v)		  { glProgramUniform1fv(shader, loc, count, v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const float2* v)	  { glProgramUniform2fv(shader, loc, count, (const float*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const float3* v)	  { glProgramUniform3fv(shader, loc, count, (const float*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const float4* v)	  { glProgramUniform4fv(shader, loc, count, (const float*)v); }
	inline static void Commit(GLuint shader, GLint loc, GLsizei count, const float4x4* v) { glProgramUniformMatrix4fv(shader, loc, count, true, (const float*)(v)); }
};

template <typename T>
struct ShaderParameterCommit
{
	ShaderParameterCommit(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			T value;  Param->GetValue(value);
			glProgramUniform::Commit(ShaderOGL, Location, value);
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <typename T>
struct ShaderParameterArrayCommit
{
public:
	ShaderParameterArrayCommit(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), Count(count), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			T pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform::Commit(ShaderOGL, Location, Count, pValue);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template<>
struct ShaderParameterCommit< EffectConstantBuffer >
{
public:
	ShaderParameterCommit(EffectConstantBuffer* param, GLuint bindingSlot)
		: BindingSlot(bindingSlot), UniformBlock(param) { }

	void operator() ()
	{
		// Update uniform buffer if changed
		UniformBlock->UpdateBuffer();

		GLuint bufferOGL = (static_cast_checked<OpenGLBuffer*>(UniformBlock->GetBuffer().get()))->GetBufferOGL();
		glBindBufferBase(GL_UNIFORM_BUFFER, BindingSlot, bufferOGL);
	}

private:
	GLuint BindingSlot;
	EffectConstantBuffer* UniformBlock;
};

template<>
struct ShaderParameterCommit<ShaderResourceView>
{
	ShaderParameterCommit(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<ShaderResourceView> srv;
			Param->GetValue(srv);

			if (auto spt = srv.lock())
			{
				OpenGLShaderResourceView* srvOGL = static_cast_checked<OpenGLShaderResourceView*>(spt.get());
				srvOGL->BindSRV(Binding);
			}
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
	TimeStamp UpdateTimeStamp;
};

template<>
struct ShaderParameterCommit<UnorderedAccessView>
{
	ShaderParameterCommit(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<UnorderedAccessView> uav;
			Param->GetValue(uav);

			if (auto spt = uav.lock())
			{
				OpenGLUnorderedAccessView* srvOGL = static_cast_checked<OpenGLUnorderedAccessView*>(spt.get());
				srvOGL->BindUAV(Binding);
			}
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
	TimeStamp UpdateTimeStamp;
};

template<>
struct ShaderParameterCommit<SamplerState>
{
	ShaderParameterCommit(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		weak_ptr<SamplerState> sampler;
		Param->GetValue(sampler);

		if (auto spt = sampler.lock())
		{
			gOpenGLDevice->SetSamplerState(ST_Pixel /*Use used in OpenGL*/, Binding, spt);
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
};

// Commit for set binding slot to shader program
struct ShaderResourceBinding
{
	enum BindType { UniformBlock, ShaderStorage, General };

	ShaderResourceBinding(BindType type, GLuint shaderID, GLint location, GLuint binding)
		: Type(type), ShaderOGL(shaderID), Location(location), Binding(binding) {}

	void operator() ()
	{
		switch (Type)
		{
		case UniformBlock:
			glUniformBlockBinding(ShaderOGL, Location, Binding);
			break;
		case ShaderStorage:
			glShaderStorageBlockBinding(ShaderOGL, Location, Binding);
			break;
		case General:
		default:
			glProgramUniform1i(ShaderOGL, Location, Binding);
			break;
		}
	}

private:
	BindType Type;

	GLuint ShaderOGL;
	GLuint Binding;
	GLuint Location;
};

//////////////////////////////////////////////////////////////////////////
OpenGLShaderPipeline::OpenGLShaderPipeline(Effect& effect)
	: ShaderPipeline(effect)
{
	glGenProgramPipelines(1, &mPipelineOGL);
}

OpenGLShaderPipeline::~OpenGLShaderPipeline()
{
	glDeleteProgramPipelines(1, &mPipelineOGL);
}

void OpenGLShaderPipeline::OnBind()
{
	// Commit all shader parameter
	for (auto& paramBindFunc : mParameterBinds)
		paramBindFunc();
}

void OpenGLShaderPipeline::OnUnbind()
{
	//glBindProgramPipeline(0);
}

bool OpenGLShaderPipeline::LinkPipeline()
{
	GLuint srvBinding = 0;
	GLuint uavBinding = 0;
	GLuint uniformBlockBinding = 0;
	//GLuint shaderStorageBinidng = 0;
	std::map<String, GLuint> mBindingCache;

	for (int i = 0; i < ST_Count; ++i)
	{
		if (mShaderStages[i])
		{
			OpenGLShader* shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[i].get()));

			// Uniform buffers
			for (const UniformBuffer& uniformBlock : shaderOGL->mUniformBuffers)
			{
				if (uniformBlock.Location == -1) // Global uniforms
				{
					for (const auto& globalUniform : uniformBlock.BufferVariables)
					{
						EffectParameter* effectParam = mEffect.FetchUniformParameter(globalUniform.Name, globalUniform.Type, globalUniform.ArraySize);
						AddUniformParamBind(shaderOGL->mShaderOGL, globalUniform.Location, effectParam, globalUniform.ArraySize);
					}
				}
				else
				{
					EffectConstantBuffer* uniformBuffer = mEffect.FetchConstantBuffer(uniformBlock.Name, uniformBlock.BufferSize);

					if (uniformBuffer->GetNumVariables() > 0) // Already created
					{
						// check buffer variables
						assert(uniformBuffer->GetNumVariables() == uniformBlock.BufferVariables.size());
						for (size_t i = 0; i < uniformBlock.BufferVariables.size(); ++i)
						{
							EffectParameter* variable = uniformBuffer->GetVariable(i);
							if (variable->GetName() != uniformBlock.BufferVariables[i].Name				||
								variable->GetParameterType() != uniformBlock.BufferVariables[i].Type	||
								variable->GetElementSize() != uniformBlock.BufferVariables[i].ArraySize ||
								variable->GetOffset() != uniformBlock.BufferVariables[i].Offset)
							{
								ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Error: Same uniform buffer with different variables!", "D3D11ShaderPipeline::LinkPipeline");
							}
						}
					}
					else
					{
						for (const auto& bufferVariable : uniformBlock.BufferVariables)
						{
							EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);
							uniformBuffer->AddVariable(variable, bufferVariable.Offset);

							if (bufferVariable.ArraySize > 1)
								variable->SetArrayStride(bufferVariable.ArrayStride);

							if (bufferVariable.Type >= EPT_Matrix2x2 && bufferVariable.Type <= EPT_Matrix4x4)
								variable->SetMatrixStride(sizeof(float4));  // always float4
						}
					}

					// Commit for binding uniform buffer to UBO binding slot 
					if (mBindingCache.find(uniformBlock.Name) == mBindingCache.end())
					{
						mBindingCache[uniformBlock.Name] = uniformBlockBinding++;
						AddUnitformBlockBind(uniformBuffer, mBindingCache[uniformBlock.Name]);
					}
					
					// Commit for set binding slot to shader context
					AddShaderResourceBind(shaderOGL->mShaderOGL, uniformBlock.Location, mBindingCache[uniformBlock.Name], ShaderResourceBinding::UniformBlock);
				}
			}

			// Shader resource views
			for (const ResouceViewParam& viewParam : shaderOGL->mBoundResources)
			{
				if (mBindingCache.find(viewParam.Name) == mBindingCache.end())
				{		
					// Bind resource to OpenGL binding slot, Texture/Image unit or SSBO
					if (viewParam.ViewClass == Shader_Param_SRV)
					{
						mBindingCache[viewParam.Name] = srvBinding++;

						EffectParameter* srvParam = mEffect.FetchSRVParameter(viewParam.Name, viewParam.Type);
						AddSRVResouceBind(srvParam, mBindingCache[viewParam.Name]);
					}
					else
					{
						mBindingCache[viewParam.Name] = uavBinding++;

						EffectParameter* uavParam = mEffect.FetchUAVParameter(viewParam.Name, viewParam.Type);
						AddUAVResourceBind(uavParam, mBindingCache[viewParam.Name]);
					}
				}

				// Set binding point to shader uniform
				if (viewParam.Type == EPT_StructureBuffer)
					AddShaderResourceBind(shaderOGL->mShaderOGL, viewParam.Location, mBindingCache[viewParam.Name], ShaderResourceBinding::ShaderStorage);
				else
					AddShaderResourceBind(shaderOGL->mShaderOGL, viewParam.Location, mBindingCache[viewParam.Name], ShaderResourceBinding::General);
			}

			// SamplerState
			for (auto& kv : shaderOGL->mSamplerStates)
			{
				EffectParameter* effectParam = mEffect.FetchSamplerParameter(kv.second);
				mParameterBinds.push_back( ShaderParameterCommit<SamplerState>(effectParam, mBindingCache[kv.first]) );
			}
		}
	}

	// Link all stage into pipeline
	if (mShaderStages[ST_Vertex])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Vertex].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_VERTEX_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_TessControl])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_TessControl].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_TESS_CONTROL_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_TessEval])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_TessEval].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_TESS_EVALUATION_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Geomerty])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Geomerty].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_GEOMETRY_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Pixel])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Pixel].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_FRAGMENT_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Compute])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Compute].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_COMPUTE_SHADER_BIT, shaderOGL);
	}

	return true;
}

void OpenGLShaderPipeline::AddUniformParamBind( GLuint shader, GLint location, EffectParameter* effectParam, GLsizei arrSize )
{
	switch(effectParam->GetParameterType())
	{
	case EPT_Boolean:
		{
			assert(arrSize <= 1);
			mParameterBinds.push_back( ShaderParameterCommit<bool>(shader, location, effectParam) );
		}
		break;
	case EPT_Int:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<int32_t*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<int32_t>(shader, location, effectParam) );
		}
		break;
	case EPT_Int2:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<int2*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<int2>(shader, location, effectParam) );
		}
		break;
	case EPT_Int3:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<int3*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<int3>(shader, location, effectParam) );
		}
		break;
	case EPT_Int4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<int4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<int4>(shader, location, effectParam) );
		}
		break;
	case EPT_UInt:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<uint32_t*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<uint32_t>(shader, location, effectParam) );
		}
		break;
	case EPT_UInt2:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<uint2*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<uint2>(shader, location, effectParam) );
		}
		break;
	case EPT_UInt3:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<uint3*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<uint3>(shader, location, effectParam) );
		}
		break;
	case EPT_UInt4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<uint4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<uint4>(shader, location, effectParam) );
		}
		break;
	case EPT_Float:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<float*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<float>(shader, location, effectParam) );
		}
		break;
	case EPT_Float2:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<float2*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<float2>(shader, location, effectParam) );	
		}
		break;
	case EPT_Float3:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<float3*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<float3>(shader, location, effectParam) );	
		}
		break;
	case EPT_Float4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<float4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<float4>(shader, location, effectParam) );		
		}
		break;
	case EPT_Matrix4x4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterArrayCommit<float4x4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterCommit<float4x4>(shader, location, effectParam) );		
		}
		break;
	default:
		assert(false);
	}
}

void OpenGLShaderPipeline::AddUnitformBlockBind( EffectConstantBuffer* effectCBuffer, GLuint binding )
{
	mParameterBinds.push_back(ShaderParameterCommit<EffectConstantBuffer>(effectCBuffer, binding));
}

void OpenGLShaderPipeline::AddSRVResouceBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterCommit<ShaderResourceView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddUAVResourceBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterCommit<UnorderedAccessView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddShaderResourceBind( GLuint shader, GLint location, GLuint binding, GLuint bindType )
{
	mParameterBinds.push_back( ShaderResourceBinding(ShaderResourceBinding::BindType(bindType), shader, location, binding) );
}


}