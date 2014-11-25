#include <Graphics/AmbientOcclusion.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Geometry.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/Image.h>
#include <Resource/ResourceManager.h>
#include <Core/Environment.h>
#include <GUI/UIAll.h>


/** Floating point bits per pixel for CSZ: 16 or 32.  There is no perf difference on GeForce GTX 580 */
#define ZBITS (32)

/** This must be greater than or equal to the MAX_MIP_LEVEL and  defined in AmbientOcclusion_AO.pix. */
#define MAX_MIP_LEVEL (5)

/** Used to allow us to depth test versus the sky without an explicit check, speeds up rendering when some of the skybox is visible */
#define Z_COORD (-1.0f)

#define MIN_SAMPLE_RADIUS (0.5f)
#define MAX_SAMPLE_RADIUS (30.0f)

namespace RcEngine {

#ifdef _DEBUG
	static uint32_t BufferAccessHint = EAH_CPU_Read | EAH_GPU_Read | EAH_GPU_Write;
#else
	static uint32_t BufferAccessHint = EAH_GPU_Read | EAH_GPU_Write;
#endif

AmbientOcclusion::AmbientOcclusion(RenderDevice* device, SSAO_Method aoMethod, uint32_t aoWidth, uint32_t aoHeight)
	: mDevice(device),
	  mMethod(aoMethod),
	  mWidth(aoWidth),
	  mHeight(aoHeight),
	  mResizing(true)
{
	RenderFactory* factory = mDevice->GetRenderFactory();

	mFrameBuffer = factory->CreateFrameBuffer(aoWidth, aoHeight);

	if (mMethod == SSAO_HBAO)
	{
		mHBAOImpl = factory->CreateHBAO(aoWidth, aoHeight);
	}
	else
	{

	}

	BuildFullscreenTriangle(mFullscreenTrangleROP);
	mAmbientOcclusionEffect = ResourceManager::GetSingleton().GetResourceByName<Effect>(RT_Effect, "AmbientOcclusion.effect.xml", "General");

	mAmbientOcclusionPanel = new AmbientOcclusionPanel(*this);
	mAmbientOcclusionPanel->InitGUI(int2(900, 500));

	if (mMethod == SSAO_Alchemy)
	{
		mSettings.Bias = 0.002f;
	}
}

void AmbientOcclusion::GenerateCameraSpaceDepth(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer)
{
	const float4x4& proj = viewCamera.GetProjMatrix();

	if (!mCameraSpaceDepthBuffer || mCameraSpaceDepthBuffer->GetWidth() != mWidth || mCameraSpaceDepthBuffer->GetHeight() != mHeight)
	{
		mWidth = zBuffer->GetWidth();
		mHeight = zBuffer->GetHeight();

		RenderFactory* factory = mDevice->GetRenderFactory();

		mCameraSpaceDepthBuffer = factory->CreateTexture2D(
			mWidth, mHeight,
			PF_R32F, 
			1, 
			MAX_MIP_LEVEL,
			1, 0,
			BufferAccessHint,
			TexCreate_RenderTarget | TexCreate_ShaderResource |TexCreate_GenerateMipmaps, 
			NULL);

		for (int i = 0; i < MAX_MIP_LEVEL; ++i)
			mCameraSpaceDepthBufferRTVs.push_back( factory->CreateRenderTargetView2D(mCameraSpaceDepthBuffer, 0, i) );
	}

	// Reconstruct camera space zbuffer
	mFrameBuffer->AttachRTV(ATT_Color0, mCameraSpaceDepthBufferRTVs[0]);
	mDevice->BindFrameBuffer(mFrameBuffer);

	mAmbientOcclusionEffect->GetParameterByName("DepthBuffer")->SetValue(zBuffer->GetShaderResourceView());
	mAmbientOcclusionEffect->GetParameterByName("ClipInfo")->SetValue(float2(proj.M33, proj.M43));

	EffectTechnique* cszTech = mAmbientOcclusionEffect->GetTechniqueByName("ResconstructeCSZ");
	mDevice->Draw(cszTech, mFullscreenTrangleROP);
	mCameraSpaceDepthBuffer->BuildMipMap();

	// Build mipmap
	//EffectTechnique* cszMinifyTech = mAmbientOcclusionEffect->GetTechniqueByName("CSZMinify");
	//mAmbientOcclusionEffect->GetParameterByName("DepthBuffer")->SetValue(zBuffer->GetShaderResourceView());
	//for (int i = 1; i < 2; ++i)
	//{
	//	mFrameBuffer->AttachRTV(ATT_Color0, mCameraSpaceDepthBufferRTVs[i]);
	//	mFrameBuffer->SetViewport(0, Viewport(0, 0, mWidth >> i, mHeight >> i));
	//	mDevice->BindFrameBuffer(mFrameBuffer);

	//	 mCameraSpaceDepthBufferRTVs[i]->ClearColor(ColorRGBA(1.5, 0, 0, 0));

	//	mAmbientOcclusionEffect->GetParameterByName("PreviousMIPLevel")->SetValue(i-1);
	//	mDevice->Draw(cszMinifyTech, mFullscreenTrangleROP);
	//}
		
/*	Image img;
	img.CopyImageFromTexture(mCameraSpaceDepthBuffer);
	img.SaveImageToFile("E:/csz.pfm", 0, 1);*/
}

void AmbientOcclusion::RenderAlchemy(const Camera& viewCamera, const shared_ptr<Texture>& cszBuffer)
{
	const float4x4& proj = viewCamera.GetProjMatrix();

	const float width = static_cast<float>(mAmbientOcclusionBuffer->GetWidth());
	const float height = static_cast<float>(mAmbientOcclusionBuffer->GetHeight());

	mFrameBuffer->AttachRTV(ATT_Color0, mAmbientOcclusionRTV);
	mDevice->BindFrameBuffer(mFrameBuffer);

	mAmbientOcclusionEffect->GetParameterByName("CameraSpaceZBuffer")->SetValue(mCameraSpaceDepthBuffer->GetShaderResourceView());

	float4 projInfo(2.0f/(width*proj.M11), -2.0f/(height*proj.M22), -1.0f/proj.M11, 1.0f/proj.M22);
	float projScale = 0.5f * proj.M22 * height;
	mAmbientOcclusionEffect->GetParameterByName("ProjInfo")->SetValue(projInfo);

	float radius2 = mSettings.Radius*mSettings.Radius;
	float radius3 = radius2 * mSettings.Radius;
	float invRadius2 = 1.0f / radius2;
	float invRadius6 = 1.0f / (radius3 * radius3);

	mAmbientOcclusionEffect->GetParameterByName("ProjScale")->SetValue(projScale);
	mAmbientOcclusionEffect->GetParameterByName("Radius")->SetValue(mSettings.Radius);
	mAmbientOcclusionEffect->GetParameterByName("Radius2")->SetValue(radius2);
	mAmbientOcclusionEffect->GetParameterByName("InvRadius2")->SetValue(invRadius2);
	mAmbientOcclusionEffect->GetParameterByName("Bias")->SetValue(mSettings.Bias);
	mAmbientOcclusionEffect->GetParameterByName("IntensityDivR6")->SetValue(invRadius6);

	EffectTechnique* aoTech = mAmbientOcclusionEffect->GetTechniqueByName("SSAO");
	mDevice->Draw(aoTech, mFullscreenTrangleROP);
	//mDevice->GetRenderFactory()->SaveTextureToFile("E:/ao.pfm", mAmbientOcclusionBuffer);
}

void AmbientOcclusion::RenderHBAO(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer)
{
	mHBAOImpl->RenderSSAO(mSettings, viewCamera, mFrameBuffer, zBuffer, nullptr);
}

void AmbientOcclusion::BlurAmbientObscurance(const shared_ptr<Texture>& aoBuffer)
{
	if (!mCameraSpaceDepthBuffer || mCameraSpaceDepthBuffer->GetWidth() != mWidth || mCameraSpaceDepthBuffer->GetHeight() != mHeight)
	{
		RenderFactory* factory = mDevice->GetRenderFactory();

		mBlurBuffer = factory->CreateTexture2D(mWidth, mHeight, PF_R32F, 1, 1, 1, 0, BufferAccessHint, TexCreate_RenderTarget | TexCreate_ShaderResource, NULL);
		mBlurRTV = factory->CreateRenderTargetView2D(mBlurBuffer, 0, 0);
	}

	EffectTechnique* blurTech = mAmbientOcclusionEffect->GetTechniqueByIndex(2);

	mFrameBuffer->AttachRTV(ATT_Color0, mBlurRTV);
	mDevice->BindFrameBuffer(mFrameBuffer);

	mAmbientOcclusionEffect->GetParameterByName("Source")->SetValue(mAmbientOcclusionBuffer->GetShaderResourceView());
	mAmbientOcclusionEffect->GetParameterByName("CSZBuffer")->SetValue(mCameraSpaceDepthBuffer->GetShaderResourceView());
	mAmbientOcclusionEffect->GetParameterByName("BlurAxis")->SetValue(int2(1, 0));
	mDevice->Draw(blurTech, mFullscreenTrangleROP);


	mFrameBuffer->AttachRTV(ATT_Color0, mAmbientOcclusionRTV);
	mDevice->BindFrameBuffer(mFrameBuffer);
	mAmbientOcclusionRTV->ClearColor(ColorRGBA(0, 0, 0, 0));

	mAmbientOcclusionEffect->GetParameterByName("Source")->SetValue(mBlurBuffer->GetShaderResourceView());
	mAmbientOcclusionEffect->GetParameterByName("BlurAxis")->SetValue(int2(0, 1));
	mDevice->Draw(blurTech, mFullscreenTrangleROP);
}

void AmbientOcclusion::Apply(const Camera& viewCamera, const shared_ptr<Texture>& zBuffer)
{
	// Update buffer size
	mWidth = zBuffer->GetWidth();
	mHeight = zBuffer->GetHeight();

	if (!mAmbientOcclusionBuffer || mAmbientOcclusionBuffer->GetWidth() != mWidth || mAmbientOcclusionBuffer->GetHeight() != mHeight)
	{
		RenderFactory* factory = mDevice->GetRenderFactory();
		mAmbientOcclusionBuffer = factory->CreateTexture2D(mWidth, mHeight, PF_R32F, 1, 1, 1, 0, BufferAccessHint, TexCreate_RenderTarget |  TexCreate_ShaderResource, NULL);
		mAmbientOcclusionRTV = factory->CreateRenderTargetView2D(mAmbientOcclusionBuffer, 0, 0);

		mFrameBuffer->Resize(mWidth, mHeight);

		if (mMethod == SSAO_HBAO)
			mFrameBuffer->AttachRTV(ATT_Color0, mAmbientOcclusionRTV);
	}

	if (mMethod == SSAO_Alchemy)
	{
		GenerateCameraSpaceDepth(viewCamera, zBuffer);
		RenderAlchemy(viewCamera, zBuffer);
	}
	else if (mMethod == SSAO_HBAO)
	{
		RenderHBAO(viewCamera, zBuffer);
	}

	Visualize(mAmbientOcclusionBuffer->GetShaderResourceView());
}

void AmbientOcclusion::Visualize(const shared_ptr<ShaderResourceView>& aoSRV)
{
	// Visualize 
	shared_ptr<FrameBuffer> screenFB = mDevice->GetScreenFrameBuffer();
	mDevice->BindFrameBuffer(screenFB);
	screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0, 0);

