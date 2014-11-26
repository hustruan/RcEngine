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
	void CreateBuffers(uint32_t width, uint32_t height);
	void RenderLambertianOnly();
	void GenerateGBuffers();

private:

	DeepGBufferRadiositySettings mSettings;

	uint32_t mBufferWidth, mBufferHeight;

	// GBuffers
	GBuffer mGBuffer;
	GBuffer mPeeledGBuffer;

	// Initial Radiosity
	shared_ptr<FrameBuffer> mLambertianDirectFrameBuffer;
	shared_ptr<Texture> mLambertDirectBuffer;

	shared_ptr<FrameBuffer> mPeeledLambertianDirectFrameBuffer;
	shared_ptr<Texture> mPeeledLambertDirectBuffer;

	shared_ptr<Texture> mDeepGBufferRadiosityBuffer;
	shared_ptr<Texture> mPreviousDepthBuffer;

	// Effect
	shared_ptr<Effect> mLambertianOnlyEffect;

	shared_ptr<Effect> mBlitEffect;

	float4x4 mPrevViewMatrix;
	float4x4 mViewportMatrix;
};	


}




#endif // DeepGBufferRadiosity_h__

