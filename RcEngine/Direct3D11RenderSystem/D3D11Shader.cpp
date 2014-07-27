#include "D3D11Shader.h"
#include "D3D11Device.h"
#include "D3D11Buffer.h"
#include "D3D11GraphicCommon.h"
#include <Graphics/EffectParameter.h>
#include <Core/Loger.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <fstream>

namespace RcEngine {

class D3D11ShaderReflection
{
public:
	D3D11ShaderReflection(D3D11Shader* shaderD3D11, void* bytecode, uint32_t size)
		: mShaderD3D11(shaderD3D11),
		  mReflectorD3D11(nullptr)
	{
		HRESULT hr;
		hr = D3DReflect( bytecode, size, IID_ID3D11ShaderReflection,  (void**) &mReflectorD3D11); 
		mReflectorD3D11->GetDesc( &mShaderDescD3D11 );
	}
	
	~D3D11ShaderReflection()
	{
		SAFE_RELEASE(mReflectorD3D11);
	}

	void ReflectShader()
	{
		if (mShaderD3D11->GetShaderType() == ST_Vertex)
			RefectInputParameters();

		ReflectConstantBuffers();
		RefectionBoundResources();
	}

	void RefectInputParameters()
	{
		D3D11VertexShader* vertexShaderD3D11 = static_cast_checked<D3D11VertexShader*>(mShaderD3D11);
		for ( UINT i = 0; i < mShaderDescD3D11.InputParameters; i++ ) 
		{ 
			D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc; 
			mReflectorD3D11->GetInputParameterDesc( i, &inputParamDesc ); 

			if (inputParamDesc.SystemValueType == D3D10_NAME_UNDEFINED)
			{
				InputSignature signature;
				signature.Semantic = inputParamDesc.SemanticName;
				signature.SemanticIndex = inputParamDesc.SemanticIndex;

				vertexShaderD3D11->InputSignatures.push_back(signature);
			}
			else
			{
				// System semantic, do not add to vertex input signature
			}
		} 
	}

