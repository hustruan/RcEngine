#ifndef DeepGBufferRadiosity_h__
#define DeepGBufferRadiosity_h__

#include <Graphics/RenderPath.h>
#include "DeepGBufferRadiositySettings.h"

namespace RcEngine {

class DeepGBufferRadiosity : public RenderPath
{
private:
	
	class GBuffer
	{
	public:
		enum Field
		{
			DepthStencil = 0,
			Lambertain,
			Glossy,
			Normal,
			ScreenSpaceVelocity,
			Count 
		};

	public:		
		void CreateBuffers(uint32_t width, uint32_t height);
		
		const shared_ptr<ShaderResourceView>& GetTextureSRV(Field field) const;
		const shared_ptr<Texture>& GetTexture(Field field) const { return mTextures[field]; }

	public:
		shared_ptr<FrameBuffer> mFrameBuffer;
		shared_ptr<Texture> mTextures[GBuffer::Count];
	};

	class MipmapGenerator
	{
	public:
		MipmapGenerator(const shared_ptr<Texture>& buffer, int maxLevel);
		void GenerateMipmap(const shared_ptr<FrameBuffer>& fb, const shared_ptr<Effect>& minifyTech, int techIndex);

	private:
		RenderDevice* mDevice;
		uint32_t mMaxLevel;
		vector<shared_ptr<ShaderResourceView> > mPrevLevelSRV;
		vector<shared_ptr<RenderView> > mLevelRTV;
	};

public:
	DeepGBufferRadiosity(void);
	~DeepGBufferRadiosity(void);

	void OnGraphicsInit(const shared_ptr<Camera>& camera);
	void OnWindowResize(uint32_t width, uint32_t height);
	void RenderScene();

private:
	
	PixelFormat GetCSZBufferFormat(bool twoChannel) const;
	PixelFormat GetNormalFormat(bool useOct16) const;
	PixelFormat GetColorFormat(bool useHalfPrecisionColor) const;

	uint32_t NumSpiralTurns() const;

	void CreateBuffers(uint32_t width, uint32_t height);
	
	void Prepare();
	void RenderGBuffers();
	void RenderLambertianOnly();
	void RenderIndirectIllumination();
	void DeferredShade();

	void ComputeMipmapedBuffers();
	void ComputeRawII();
	
	void TemporalFiltering();

	//void BlurHorizontal();
	//void BlurVertical();

	const shared_ptr<Texture> GetRadiosityTexture() const;

private:
	DeepGBufferRadiositySettings mSettings;

	uint32_t mBufferWidth, mBufferHeight;

	float4 mProjInfo;
	float2 mClipInfo;
	float mProjScale;

	// GBuffers
	GBuffer mGBuffer;
	GBuffer mPeeledGBuffer;

	shared_ptr<FrameBuffer> mFrameBuffer;
	
	shared_ptr<Texture> mCSZBuffer;
	shared_ptr<RenderView> mCSZRTV;
	shared_ptr<MipmapGenerator> mCSZMipmapGen;

	shared_ptr<Texture> mPackedNormalBuffer;
	shared_ptr<RenderView> mPackedNormalRTV;
	shared_ptr<MipmapGenerator> mPackedNormalMipmapGen;

	// Initial Radiosity
	shared_ptr<Texture> mLambertDirectBuffer;
	shared_ptr<RenderView> mLambertDirectRTV;
	shared_ptr<MipmapGenerator> mLambertMipmapGen;

	shared_ptr<Texture> mPeeledLambertDirectBuffer;
	shared_ptr<RenderView> mPeeledLambertDirectRTV;
	shared_ptr<MipmapGenerator> mPeeledLambertMipmapGen;
	
	shared_ptr<Texture> mResultBuffer;
	shared_ptr<Texture> mTempFiltedResultBuffer;
	shared_ptr<RenderView> mTempFiltedResultRTV;
	shared_ptr<RenderView> mResultRTV;

	// Raw indirect irradiance buffer
	shared_ptr<Texture> mRawIIBuffer;
	shared_ptr<Texture> mPreviousRawIIBuffer;
	shared_ptr<RenderView> mRawIIRTV;

	shared_ptr<Texture> mPreviousDepthBuffer;

	// Effect
	shared_ptr<Effect> mLambertianOnlyEffect;
	shared_ptr<Effect> mRadiosityEffect;
    shared_ptr<Effect> mReconstrctCSZEffect;
	shared_ptr<Effect> mMinifyEffect;
	shared_ptr<Effect> mDeepGBufferShadeEffect;
	shared_ptr<Effect> mTemporalFilterEffect;
	shared_ptr<Effect> mBlitEffect;

	float4x4 mInvViewMatrix;
	float4x4 mPrevViewMatrix;
	float4x4 mPrevInvViewMatrix;
	float4x4 mViewportMatrix;
};	


}




#endif // DeepGBufferRadiosity_h__

