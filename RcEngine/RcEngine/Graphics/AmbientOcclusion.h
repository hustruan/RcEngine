#ifndef AmbientOcclusion_h__
#define AmbientOcclusion_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicsResource.h>
#include <GUI/UIWindow.h>

namespace RcEngine {

class RenderDevice;
class AmbientOcclusionPanel;

enum SSAO_Method
{
	SSAO_Alchemy,
	SSAO_HBAO
};

enum SSAO_BlurRadius
{
	SSAO_BLUR_RADIUS_2,                               // Kernel radius = 2 pixels
	SSAO_BLUR_RADIUS_4,                               // Kernel radius = 4 pixels
	SSAO_BLUR_RADIUS_8,                               // Kernel radius = 8 pixels
};

struct AmbientOcclusionSettings
{
	float                Radius;                     // The AO radius in meters
	float                Bias;                       // To hide low-tessellation artifacts // 0.0~1.0
	float                DetailAO;                   // Scale factor for the detail AO, the greater the darker // 0.0~2.0
	float                CoarseAO;                   // Scale factor for the coarse AO, the greater the darker // 0.0~2.0
	float                PowerExponent;              // The final AO output is pow(AO, powerExponent)

	bool                 BlurEnable;                 // To blur the AO with an edge-preserving blur
	SSAO_BlurRadius      BlurRadius;                 // BLUR_RADIUS_2, BLUR_RADIUS_4 or BLUR_RADIUS_8
	float                Sharpness;                  // The higher, the more the blur preserves edges // 0.0~16.0

	float				 MetersToViewSpaceUnits;

	AmbientOcclusionSettings()
		: Radius(1.f),
		  Bias(0.1f),
		  DetailAO(0.f), 
		  CoarseAO(1.f) , 
		  PowerExponent(2.f),
		  BlurEnable(true),
		  BlurRadius(SSAO_BLUR_RADIUS_4),
		  Sharpness(4.f),
		  MetersToViewSpaceUnits(1.0f)
	{
	}
};

class _ApiExport AmbientOcclusion
{
public:
	struct _ApiExport HBAOImpl
	{
		HBAOImpl() {}
		virtual ~HBAOImpl() {}

		virtual void RenderSSAO(const AmbientOcclusionSettings& settings, 
			const Camera& viewCamera,
			const shared_ptr<FrameBuffer>& outAOFrameBuffer,
			const shared_ptr<Texture>& depthBuffer, 
			const shared_ptr<Texture>& normalBuffer) = 0;
	};

public:
	AmbientOcclusion(RenderDevice* device, SSAO_Method aoMethod, uint32_t aoWidth, uint32_t aoHeight);
	~AmbientOcclusion() {}

	void Apply(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer);
	void Visualize(const shared_ptr<ShaderResourceView>& aoSRV);
	shared_ptr<ShaderResourceView> GetAmbientOcclusionSRV() const { return mAmbientOcclusionBuffer->GetShaderResourceView(); }

private:
	void GenerateCameraSpaceDepth(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer);
	void RenderAlchemy(const Camera& viewCamera, const shared_ptr<Texture>& cszBuffer);
	void RenderHBAO(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer);
	void BlurAmbientObscurance(const shared_ptr<Texture>& aoBuffer);

private:
	RenderDevice* mDevice;
	SSAO_Method mMethod;
	
	int mWidth, mHeight;
	bool mResizing; 

	shared_ptr<FrameBuffer> mFrameBuffer;

	shared_ptr<Texture> mAmbientOcclusionBuffer;
	shared_ptr<RenderView> mAmbientOcclusionRTV;

	shared_ptr<Texture> mCameraSpaceDepthBuffer;
	vector<shared_ptr<RenderView> > mCameraSpaceDepthBufferRTVs;

	shared_ptr<Texture> mBlurBuffer;
	shared_ptr<RenderView> mBlurRTV;

	shared_ptr<Effect> mAmbientOcclusionEffect;
	RenderOperation mFullscreenTrangleROP;

	shared_ptr<HBAOImpl> mHBAOImpl;

public:
	AmbientOcclusionSettings mSettings;
	AmbientOcclusionPanel* mAmbientOcclusionPanel;
};

class _ApiExport AmbientOcclusionPanel
{
public:
	AmbientOcclusionPanel(AmbientOcclusion& ao);
	~AmbientOcclusionPanel() {}

	void InitGUI(int2 position);

private:
	void RadiusChanged(int32_t value);
	void BiasChanged(int32_t value);
	void DetailAOChanged(int32_t value);
	void CoarseAOChanged(int32_t value);
	void SceneUnitsChanged(int32_t value);

private:
	AmbientOcclusion& mAmbientOcclusion;

	UIWindow* mSSAOParamsPanel;

	Label* mRadiusLabel;
	Slider* mRadiusSlider;

	Label* mBiasLabel;
	Slider* mBiasSlider;

	Label* mDetailAOLabel;
	Slider* mDetailAOSlider;

	Label* mCoarseAOLabel;
	Slider* mCoarseAOSlider;

	Label* mSceneUnitsLabel;
	Slider* mSceneUnitslider;
};


}

#endif // AmbientOcclusion_h__
