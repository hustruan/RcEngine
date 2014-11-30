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

	void CreateBuffers(uint32_t width, uint32_t height);
	
	void RenderGBuffers();
	void RenderLambertianOnly();
	void RenderIndirectIllumination();

	void ComputeMipmapedBuffers();
	void ComputeRawII();
	void TemporalFiltering();

	const shared_ptr<Texture> GetRadiosityTexture() const;



private:
	DeepGBufferRadiositySettings mSettings;

	uint32_t mBufferWidth, mBufferHeight;

	// GBuffers
	GBuffer mGBuffer;
	GBuffer mPeeledGBuffer;

	shared_ptr<FrameBuffer> mFrameBuffer;
	
	shared_ptr<Texture> mCSZBuffer;
	shared_ptr<RenderView> mCSZRTV;

	shared_ptr<Texture> mPackedNormalBuffer;
	shared_ptr<RenderView> mPackedNormalRTV;

	// Initial Radiosity
	shared_ptr<Texture> mLambertDirectBuffer;
	shared_ptr<RenderView> mLambertDirectRTV;

	shared_ptr<Texture> mPeeledLambertDirectBuffer;
	shared_ptr<RenderView> mPeeledLambertDirectRTV;
	
	shared_ptr<Texture> mResultBuffer;
	shared_ptr<Texture> mTempFiltedResultBuffer;

	// Raw indirect irradiance buffer
	shared_ptr<Texture> mRawIIBuffer;
	shared_ptr<RenderView> mRawIIRTV;

	shared_ptr<Texture> mPreviousDepthBuffer;


	// Effect
	shared_ptr<Effect> mLambertianOnlyEffect;
	shared_ptr<Effect> mRadiosityEffect;
    shared_ptr<Effect> mReconstrctCSZEffect;
	shared_ptr<Effect> mBlitEffect;

	float4x4 mPrevViewMatrix;
	float4x4 mViewportMatrix;
};	


}




#endif // DeepGBufferRadiosity_h__

