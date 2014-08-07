#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include <Graphics/RenderDevice.h>
#include "OpenGLGraphicCommon.h"
#include "OpenGLFrameBuffer.h"

namespace RcEngine {

class OpenGLShader;
class OpenGLBuffer;
class OpenGLDevice;

// Global OpenGL deice
extern OpenGLDevice* gOpenGLDevice;

class _OpenGLExport OpenGLDevice : public RenderDevice
{
public:
	OpenGLDevice();
	~OpenGLDevice(void);

	void CreateRenderWindow();

	void GetBlitFBO(GLuint& srcFBO, GLuint& dstFBO);
	
	inline GLuint GetCurrentFBO() const { return mCurrentFBO; }
	void BindFBO(GLuint fbo); 


	void OnWindowResize( uint32_t width, uint32_t height );
	void ToggleFullscreen(bool fs);
	void AdjustProjectionMatrix(float4x4& pOut);

	void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);		
	void SetRasterizerState(const shared_ptr<RasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state);
	void SetViewports(const std::vector<Viewport>& viewports);
	void DispatchCompute(const EffectTechnique* technique, uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCounZ);

protected:

	void DoBindShaderPipeline(const shared_ptr<ShaderPipeline>& pipeline);
	void DoDraw(const EffectTechnique* technique, const RenderOperation& operation);

private:
	
	// Only track the first view port
	Viewport mCurrentViewport;

	shared_ptr<SamplerState> mCurrentSamplerStates[MaxSamplerCout];		

	// source and destination blit framebuffer
	GLuint mBlitFBO[2];

	GLuint mCurrentFBO;
};

}


#endif // OpenGLRenderDevice_h__