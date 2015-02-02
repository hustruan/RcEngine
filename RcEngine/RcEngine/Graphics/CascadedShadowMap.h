#ifndef CascadedShadowMap_h__
#define CascadedShadowMap_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderOperation.h>
#include <Math/Matrix.h>

namespace RcEngine {

#define MAX_CASCADES 4
#define SHADOW_MAP_SIZE 2048
#define SHADOW_MAP_BLUR_KERNEL_SIZE 5
#define MAX_POSSION_SAMPLES 24

class RenderDevice;

class _ApiExport CascadedShadowMap
{
public:
	enum ShadowMapFilter
	{
		PossionDiskPCF,
		VSM,
		EVSM
	};

public:
	CascadedShadowMap(RenderDevice* device);
	virtual ~CascadedShadowMap() {}
	
	void MakeCascadedShadowMap(const Light& light);
	void MakeSpotShadowMap(const Light& light);

private:
	void UpdateShadowMapStorage(const Light& light);
	void UpdateShadowMatrix(const Camera& camera, const Light& directionLight);
	void CreatePossionDiskSamples();

private:	
	RenderDevice* mDevice;

	float mSplitPlanes[MAX_CASCADES+1];

	// Used in frame buffer camera
	std::vector<shared_ptr<Camera>> mLightCamera;
	std::vector<shared_ptr<RenderView>> mShadowSplitsRTV;

	shared_ptr<FrameBuffer> mShadowFrameBuffer;
	
	shared_ptr<Texture> mShadowMapTempBlur;
	shared_ptr<RenderView> mShadowMapTempBlurRTV;

	shared_ptr<Texture> mShadowMapTempBlur1;
	shared_ptr<RenderView> mShadowMapTempBlurRTV1;

	shared_ptr<Effect> mBlurEffect;

	// FSQuad
	RenderOperation mFSQuadRop;

public:

	ShadowMapFilter mShadowMapFilter;
	String mShadowMapTech;

	shared_ptr<Texture> mShadowDepth;
	shared_ptr<Texture> mShadowTexture;
	
	// ShadowMap SamplState
	shared_ptr<SamplerState> mPCFSampleState;
	shared_ptr<SamplerState> mVSMSampleState;

	// Light view matrix
	float4x4 mLightViewMatrix; 

	// Map [-1, 1]x[-1, 1] -> [0, 1]x[0, 1]
	float4x4 mShadowTexCoordNormMatrix;
	
	// Light ortho projection scale and offset 
	std::vector<float4> mShadowCascadeScale;
	std::vector<float4> mShadowCascadeOffset;

	// For Map based selection scheme, this keeps the pixels inside of the the valid range.
	// When there is no boarder, these values are 0 and 1 respectivley.
	float2 mBorderPaddingMinMax;

	bool mMoveLightTexelSize;
	float mCascadeBlendArea;

	// PCF 
	uint32_t mNumPossionSamples;
	float mShadowFilterSize;
	float mShadowBias;	
	shared_ptr<GraphicsBuffer> mPossionSamplesCBuffer;
};


}

#endif // CascadedShadowMap_h__