	void ReflectConstantBuffers()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.ConstantBuffers; i++ ) 
		{
			ID3D11ShaderReflectionConstantBuffer* bufferD3D11 = mReflectorD3D11->GetConstantBufferByIndex( i ); 

			D3D11_SHADER_BUFFER_DESC bufferDesc;
			bufferD3D11->GetDesc(&bufferDesc);

			if (bufferDesc.Type != D3D11_CT_CBUFFER)
				continue; // Only collect constant buffer
			
			// Add new cbuffer
			mShaderD3D11->ConstantBufferParams.resize(mShaderD3D11->ConstantBufferParams.size() + 1);
			ConstantBuffer& cbufferParam = mShaderD3D11->ConstantBufferParams.back();

			cbufferParam.Name = bufferDesc.Name;
			cbufferParam.BufferSize = bufferDesc.Size; 
			cbufferParam.Binding = i;

			if (cbufferParam.Name == "$Globals") 
				mShaderD3D11->CreateGlobalConstantBuffer(bufferDesc.Size);

			cbufferParam.BufferVariables.resize(bufferDesc.Variables);
			for ( UINT j = 0; j < bufferDesc.Variables; j++ ) 
			{
				ID3D11ShaderReflectionVariable* variableD3D11 = bufferD3D11->GetVariableByIndex( j ); 
				ID3D11ShaderReflectionType* varTypeD3D11 = variableD3D11->GetType(); 

				D3D11_SHADER_VARIABLE_DESC varDesc; 
				D3D11_SHADER_TYPE_DESC varTypeDesc; 

				variableD3D11->GetDesc( &varDesc ); 
				varTypeD3D11->GetDesc( &varTypeDesc ); 

				UniformParam& param = cbufferParam.BufferVariables[j];

				param.Name = varDesc.Name;
				param.Offset = varDesc.StartOffset;
				param.ArraySize = varTypeDesc.Elements;

				switch (varTypeDesc.Class)
				{
				case D3D10_SVC_SCALAR:
					{
						switch (varTypeDesc.Type)
						{
						case D3D10_SVT_BOOL:	param.Type = EPT_Boolean; break;
						case D3D10_SVT_INT:		param.Type = EPT_Int; break;
						case D3D10_SVT_UINT:	param.Type = EPT_UInt; break;
						case D3D10_SVT_FLOAT:	param.Type = EPT_Float; break;
						default:
							break;
						}
					}
					break;
				case D3D10_SVC_VECTOR:
					{
						switch (varTypeDesc.Type)
						{
						case D3D10_SVT_BOOL:	param.Type = EPT_Boolean; break;
						case D3D10_SVT_INT:		param.Type = EffectParameterType(EPT_Int + varTypeDesc.Columns - 1); break;
						case D3D10_SVT_UINT:	param.Type = EffectParameterType(EPT_UInt + varTypeDesc.Columns - 1); break;
						case D3D10_SVT_FLOAT:	param.Type = EffectParameterType(EPT_Float + varTypeDesc.Columns - 1); break;
						default:
							break;
						}
					}
					break;
				case D3D10_SVC_MATRIX_ROWS:
				case D3D10_SVC_MATRIX_COLUMNS:
					{
						if (varTypeDesc.Rows == 4 && varTypeDesc.Columns == 4)
							param.Type = EPT_Matrix4x4;
						else if (varTypeDesc.Rows == 3 && varTypeDesc.Columns == 3)
							param.Type = EPT_Matrix3x3;
						else if (varTypeDesc.Rows == 2 && varTypeDesc.Columns == 2)
							param.Type = EPT_Matrix2x2;
						else 
							assert(false);
					}
					break;
				case D3D10_SVC_STRUCT:
					{
						//assert(false);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void RefectionBoundResources()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.BoundResources; i++ ) 
		{
			D3D11_SHADER_INPUT_BIND_DESC resBindDesc;
			mReflectorD3D11->GetResourceBindingDesc(i, &resBindDesc);
			
			switch (resBindDesc.Type)
			{
			case D3D10_SIT_CBUFFER:
				break;
			case D3D10_SIT_SAMPLER:
				{
					bool bCompare = (resBindDesc.uFlags & D3D10_SIF_COMPARISON_SAMPLER) != 0;
					//printf("Sampler: %s cmp=%d binding=%d\n", resBindDesc.Name, bCompare, resBindDesc.BindPoint);

					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_Sampler;
					param.Class = Shader_Param_Sampler;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D10_SIT_TEXTURE:  // Missing TextureUAV
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = UnmapTextureType(resBindDesc.Dimension);
					param.Class = Shader_Param_SRV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D10_SIT_TBUFFER:
				{
					assert(false);
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_TextureBuffer;
					param.Class = Shader_Param_SRV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_UAV_RWTYPED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = UnmapTextureType(resBindDesc.Dimension);
					param.Class = Shader_Param_UAV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_STRUCTURED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_StructureBuffer;
					param.Class = Shader_Param_SRV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_UAV_RWSTRUCTURED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_StructureBuffer;
					param.Class = Shader_Param_UAV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;

			case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				{
					assert(false);
					printf("UAV Atomic StructuredBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_BYTEADDRESS:
			case D3D11_SIT_UAV_RWBYTEADDRESS:
			case D3D11_SIT_UAV_APPEND_STRUCTURED:
			case D3D11_SIT_UAV_CONSUME_STRUCTURED:
			default:
				assert(false);
				break;
			}

		}
	}

	EffectParameterType UnmapTextureType(D3D10_SRV_DIMENSION Dimension)
	{
		switch (Dimension)
		{
		case D3D10_SRV_DIMENSION_BUFFER: return EPT_TextureBuffer; 
		case D3D10_SRV_DIMENSION_TEXTURE1D : return EPT_Texture1D; 
		case  D3D10_SRV_DIMENSION_TEXTURE1DARRAY: return EPT_Texture1DArray;
		case  D3D10_SRV_DIMENSION_TEXTURE2D : return EPT_Texture2D; 
		case D3D10_SRV_DIMENSION_TEXTURE2DARRAY: return EPT_Texture2DArray; 
		case D3D10_SRV_DIMENSION_TEXTURE2DMS: return EPT_Texture2D; 
		case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY: return EPT_Texture2DArray; 
		case D3D10_SRV_DIMENSION_TEXTURE3D: return EPT_Texture3D;
		case D3D10_SRV_DIMENSION_TEXTURECUBE: return EPT_TextureCube;
		default:
			break;
		}
		assert (false);
		return EPT_Texture2D;
	}

protected:
	D3D11Shader* mShaderD3D11;

	D3D11_SHADER_DESC mShaderDescD3D11; 
	ID3D11ShaderReflection* mReflectorD3D11;
};


bool LoadBinary(const char* filename, std::vector<uint8_t>& byteCode)
{
	FILE* fp = fopen(filename, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		long len = ftell(fp);
		byteCode.resize(len);

		fseek(fp, 0, SEEK_SET);
		fread(&byteCode[0], len, 1, fp);
		fclose(fp);

		return true;
	}
	return false;
}

// Helper function to dynamic compile HLSL shader code
HRESULT CompileHLSL(const String& filename, const ShaderMacro* macros, uint32_t macroCount,
	const String& entryPoint, const String& shaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	D3D10_SHADER_MACRO* pMacro = nullptr;
	std::vector<D3D10_SHADER_MACRO> d3dMacro;
	if (macros)
	{
		d3dMacro.resize(macroCount+1);
		for (uint32_t i = 0; i < macroCount; ++i)
		{
			d3dMacro[i].Name = macros[i].Name.c_str();
			d3dMacro[i].Definition = macros[i].Definition.c_str();
		}
		d3dMacro.back().Name = d3dMacro.back().Definition = nullptr;
		pMacro = &d3dMacro[0];
	}

	hr = D3DCompileFromFile(StringToWString(filename).c_str(), pMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), 
		shaderModel.c_str(), dwShaderFlags, 0,  ppBlobOut, &pErrorBlob);

	if( FAILED(hr) )
	{
		if (pErrorBlob)
		{
			const char* msg = (char*)( pErrorBlob->GetBufferPointer() );
			fprintf(stderr, "%s %s compile error:\n%s\n", filename.c_str(), entryPoint.c_str(), msg);

			pErrorBlob->Release();
		}
		else
			EngineLogger::LogError("HLSL shader compile failed!");
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
D3D11Shader::D3D11Shader( ShaderType type ) 
	: Shader(type), GlobalCBuffer(NULL)
{

}

D3D11Shader::~D3D11Shader()
{
	SAFE_DELETE(GlobalCBuffer);
}

void D3D11Shader::CreateGlobalConstantBuffer( uint32_t bufferSize )
{
	assert(GlobalCBuffer == nullptr);
	GlobalCBuffer = new EffectConstantBuffer("$Globals", bufferSize);
}

//////////////////////////////////////////////////////////////////////////
D3D11VertexShader::D3D11VertexShader()
	: D3D11Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11VertexShader::~D3D11VertexShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11VertexShader::LoadFromByteCode( const String& filename )
{
	if ( LoadBinary(filename.c_str(), ShaderCode) == false )
	{
		ShaderCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}
	

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateVertexShader(&ShaderCode[0], ShaderCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Vertex Shader %s compile failed\n", filename.c_str());
		return false;
	}
	
	return true;
}

bool D3D11VertexShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "vs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		ShaderCode.resize(shaderBlob->GetBufferSize());
		std::memcpy(&ShaderCode[0], shaderBlob->GetBufferPointer(), ShaderCode.size());

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11HullShader::D3D11HullShader()
	: D3D11Shader(ST_TessControl),
	  ShaderD3D11(nullptr)
{

}

D3D11HullShader::~D3D11HullShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11HullShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateHullShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11HullShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "hs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11DomainShader::D3D11DomainShader()
	: D3D11Shader(ST_TessEval),
	  ShaderD3D11(nullptr)
{

}

D3D11DomainShader::~D3D11DomainShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11DomainShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateDomainShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11DomainShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "ds_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11GeometryShader::D3D11GeometryShader()
	: D3D11Shader(ST_Geomerty),
	  ShaderD3D11(nullptr)
{

}

D3D11GeometryShader::~D3D11GeometryShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11GeometryShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateGeometryShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11GeometryShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "gs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11PixelShader::D3D11PixelShader()
	: D3D11Shader(ST_Pixel),
	  ShaderD3D11(nullptr)
{

}

D3D11PixelShader::~D3D11PixelShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11PixelShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreatePixelShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11PixelShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "ps_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);
	
		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11ComputeShader::D3D11ComputeShader()
	: D3D11Shader(ST_Compute),
	  ShaderD3D11(nullptr)
{

}

D3D11ComputeShader::~D3D11ComputeShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11ComputeShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateComputeShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11ComputeShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "cs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		D3D11_VERRY( deviceD3D11->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11) );

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

}