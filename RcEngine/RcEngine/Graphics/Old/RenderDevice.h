#ifndef RenderDevice_h__
#define RenderDevice_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderSettings.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>

#define MaxSamplerCout 16

namespace RcEngine {

enum RenderDeviceType
{
	RD_OpenGL = 0,
	RD_OpenGL_ES,
	RD_Direct3D11,
	RD_Count
};

class _ApiExport RenderDevice
{
public:
	RenderDevice(RenderDeviceType deviceType);
	virtual ~RenderDevice(void);

	virtual void Create() = 0;
	virtual void Release() = 0;
	virtual void ToggleFullscreen(bool fs) = 0;
	virtual bool Fullscreen() const = 0;
	virtual void CreateRenderWindow(const RenderSettings& settings) = 0;	
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

	void Render( EffectTechnique& tech, RenderOperation& op);
	void Resize(uint32_t width, uint32_t height);

	void BindFrameBuffer(const shared_ptr<FrameBuffer>& fb);

	RenderDeviceType GetRenderDeviceType() const			{ return mRenderDeviceType; }
	RenderFactory* GetRenderFactory() const					{ return mRenderFactory; }

	shared_ptr<FrameBuffer> GetCurrentFrameBuffer() const	{ return mCurrentFrameBuffer; }
	shared_ptr<FrameBuffer> GetScreenFrameBuffer() const	{ return mScreenFrameBuffer; }

	shared_ptr<DepthStencilState> GetCurrentDepthStencilState() const	{ return mCurrentDepthStencilState; }
	shared_ptr<BlendState> GetCurrentBlendState() const					{ return mCurrentBlendState; }

	virtual void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state) = 0;
	virtual void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	virtual void SetRasterizerState(const shared_ptr<RasterizerState>& state) = 0;
	virtual void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0) = 0;

protected:
	virtual void DoBindFrameBuffer(const shared_ptr<FrameBuffer>& fb) = 0;
	virtual void DoRender( EffectTechnique& tech, RenderOperation& op ) = 0;

protected:
	RenderDeviceType mRenderDeviceType;

	uint32_t mWidth, mHeight;
	PixelFormat mColorFormat;
	uint32_t mColorBits;

	uint32_t mDepthBits, mStencilBits;
	bool mIsDepthBuffered;

	shared_ptr<FrameBuffer> mCurrentFrameBuffer;
	shared_ptr<FrameBuffer> mScreenFrameBuffer;
			
	RenderSettings mRenderSettings;

	RenderFactory* mRenderFactory;

	shared_ptr<BlendState> mCurrentBlendState;
	shared_ptr<RasterizerState> mCurrentRasterizerState;
	shared_ptr<DepthStencilState> mCurrentDepthStencilState;

	ColorRGBA mCurrentBlendFactor;
	uint32_t mCurrentSampleMask;

	uint16_t mCurrentFrontStencilRef, mCurrentBackStencilRef;	
};

} // Namespace RcEngine

#endif // RenderDevice_h__