	mAmbientOcclusionEffect->GetParameterByName("DepthBuffer")->SetValue(aoSRV);

	EffectTechnique* aoTech = mAmbientOcclusionEffect->GetTechniqueByName("CopyColor");
	mDevice->Draw(aoTech, mFullscreenTrangleROP);
}

//////////////////////////////////////////////////////////////////////////
AmbientOcclusionPanel::AmbientOcclusionPanel(AmbientOcclusion& ao)
	: mAmbientOcclusion(ao)
{

}

void AmbientOcclusionPanel::InitGUI(int2 position)
{
	mSSAOParamsPanel = new UIWindow;
	UIManager::GetSingleton().GetRoot()->AddChild(mSSAOParamsPanel);

	mSSAOParamsPanel->InitGuiStyle(nullptr);
	mSSAOParamsPanel->SetTitle(L"SSAO Panel");
	mSSAOParamsPanel->SetPosition(position);
	mSSAOParamsPanel->SetSize(int2(300, 200));

	uint32_t uiY = 30;
	
	wchar_t sz[100];
	int sliderPos;
	swprintf(sz, 100, L"Radius %.2f", mAmbientOcclusion.mSettings.Radius);
	sliderPos = (mAmbientOcclusion.mSettings.Radius - MIN_SAMPLE_RADIUS) / (MAX_SAMPLE_RADIUS - MIN_SAMPLE_RADIUS) * 100;

	mRadiusLabel = new Label;
	mRadiusLabel->InitGuiStyle(nullptr);
	mRadiusLabel->SetFontSize(20.0f);
	mRadiusLabel->SetPosition(int2(8, uiY));
	mRadiusLabel->SetSize(int2(150, 20));
	mRadiusLabel->SetText(sz);
	mRadiusLabel->SetTextAlignment(AlignLeft | AlignVCenter);
	mSSAOParamsPanel->AddChild( mRadiusLabel );

	mRadiusSlider = new Slider(UI_Horizontal);
	mRadiusSlider->InitGuiStyle(nullptr);
	mRadiusSlider->SetPosition(int2(130 , uiY + 7));
	mRadiusSlider->SetTrackLength(150);
	mRadiusSlider->SetValue(sliderPos);
	mRadiusSlider->EventValueChanged.bind(this, &AmbientOcclusionPanel::RadiusChanged);
	mSSAOParamsPanel->AddChild( mRadiusSlider );  

	uiY += mRadiusLabel->GetSize().Y() + 5;
	swprintf(sz, 100, L"Bias %.3f", mAmbientOcclusion.mSettings.Bias);
	sliderPos = mAmbientOcclusion.mSettings.Bias * 100;

	mBiasLabel = new Label;
	mBiasLabel->InitGuiStyle(nullptr);
	mBiasLabel->SetFontSize(20.0f);
	mBiasLabel->SetPosition(int2(8, uiY));
	mBiasLabel->SetSize(int2(150, 20));
	mBiasLabel->SetTextAlignment(AlignLeft | AlignVCenter);
	mBiasLabel->SetText(sz);
	mSSAOParamsPanel->AddChild( mBiasLabel );

	mBiasSlider = new Slider(UI_Horizontal);
	mBiasSlider->InitGuiStyle(nullptr);
	mBiasSlider->SetPosition(int2(130 , uiY + 7));
	mBiasSlider->SetTrackLength(150);
	mBiasSlider->SetValue(sliderPos);
	mBiasSlider->EventValueChanged.bind(this, &AmbientOcclusionPanel::BiasChanged);
	mSSAOParamsPanel->AddChild( mBiasSlider );  

	//Detail AO
	uiY += mBiasLabel->GetSize().Y() + 5;
	swprintf(sz, 100, L"DetailAO %.2f", mAmbientOcclusion.mSettings.DetailAO);
	sliderPos = mAmbientOcclusion.mSettings.DetailAO * 50;

	mDetailAOLabel = new Label;
	mDetailAOLabel->InitGuiStyle(nullptr);
	mDetailAOLabel->SetFontSize(20.0f);
	mDetailAOLabel->SetPosition(int2(8, uiY));
	mDetailAOLabel->SetSize(int2(150, 20));
	mDetailAOLabel->SetTextAlignment(AlignLeft | AlignVCenter);
	mDetailAOLabel->SetText(sz);
	mSSAOParamsPanel->AddChild( mDetailAOLabel );

	mDetailAOSlider = new Slider(UI_Horizontal);
	mDetailAOSlider->InitGuiStyle(nullptr);
	mDetailAOSlider->SetPosition(int2(150 , uiY + 7));
	mDetailAOSlider->SetTrackLength(120);
	mDetailAOSlider->SetValue(sliderPos);
	mDetailAOSlider->EventValueChanged.bind(this, &AmbientOcclusionPanel::DetailAOChanged);
	mSSAOParamsPanel->AddChild( mDetailAOSlider );  

	// Coarse AO
	uiY += mDetailAOLabel->GetSize().Y() + 5;
	swprintf(sz, 100, L"CoarseAO %.2f", mAmbientOcclusion.mSettings.CoarseAO);
	sliderPos = mAmbientOcclusion.mSettings.CoarseAO * 50;

	mCoarseAOLabel = new Label;
	mCoarseAOLabel->InitGuiStyle(nullptr);
	mCoarseAOLabel->SetFontSize(20.0f);
	mCoarseAOLabel->SetPosition(int2(8, uiY));
	mCoarseAOLabel->SetSize(int2(150, 20));
	mCoarseAOLabel->SetTextAlignment(AlignLeft | AlignVCenter);
	mCoarseAOLabel->SetText(sz);
	mSSAOParamsPanel->AddChild( mCoarseAOLabel );

	mCoarseAOSlider = new Slider(UI_Horizontal);
	mCoarseAOSlider->InitGuiStyle(nullptr);
	mCoarseAOSlider->SetPosition(int2(150 , uiY + 7));
	mCoarseAOSlider->SetTrackLength(120);
	mCoarseAOSlider->SetValue(sliderPos);
	mCoarseAOSlider->EventValueChanged.bind(this, &AmbientOcclusionPanel::CoarseAOChanged);
	mSSAOParamsPanel->AddChild( mCoarseAOSlider );  

	// Coarse AO
	uiY += mCoarseAOLabel->GetSize().Y() + 5;
	swprintf(sz, 100, L"Units %.1f", mAmbientOcclusion.mSettings.MetersToViewSpaceUnits);
	sliderPos = mAmbientOcclusion.mSettings.MetersToViewSpaceUnits * 2;

	mSceneUnitsLabel = new Label;
	mSceneUnitsLabel->InitGuiStyle(nullptr);
	mSceneUnitsLabel->SetFontSize(20.0f);
	mSceneUnitsLabel->SetPosition(int2(8, uiY));
	mSceneUnitsLabel->SetSize(int2(150, 20));
	mSceneUnitsLabel->SetTextAlignment(AlignLeft | AlignVCenter);
	mSceneUnitsLabel->SetText(sz);
	mSSAOParamsPanel->AddChild( mSceneUnitsLabel );

	mSceneUnitslider = new Slider(UI_Horizontal);
	mSceneUnitslider->InitGuiStyle(nullptr);
	mSceneUnitslider->SetPosition(int2(150 , uiY + 7));
	mSceneUnitslider->SetTrackLength(120);
	mSceneUnitslider->SetValue(sliderPos);
	mSceneUnitslider->EventValueChanged.bind(this, &AmbientOcclusionPanel::SceneUnitsChanged);
	mSSAOParamsPanel->AddChild( mSceneUnitslider );  
}

