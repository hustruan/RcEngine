#ifndef D3D11Shader_h__
#define D3D11Shader_h__

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

struct InputSignature
{
	String Semantic;
	uint32_t SemanticIndex;
};

struct ResourceInputParam
{
	String Name;
	uint32_t Binding;
	EffectParameterType Type;
	ShaderParameterClass Class;
};

struct UniformParam
{
	String Name;
	uint32_t Offset;
	uint32_t ArraySize;
	EffectParameterType Type;
};

struct ConstantBuffer
{
	String Name;
	uint32_t BufferSize;
	uint32_t Binding;
	vector<UniformParam> BufferVariables;
};

// Forward declaration
class EffectConstantBuffer;

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11Shader : public Shader
{
public:
	D3D11Shader(ShaderType type);
	virtual ~D3D11Shader();

	/**
	 * Only called when shader has global uniform.
	 */
	void CreateGlobalConstantBuffer(uint32_t bufferSize);
	
public:
	vector<ResourceInputParam> ResourceInputParams;
	vector<ConstantBuffer> ConstantBufferParams;
	
	// Global constant buffer with name "$Globals", keep it as a state of separate shader 
	EffectConstantBuffer* GlobalCBuffer;

	friend class D3D11ShaderReflection;
	friend class D3D11ShaderPipeline;
};


//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11VertexShader : public D3D11Shader
{
public:
	D3D11VertexShader();
	~D3D11VertexShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11VertexShader* ShaderD3D11;
	vector<uint8_t> ShaderCode;
	vector<InputSignature> InputSignatures;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11HullShader : public D3D11Shader
{
public:
	D3D11HullShader();
	~D3D11HullShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11HullShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11DomainShader : public D3D11Shader
{
public:
	D3D11DomainShader();
	~D3D11DomainShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11DomainShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11GeometryShader : public D3D11Shader
{
public:
	D3D11GeometryShader();
	~D3D11GeometryShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11GeometryShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11PixelShader : public D3D11Shader
{
public:
	D3D11PixelShader();
	~D3D11PixelShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11PixelShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ComputeShader : public D3D11Shader
{
public:
	D3D11ComputeShader();
	~D3D11ComputeShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11ComputeShader* ShaderD3D11;
};


////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ShaderPipeline : public ShaderPipeline
{
public:
	D3D11ShaderPipeline(Effect& effect);

	virtual bool LinkPipeline();
	virtual void OnBind();
	virtual void OnUnbind();

private:
	void AddGlobalUniformBind(EffectConstantBuffer* cbuffer, EffectParameter* effectParam, uint32_t offset, uint32_t arrSize);

protected:
	std::vector<std::function<void()>> mParameterBinds; 
}; 

}

#endif // D3D11Shader_h__
