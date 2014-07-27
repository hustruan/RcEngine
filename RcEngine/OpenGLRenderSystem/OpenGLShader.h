#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

struct InputSignature
{
#ifdef _DEBUG
	String Name;
#endif
	VertexElementFormat Format;
	GLint ArraySize; 
	GLuint AttributeSlot;
};

struct UniformParam
{
	String Name;
	GLint Location;
	GLint Offset;
	GLuint ArraySize;
	GLuint ArrayStride;
	EffectParameterType Type;
};

struct UniformBuffer
{
	String Name;
	GLint Location;
	uint32_t BufferSize;
	vector<UniformParam> BufferVariables;
};

struct ResouceViewParam
{
	String Name;
	GLint Location;
	EffectParameterType Type;
	ShaderParameterClass ViewClass;
};

// Forward declaration
class OpenGLShaderReflection;
class OpenGLShaderPipeline;

class _OpenGLExport OpenGLShader : public Shader
{
public:
	OpenGLShader(ShaderType shaderType);
	virtual ~OpenGLShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint = "");

private:
	friend class OpenGLShaderReflection;
	friend class OpenGLShaderPipeline;
	friend class GLSLScriptCompiler;

	GLuint mShaderOGL;
	
	std::vector<ResouceViewParam> mBoundResources;
	std::vector<UniformBuffer> mUniformBuffers;
	std::vector<InputSignature> mInputSignatures;
	std::map<String, String> mSamplerStates;
};

class EffectConstantBuffer;

class _OpenGLExport OpenGLShaderPipeline : public ShaderPipeline
{
public:
	OpenGLShaderPipeline(Effect& effect);
	virtual ~OpenGLShaderPipeline();

	inline GLuint GetPipelineOGL() const { return mPipelineOGL; }

	virtual void OnBind();
	virtual void OnUnbind();
	virtual bool LinkPipeline();

private:
	void AddUniformParamBind(GLuint shader, GLint location, EffectParameter* effectParam, GLsizei arrSize);
	
	// Bind resource to OpenGL binding unit. (Texture/Image/UBO/SSBO)
	void AddUnitformBlockBind(EffectConstantBuffer* effectCBuffer, GLuint binding);
	void AddSRVResouceBind(EffectParameter* effectParam, GLuint binding);
	void AddUAVResourceBind(EffectParameter* effectParam, GLuint binding);

	// Set binding slot to shader context
	void AddShaderResourceBind(GLuint shader, GLint location, GLuint binding, GLuint resourceType);

private:
	GLuint mPipelineOGL;
	std::vector<std::function<void()>> mParameterBinds; 
};


}



#endif // OpenGLShader_h__