void AmbientOcclusionPanel::RadiusChanged(int32_t value)
{
	float v = (value) / 100.0f;
	mAmbientOcclusion.mSettings.Radius = Lerp(MIN_SAMPLE_RADIUS, MAX_SAMPLE_RADIUS, v);

	wchar_t sz[100];
	swprintf(sz, 100, L"Radius %.2f", mAmbientOcclusion.mSettings.Radius);
	mRadiusLabel->SetText(sz);
}

void AmbientOcclusionPanel::BiasChanged(int32_t value)
{
	mAmbientOcclusion.mSettings.Bias = (value) / 100.0f;

	wchar_t sz[100];
	swprintf(sz, 100, L"Bias %.3f", mAmbientOcclusion.mSettings.Bias);
	mBiasLabel->SetText(sz);
}

void AmbientOcclusionPanel::DetailAOChanged(int32_t value)
{
	mAmbientOcclusion.mSettings.DetailAO = (value) / 50.0f;

	wchar_t sz[100];
	swprintf(sz, 100, L"DetailAO %.2f", mAmbientOcclusion.mSettings.DetailAO);
	mDetailAOLabel->SetText(sz);
}

void AmbientOcclusionPanel::CoarseAOChanged(int32_t value)
{
	mAmbientOcclusion.mSettings.CoarseAO = (value) / 50.0f;

	wchar_t sz[100];
	swprintf(sz, 100, L"CoarseAO %.2f", mAmbientOcclusion.mSettings.CoarseAO);
	mCoarseAOLabel->SetText(sz);
}

void AmbientOcclusionPanel::SceneUnitsChanged(int32_t value)
{
	mAmbientOcclusion.mSettings.MetersToViewSpaceUnits = (value) / 2.0f;

	wchar_t sz[100];
	swprintf(sz, 100, L"Units %.1f", mAmbientOcclusion.mSettings.MetersToViewSpaceUnits);
	mSceneUnitsLabel->SetText(sz);
}

}