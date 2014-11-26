#ifndef RHDevice_h__
#define RHDevice_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderOperation.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>


#define MaxSamplerCout 16

namespace RcEngine {

class RenderFactory;
struct Viewport;

class _ApiExport RenderDevice
{
public:
	RenderDevice();
	virtual ~RenderDevice(void);

	inline bool Fullscreen() const												    { return mIsFullscreen; }
	inline RenderFactory* GetRenderFactory() const									{ return mRenderFactory; }
																  
	inline shared_ptr<FrameBuffer> GetCurrentFrameBuffer() const					{ return mCurrentFrameBuffer; }
	inline shared_ptr<FrameBuffer> GetScreenFrameBuffer() const					    { return mScreenFrameBuffer; }
	inline shared_ptr<DepthStencilState> GetCurrentDepthStencilState() const	    { return mCurrentDepthStencilState; }
	inline shared_ptr<BlendState> GetCurrentBlendState() const					    { return mCurrentBlendState; }

	virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
	virtual void ToggleFullscreen(bool fs) = 0;
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

	virtual void SetRasterizerState(const shared_ptr<RasterizerState>& state) = 0;
	virtual void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state) = 0;
	virtual void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	virtual void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0) = 0;
	virtual void DispatchCompute(const EffectTechnique* technique, uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCounZ) = 0;

	void BindFrameBuffer(const shared_ptr<FrameBuffer>& fb);
	void BindShaderPipeline(const shared_ptr<ShaderPipeline>& pipeline);

	void Draw(const EffectTechnique* technique, const RenderOperation& operation);
	
	// Draw a full screen triangle without Vertex/Index buffer
	void DrawFSTriangle(const EffectTechnique* technique);

protected:
	virtual void DoDraw(const EffectTechnique* technique, const RenderOperation& operation) = 0;
	virtual void DoBindShaderPipeline(const shared_ptr<ShaderPipeline>& pipeline) = 0;

private:
	RenderOperation mFSTriangleROP;

protected:
	RenderFactory* mRenderFactory;

	uint32_t mWidth, mHeight;
	bool mIsFullscreen;

	shared_ptr<ShaderPipeline> mCurrentShaderPipeline;
	shared_ptr<FrameBuffer> mCurrentFrameBuffer;
	shared_ptr<FrameBuffer> mScreenFrameBuffer;
	shared_ptr<BlendState> mCurrentBlendState;
	shared_ptr<RasterizerState> mCurrentRasterizerState;
	shared_ptr<DepthStencilState> mCurrentDepthStencilState;

	ColorRGBA mCurrentBlendFactor;
	uint32_t mCurrentSampleMask;
	uint16_t mCurrentFrontStencilRef, mCurrentBackStencilRef;	
};

} // Namespace RcEngine

#endif // RHDevice_h__
