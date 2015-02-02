#include "DeepGBufferRadiosity.h"
#include <MainApp/Application.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Material.h>
#include <Graphics/Camera.h>
#include <Graphics/Effect.h>
#include <Graphics/CascadedShadowMap.h>
#include <Graphics/AmbientOcclusion.h>
#include <GUI/UIManager.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Light.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Environment.h>
#include <Input/InputSystem.h>

/** Floating point bits per pixel for CSZ: 16 or 32. */
#define ZBITS (32)

/** This must be greater than or equal to the MAX_MIP_LEVEL and  defined in AmbientOcclusion_AO.pix. */
#define MAX_MIP_LEVEL (5)

namespace RcEngine {

#ifdef _DEBUG
	static uint32_t BufferAccessHint = EAH_CPU_Read | EAH_GPU_Read | EAH_GPU_Write;
#else
	static uint32_t BufferAccessHint = EAH_GPU_Read | EAH_GPU_Write;
#endif

//////////////////////////////////////////////////////////////////////////
void DeepGBufferRadiosity::GBuffer::CreateBuffers( uint32_t width, uint32_t height)
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	if (!mFrameBuffer)
	{
		mFrameBuffer = factory->CreateFrameBuffer(width, height);
	}
	else
	{
		mFrameBuffer->DetachAll();
		mFrameBuffer->Resize(width, height);
	}

	mTextures[GBuffer::DepthStencil] = factory->CreateTexture2D(
									width, height, 
									PF_D32F,
									1,  // layers
									1,  // levels
									1, 0, // samples
									BufferAccessHint,
									TexCreate_DepthStencilTarget | TexCreate_ShaderResource,
									NULL);
	
	mTextures[GBuffer::Lambertain] = factory->CreateTexture2D(
									width, height, 
									PF_RGBA8_UNORM, 
									1,  // layers
									1,  // levels
									1, 0, // samples
									BufferAccessHint,
									TexCreate_RenderTarget | TexCreate_ShaderResource,
									NULL);

	mTextures[GBuffer::Normal] = factory->CreateTexture2D(
									width, height, 
									PF_RGB10A2, //PF_RGB16F
									1,  // layers
									1,  // levels
									1, 0, // samples
									BufferAccessHint,
									TexCreate_RenderTarget | TexCreate_ShaderResource,
									NULL);

	mTextures[GBuffer::Glossy] = factory->CreateTexture2D(
									width, height, 
									PF_RGBA8_UNORM, 
									1,  // layers
									1,  // levels
									1, 0, // samples
									BufferAccessHint,
									TexCreate_RenderTarget | TexCreate_ShaderResource,
									NULL);

	mTextures[GBuffer::ScreenSpaceVelocity] = factory->CreateTexture2D(
									width, height, 
									PF_RG16F,
									1,  // layers
									1,  // levels
									1, 0, // samples
									BufferAccessHint,
									TexCreate_RenderTarget | TexCreate_ShaderResource,
									NULL);

	mFrameBuffer->AttachRTV(ATT_DepthStencil, factory->CreateDepthStencilView(mTextures[DepthStencil], 0, 0));
	mFrameBuffer->AttachRTV(ATT_Color0, factory->CreateRenderTargetView2D(mTextures[Lambertain], 0, 0));
	mFrameBuffer->AttachRTV(ATT_Color1, factory->CreateRenderTargetView2D(mTextures[Glossy], 0, 0));
	mFrameBuffer->AttachRTV(ATT_Color2, factory->CreateRenderTargetView2D(mTextures[Normal], 0, 0));
	mFrameBuffer->AttachRTV(ATT_Color3, factory->CreateRenderTargetView2D(mTextures[ScreenSpaceVelocity], 0, 0));
}

const shared_ptr<ShaderResourceView>& DeepGBufferRadiosity::GBuffer::GetTextureSRV( Field field ) const
{
	return mTextures[field]->GetShaderResourceView();
}

//////////////////////////////////////////////////////////////////////////
DeepGBufferRadiosity::MipmapGenerator::MipmapGenerator(const shared_ptr<Texture>& buffer, int maxLevel)
	: mMaxLevel(maxLevel)
{
	mDevice = Environment::GetSingleton().GetRenderDevice();

	if (Environment::GetSingleton().GetApplication()->GetAppSettings().RHDeviceType == RD_Direct3D11)
	{
		for (int i = 1; i <= maxLevel; ++i)
		{
			mLevelRTV.push_back( mDevice->GetRenderFactory()->CreateRenderTargetView2D(buffer, 0, i) );
			mPrevLevelSRV.push_back( mDevice->GetRenderFactory()->CreateTexture2DSRV(buffer, 0, i, 0, 1) );
		}
	}
}

void DeepGBufferRadiosity::MipmapGenerator::GenerateMipmap(const shared_ptr<FrameBuffer>& frameBuffer, const shared_ptr<Effect>& minifyEffect, int techIndex)
{
	uint32_t baseWidth = frameBuffer->GetWidth();
	uint32_t baseHeight = frameBuffer->GetHeight();

	uint32_t width = frameBuffer->GetWidth();
	uint32_t height = frameBuffer->GetHeight();

	for (uint32_t i = 1; i <= mMaxLevel; ++i)
	{
		minifyEffect->GetParameterByName("SourceMap")->SetValue( mPrevLevelSRV[i-1] ); 

		// last level size
		minifyEffect->GetParameterByName("PreviousMIP")->SetValue( int3(i-1, width, height) );

		// curr level size
		width = std::max(1U, width >> 1);
		height = std::max(1U, height >> 1);
		frameBuffer->Resize(width, height);

		frameBuffer->AttachRTV(ATT_Color0, mLevelRTV[i-1]);
		mDevice->BindFrameBuffer(frameBuffer);
		mDevice->DrawFSTriangle(minifyEffect->GetTechniqueByIndex(techIndex));
	}

	frameBuffer->Resize(baseWidth, baseHeight);
}

//////////////////////////////////////////////////////////////////////////
DeepGBufferRadiosity::DeepGBufferRadiosity(void)
{
}

DeepGBufferRadiosity::~DeepGBufferRadiosity(void)
{
}

PixelFormat DeepGBufferRadiosity::GetCSZBufferFormat(bool twoChannel) const
{
	return twoChannel ? ((ZBITS == 16) ? PF_RG16F : PF_RG32F)
		: ((ZBITS == 16) ? PF_R16F : PF_R32F);
}

PixelFormat DeepGBufferRadiosity::GetColorFormat(bool useHalfPrecisionColor) const
{
	return useHalfPrecisionColor ? PF_RGBA16F : PF_RGBA32F;
}

PixelFormat DeepGBufferRadiosity::GetNormalFormat(bool useOct16) const
{
	return useOct16 ? PF_RGBA8_UNORM : PF_RGB10A2;
}

const shared_ptr<Texture> DeepGBufferRadiosity::GetRadiosityTexture() const
{
	//return mTempFiltedResultBuffer;

	if (mSettings.BlurRadius != 0) 
	{
		return mResultBuffer;
	} 
	else
	{ // No blur passes, so pull out the raw buffer!
		return mTempFiltedResultBuffer;
	}
}

uint32_t DeepGBufferRadiosity::NumSpiralTurns() const
{
#define NUM_PRECOMPUTED 100

	static int minDiscrepancyArray[NUM_PRECOMPUTED] = {
		//  0   1   2   3   4   5   6   7   8   9
		1,  1,  1,  2,  3,  2,  5,  2,  3,  2,  // 0
		3,  3,  5,  5,  3,  4,  7,  5,  5,  7,  // 1
		9,  8,  5,  5,  7,  7,  7,  8,  5,  8,  // 2
		11, 12,  7, 10, 13,  8, 11,  8,  7, 14,  // 3
		11, 11, 13, 12, 13, 19, 17, 13, 11, 18,  // 4
		19, 11, 11, 14, 17, 21, 15, 16, 17, 18,  // 5
		13, 17, 11, 17, 19, 18, 25, 18, 19, 19,  // 6
		29, 21, 19, 27, 31, 29, 21, 18, 17, 29,  // 7
		31, 31, 23, 18, 25, 26, 25, 23, 19, 34,  // 8
		19, 27, 21, 25, 39, 29, 17, 21, 27, 29}; // 9

	if (mSettings.NumSamples < NUM_PRECOMPUTED) {
		return minDiscrepancyArray[mSettings.NumSamples];
	} else {
		return 5779; // Some large prime. Hope it does alright. It'll at least never degenerate into a perfect line until we have 5779 samples...
	}

#undef NUM_PRECOMPUTED
}

void DeepGBufferRadiosity::SetEnvironmentLightingProbe(const shared_ptr<Texture>& cubeEnvLightMap)
{
	mEnvLightProbeMap = cubeEnvLightMap;
}

void DeepGBufferRadiosity::OnGraphicsInit(const shared_ptr<Camera>& camera)
{
	RenderPath::OnGraphicsInit(camera);

	/*mSettings.Radius = 7.4f;
	mSettings.DepthPeelSeparationHint = 1.6f;
	mSettings.SaturatedBoost = 2.0f;
	mSettings.UnsaturatedBoost = 1.0f;*/

	Window* appWindow = Application::msApp->GetMainWindow();
	const uint32_t windowWidth = appWindow->GetWidth();
	const uint32_t windowHeight = appWindow->GetHeight();

	// Load Effect 
	ResourceManager& resMan = ResourceManager::GetSingleton();
	mBlitEffect = resMan.GetResourceByName<Effect>(RT_Effect, "FSQuad.effect.xml", "General");	
	mMinifyEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/MipmapGen.effect.xml", "General");
	mLambertianOnlyEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/LambertianOnly.effect.xml", "General");
	mRadiosityEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/DeepGBufferRadiosity.effect.xml", "General");
	mReconstrctCSZEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/Reconstruct.effect.xml", "General");
	mDeepGBufferShadingEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/DeepGBufferShading.effect.xml", "General");
	mTemporalFilterEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/TemporalFilter.effect.xml", "General");
	mRadiosityBlurEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/DeepGBufferBlur.effect.xml", "General");
	
	// Create Buffers
	CreateBuffers(windowWidth, windowHeight);

	mShadowMan = std::make_shared<CascadedShadowMap>(mDevice);
	mAmbientOcclusion = std::make_shared<AmbientOcclusion>(mDevice, SSAO_HBAO, windowWidth, windowHeight);
}

void DeepGBufferRadiosity::CreateBuffers( uint32_t width, uint32_t height )
{
	mBufferWidth = width;
	mBufferHeight = height;

	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;
	mViewportMatrix = float4x4(halfWidth,		0,			0,		0,
								  0,       -halfHeight,		0,		0,
								  0,			0,			1,		0,
							   halfWidth,   halfHeight,		0,      1);

	// DeepGBuffer
	mGBuffer.CreateBuffers(width, height);
	mPeeledGBuffer.CreateBuffers(width, height);

	// Bind view camera
	mGBuffer.mFrameBuffer->SetCamera(mCamera);
	mPeeledGBuffer.mFrameBuffer->SetCamera(mCamera);

	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	if (!mFrameBuffer)
	{
		mFrameBuffer = factory->CreateFrameBuffer(width, height);
	}
	else
	{
		mFrameBuffer->DetachAll();
		mFrameBuffer->Resize(width, height);
	}

	PixelFormat colorFormat = mSettings.UseHalfPrecisionColors ? PF_RGBA16F : PF_RGBA32F;
	const uint32_t texCreateFlags = TexCreate_RenderTarget | TexCreate_ShaderResource;

	uint32_t numLevel = (mSettings.Enabled && mSettings.UseMipMaps) ? MAX_MIP_LEVEL+1 : 1;

	mLambertDirectBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, numLevel, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mLambertDirectRTV = factory->CreateRenderTargetView2D(mLambertDirectBuffer, 0, 0);
	mLambertMipmapGen = std::make_shared<MipmapGenerator>(mLambertDirectBuffer, MAX_MIP_LEVEL);

	mPeeledLambertDirectBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, numLevel, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mPeeledLambertDirectRTV = factory->CreateRenderTargetView2D(mPeeledLambertDirectBuffer, 0, 0);
	mPeeledLambertMipmapGen = std::make_shared<MipmapGenerator>(mPeeledLambertDirectBuffer, MAX_MIP_LEVEL);

	mRawIIBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mPreviousRawIIBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mRawIIRTV = factory->CreateRenderTargetView2D(mRawIIBuffer, 0, 0);

	mResultBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mResultRTV = factory->CreateRenderTargetView2D(mResultBuffer, 0, 0);

	mTempFiltedResultBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mTempFiltedResultRTV = factory->CreateRenderTargetView2D(mTempFiltedResultBuffer, 0, 0); 

	mTempBlurBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0, BufferAccessHint, texCreateFlags, NULL);
	mTempBlurRTV = factory->CreateRenderTargetView2D(mTempBlurBuffer, 0, 0);

	//------
	mHDRBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0, BufferAccessHint, texCreateFlags, NULL);
	mHDRBufferRTV = factory->CreateRenderTargetView2D(mHDRBuffer, 0, 0);

	mCSZBuffer = factory->CreateTexture2D(width, height, GetCSZBufferFormat(mSettings.UseDepthPeelBuffer), 1, numLevel, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mCSZRTV = factory->CreateRenderTargetView2D(mCSZBuffer, 0, 0);
	mCSZMipmapGen = std::make_shared<MipmapGenerator>(mCSZBuffer, MAX_MIP_LEVEL);

	// Bind shader parameter for SRV
	mReconstrctCSZEffect->GetParameterByName("DepthBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::DepthStencil));
	mReconstrctCSZEffect->GetParameterByName("PeeledDepthBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::DepthStencil));
	mReconstrctCSZEffect->GetParameterByName("NormalBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));

	mRadiosityEffect->GetParameterByName("CSZBuffer")->SetValue(mCSZBuffer->GetShaderResourceView());
	mRadiosityEffect->GetParameterByName("NormalBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));
	mRadiosityEffect->GetParameterByName("PeeledNormalBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::Normal));
	mRadiosityEffect->GetParameterByName("BounceBuffer")->SetValue(mLambertDirectBuffer->GetShaderResourceView());
	mRadiosityEffect->GetParameterByName("PeeledBounceBuffer")->SetValue(mPeeledLambertDirectBuffer->GetShaderResourceView());

	mDeepGBufferShadingEffect->GetParameterByName("GBufferLambertain")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Lambertain));
	mDeepGBufferShadingEffect->GetParameterByName("GBufferGossly")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Glossy));
	mDeepGBufferShadingEffect->GetParameterByName("GBufferNormal")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));
	mDeepGBufferShadingEffect->GetParameterByName("GBufferDepth")->SetValue(mGBuffer.GetTextureSRV(GBuffer::DepthStencil));
}

void DeepGBufferRadiosity::OnWindowResize(uint32_t width, uint32_t height)
{
	CreateBuffers(width, height);
}

void DeepGBufferRadiosity::RenderScene()
{
	Prepare();	
	ComputeShadows();
	RenderGBuffers();

	if (mAmbientOcclusion)
	{
		mAmbientOcclusion->Apply(*mCamera, mGBuffer.GetTexture(GBuffer::DepthStencil));
		//mAmbientOcclusion->Visualize(mAmbientOcclusion->GetAmbientOcclusionSRV());
		//return;
	}

	RenderLambertianOnly();
	RenderIndirectIllumination();
	
	static int i = 0;
	static int j = 0;
	//String filename = "E:/DeepGBuffer/LambertDirect" + std::to_string(i++) + "_.pfm";
//	mDevice->GetRenderFactory()->SaveTextureToFile(filename, mLambertDirectBuffer);
	//filename = "E:/DeepGBuffer/rawII" + std::to_string(j++) + "_.pfm";
	//mDevice->GetRenderFactory()->SaveTextureToFile(filename, mRawIIBuffer);

	//String filename = "E:/DeepGBuffer/tempRawII" + std::to_string(i++) + ".pfm";
	//mDevice->GetRenderFactory()->SaveTextureToFile(filename, mTempFiltedResultBuffer);
	//mDevice->GetRenderFactory()->SaveTextureToFile("E:/DeepGBuffer/light.pfm", mLambertDirectBuffer);
	//mDevice->GetRenderFactory()->SaveTextureToFile("E:/DeepGBuffer/rawII.pfm", mRawIIBuffer);
	//mDevice->GetRenderFactory()->SaveTextureToFile("E:/DeepGBuffer/result.pfm", mResultBuffer);
	
	//mFrameBuffer->AttachRTV(ATT_Color0, mHDRBufferRTV);
	//mDevice->BindFrameBuffer(mFrameBuffer);
	//mFrameBuffer->Clear(CF_Color, ColorRGBA::Black, 1.0, 0);

	// Final shading
	shared_ptr<FrameBuffer> screenFrameBuffer = mDevice->GetScreenFrameBuffer();
	mDevice->BindFrameBuffer(screenFrameBuffer);
	screenFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0f, 0);
	
	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Lambertain));
	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(mLambertDirectBuffer->GetShaderResourceView());
	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(mPeeledLambertDirectBuffer->GetShaderResourceView());
	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(/*GetRadiosityTexture()*/mRawIIBuffer->GetShaderResourceView());
	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(mHDRBuffer->GetShaderResourceView());
	//mDevice->DrawFSTriangle(mBlitEffect->GetTechniqueByName("BlitColor"));

	ForwardShading();
	DeferredShading();

	mRawIIBuffer->CopyToTexture(*mPreviousRawIIBuffer);
	if (mSettings.Enabled && mSettings.PropagationDamping < 1.0f)
		mGBuffer.GetTexture(GBuffer::DepthStencil)->CopyToTexture(*mPreviousDepthBuffer);

	mPrevViewMatrix = mCamera->GetViewMatrix();
	mPrevInvViewMatrix = mInvViewMatrix;



	// Update overlays
	UIManager& uiMan = UIManager::GetSingleton();
	uiMan.Render();

	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
	sceneMan->UpdateOverlayQueue();

	const RenderBucket& guiBucket = sceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay, false);
	for (const RenderQueueItem& renderItem : guiBucket)
		renderItem.Renderable->Render();
}

void DeepGBufferRadiosity::Prepare()
{
	const float4x4& proj = mCamera->GetProjMatrix();

	mProjInfo = float4(2.0f/(mBufferWidth*proj.M11), -2.0f/(mBufferHeight*proj.M22), -1.0f/proj.M11, 1.0f/proj.M22);
	mClipInfo = float2(proj.M33, proj.M43);
	mProjScale = 0.5f * proj.M22 * mBufferHeight;

	mInvViewMatrix = MatrixInverse(mCamera->GetViewMatrix());
}

void DeepGBufferRadiosity::ComputeShadows()
{
	mSceneMan->UpdateLightQueue(*mCamera);

	Light* mainDirLight = mSceneMan->GetLightQueue().front();
	assert(mainDirLight->GetLightType() == LT_DirectionalLight);
	mShadowMan->MakeCascadedShadowMap(*mainDirLight);
}

void DeepGBufferRadiosity::RenderGBuffers()
{
	// Todo: update render queue with render bucket filter
	mSceneMan->UpdateRenderQueue(mCamera, RO_None, RenderQueue::BucketAll, 0);   
	const RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);

	float4x4 projToScreenMatrix = mCamera->GetProjMatrix() * mViewportMatrix;

	mDevice->BindFrameBuffer(mGBuffer.mFrameBuffer);
	mGBuffer.mFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0f, 0);

	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		shared_ptr<Effect> effect = renderItem.Renderable->GetMaterial()->GetEffect();
		
		effect->SetCurrentTechnique("DeepGBuffer");
		effect->GetParameterByName("PrevView")->SetValue(mPrevViewMatrix);
		effect->GetParameterByName("ProjectionToScreenMatrix")->SetValue(projToScreenMatrix);
		renderItem.Renderable->Render();
	}

	mDevice->BindFrameBuffer(mPeeledGBuffer.mFrameBuffer);
	mPeeledGBuffer.mFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0f, 0);

	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		shared_ptr<Effect> effect = renderItem.Renderable->GetMaterial()->GetEffect();
		effect->SetCurrentTechnique("PeeledDeepGBuffer");
		effect->GetParameterByName("ClipInfo")->SetValue(mClipInfo);
		effect->GetParameterByName("MinZSeparation")->SetValue(mSettings.DepthPeelSeparationHint);
		effect->GetParameterByName("PrevDepthBuffer")->SetValue( mGBuffer.GetTextureSRV(GBuffer::DepthStencil) );

		renderItem.Renderable->Render();
	}

	RenderFactory* factory = mDevice->GetRenderFactory();
	//factory->SaveTextureToFile("E:/DeepGBuffer_Lambertain.tga", mGBuffer.GetTexture(GBuffer::Lambertain));
	//factory->SaveTextureToFile("E:/DeepGBuffer_SSVelocity.pfm", mGBuffer.GetTexture(GBuffer::ScreenSpaceVelocity));
	//factory->SaveTextureToFile("E:/DeepGBuffer_Normal.pfm", mGBuffer.GetTexture(GBuffer::Normal));
}

void DeepGBufferRadiosity::RenderLambertianOnly()
{
	Light* mainDirLight = mSceneMan->GetLightQueue().front();
	assert(mainDirLight->GetLightType() == LT_DirectionalLight);

	float3 lightColor = mainDirLight->GetLightColor() * mainDirLight->GetLightIntensity();
	
	const float4x4& View = mCamera->GetViewMatrix();
	const float3& lightDirection = mainDirLight->GetDerivedDirection();
	float4 lightDirCS = float4(lightDirection.X(), lightDirection.Y(), lightDirection.Z(), 0.0) * View;
	
	mLambertianOnlyEffect->GetParameterByName("InvView")->SetValue(mInvViewMatrix);
	mLambertianOnlyEffect->GetParameterByName("PrevoiusInvView")->SetValue(mPrevInvViewMatrix);
	mLambertianOnlyEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
	mLambertianOnlyEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
	mLambertianOnlyEffect->GetParameterByName("ClipInfo")->SetValue(mClipInfo);
	mLambertianOnlyEffect->GetParameterByName("LightDirection")->SetValue(float3(lightDirCS.X(), lightDirCS.Y(), lightDirCS.Z()));
	mLambertianOnlyEffect->GetParameterByName("LightColor")->SetValue(lightColor);
	mLambertianOnlyEffect->GetParameterByName("PropagationDamping")->SetValue(mSettings.PropagationDamping);
	mLambertianOnlyEffect->GetParameterByName("InvViewport")->SetValue(float2(1.0f / mBufferWidth, 1.0f / mBufferHeight));
	mLambertianOnlyEffect->GetParameterByName("LightBoost")->SetValue(float2(mSettings.UnsaturatedBoost, mSettings.SaturatedBoost));

	// Setup shadow
	mLambertianOnlyEffect->GetConstantBuffer("cbPossionDiskSamples")->SetBuffer(mShadowMan->mPossionSamplesCBuffer);
	mLambertianOnlyEffect->GetParameterByName("CascadeShadowMap")->SetValue( mShadowMan->mShadowTexture->GetShaderResourceView());
	mLambertianOnlyEffect->GetParameterByName("LightView")->SetValue(mShadowMan->mLightViewMatrix);
	mLambertianOnlyEffect->GetParameterByName("NumCascades")->SetValue((int)mainDirLight->GetShadowCascades());
	mLambertianOnlyEffect->GetParameterByName("BorderPaddingMinMax")->SetValue(mShadowMan->mBorderPaddingMinMax);
	mLambertianOnlyEffect->GetParameterByName("CascadeScale")->SetValue(&mShadowMan->mShadowCascadeScale[0], MAX_CASCADES);
	mLambertianOnlyEffect->GetParameterByName("CascadeOffset")->SetValue(&mShadowMan->mShadowCascadeOffset[0], MAX_CASCADES); 
	mLambertianOnlyEffect->GetParameterByName("InvShadowMapSize")->SetValue(1.0f / SHADOW_MAP_SIZE);


	EffectTechnique* lambertianTech = mLambertianOnlyEffect->GetTechniqueByIndex(0);
	if (mSettings.Enabled && mSettings.PropagationDamping < 1.0f)
	{
		// Use previous frame's bounces as input into the radiosity iteration
		if (!mPreviousDepthBuffer || mPreviousDepthBuffer->GetWidth() != mBufferWidth || mPreviousDepthBuffer->GetHeight() != mBufferHeight)
		{
			mPreviousDepthBuffer = mDevice->GetRenderFactory()->CreateTexture2D(mBufferWidth, mBufferHeight, 
				PF_D32F,
				1,
				1,
				1, 0, 
				BufferAccessHint, 
				TexCreate_DepthStencilTarget | TexCreate_ShaderResource, 
				NULL);

			mLambertianOnlyEffect->GetParameterByName("PrevDepthBuffer")->SetValue( mPreviousDepthBuffer->GetShaderResourceView() );
			mLambertianOnlyEffect->GetParameterByName("PrevIndirectRadiosityBuffer")->SetValue( GetRadiosityTexture()->GetShaderResourceView() );
		}
		
		lambertianTech = mLambertianOnlyEffect->GetTechniqueByIndex(1);
	}

	
	// Compute initial radiosity for first layer
	{
		mLambertianOnlyEffect->GetParameterByName("GBufferLambertain")->SetValue( mGBuffer.GetTextureSRV(GBuffer::Lambertain) );
		mLambertianOnlyEffect->GetParameterByName("GBufferNormal")->SetValue( mGBuffer.GetTextureSRV(GBuffer::Normal) );
		mLambertianOnlyEffect->GetParameterByName("GBufferSSVelocity")->SetValue( mGBuffer.GetTextureSRV(GBuffer::ScreenSpaceVelocity) );
		mLambertianOnlyEffect->GetParameterByName("GBufferDepth")->SetValue( mGBuffer.GetTextureSRV(GBuffer::DepthStencil) );

		mFrameBuffer->AttachRTV(ATT_Color0, mLambertDirectRTV);
		mDevice->BindFrameBuffer(mFrameBuffer);
		mFrameBuffer->Clear(CF_Color, ColorRGBA(0, 0, 0, 0), 1.0f, 0);
		
		mDevice->DrawFSTriangle(lambertianTech);
	}

	// Compute initial radiosity for peeled layer
	{
		mLambertianOnlyEffect->GetParameterByName("GBufferLambertain")->SetValue( mPeeledGBuffer.GetTextureSRV(GBuffer::Lambertain) );
		mLambertianOnlyEffect->GetParameterByName("GBufferNormal")->SetValue( mPeeledGBuffer.GetTextureSRV(GBuffer::Normal) );
		mLambertianOnlyEffect->GetParameterByName("GBufferSSVelocity")->SetValue( mPeeledGBuffer.GetTextureSRV(GBuffer::ScreenSpaceVelocity) );
		mLambertianOnlyEffect->GetParameterByName("GBufferDepth")->SetValue( mPeeledGBuffer.GetTextureSRV(GBuffer::DepthStencil) );

		mFrameBuffer->AttachRTV(ATT_Color0, mPeeledLambertDirectRTV);
		mDevice->BindFrameBuffer(mFrameBuffer);
		mFrameBuffer->Clear(CF_Color, ColorRGBA(0, 0, 0, 0), 1.0f, 0);

		mDevice->DrawFSTriangle(lambertianTech);
	}
}

void DeepGBufferRadiosity::RenderIndirectIllumination()
{
	if (mSettings.Enabled)
	{
		// Step 1, Compute mipmaped buffer, (CSZBuffer, PreviousBounceBuffer, NormalBuffer) 
		ComputeMipmapedBuffers();

		// Step 2, Compute indirect light bounce (one bounce per frame)
		ComputeRawII();

		// Step 3, Temporal filtering 
		TemporalFiltering();

		// Step, Blur
		RadiosityBlur();
	}
}

void DeepGBufferRadiosity::ComputeMipmapedBuffers()
{
	EffectTechnique* reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(0);
	
	// Step 1, build camera space zbuffer, and pack normals using oct16 if enabled
	mFrameBuffer->AttachRTV(ATT_Color0, mCSZRTV);
	if (mSettings.UseDepthPeelBuffer)
	{
		if (mSettings.UseOct16)
		{
			// lazy created if needed
			if (!mPackedNormalBuffer || mPackedNormalBuffer->GetWidth() != mBufferWidth || mPackedNormalBuffer->GetHeight() != mBufferHeight)
			{
				RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

				const uint32_t numLevel = (mSettings.Enabled && mSettings.UseMipMaps) ? MAX_MIP_LEVEL+1 : 1;
				const uint32_t texCreateFlags = TexCreate_RenderTarget | TexCreate_ShaderResource;

				mPackedNormalBuffer = factory->CreateTexture2D(mBufferWidth, mBufferHeight, GetNormalFormat(mSettings.UseOct16), 1, numLevel, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
				mPackedNormalRTV = factory->CreateRenderTargetView2D(mPackedNormalBuffer, 0, 0);
				mPackedNormalMipmapGen = std::make_shared<MipmapGenerator>(mPackedNormalBuffer, MAX_MIP_LEVEL);

				// Bind shader parameter for SRV
				mReconstrctCSZEffect->GetParameterByName("PeeledNormalBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::Normal));
				mRadiosityEffect->GetParameterByName("NormalBuffer")->SetValue(mPackedNormalBuffer->GetShaderResourceView());
			}

			reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(2);
			mFrameBuffer->AttachRTV(ATT_Color2, mPackedNormalRTV);
		}
		else
		{
			mRadiosityEffect->GetParameterByName("NormalBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));
			reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(1);
		}
	}

	mReconstrctCSZEffect->GetParameterByName("ClipInfo")->SetValue(mClipInfo);
	mDevice->BindFrameBuffer(mFrameBuffer);
	mDevice->DrawFSTriangle(reconstrctTech);

	// Step2, Generate mipmap 
	if (mSettings.UseMipMaps)
	{
		mFrameBuffer->DetachAll();
		mCSZMipmapGen->GenerateMipmap(mFrameBuffer, mMinifyEffect, 1);
		mLambertMipmapGen->GenerateMipmap(mFrameBuffer, mMinifyEffect, 1);
		if (mSettings.Enabled && mSettings.UseOct16)
		{
			mPackedNormalMipmapGen->GenerateMipmap(mFrameBuffer, mMinifyEffect, 1);
			mPeeledLambertMipmapGen->GenerateMipmap(mFrameBuffer, mMinifyEffect, 1);
		}
	}
}

void DeepGBufferRadiosity::ComputeRawII()
{
	mRadiosityEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
	mRadiosityEffect->GetParameterByName("ProjScale")->SetValue(mProjScale);
	mRadiosityEffect->GetParameterByName("TextureBound")->SetValue(int4(0, 0, mBufferWidth-1, mBufferHeight-1));
	mRadiosityEffect->GetParameterByName("Radius")->SetValue(mSettings.Radius);
	mRadiosityEffect->GetParameterByName("Radius2")->SetValue(mSettings.Radius * mSettings.Radius);
	mRadiosityEffect->GetParameterByName("GameTime")->SetValue(0.0f);

	EffectTechnique* radiosityTech = mRadiosityEffect->GetTechniqueByIndex(0);
	if (mSettings.UseDepthPeelBuffer)
	{
		if (!mSettings.UseOct16)
		{
			radiosityTech = mRadiosityEffect->GetTechniqueByIndex(1);
		}
		else
		{
			radiosityTech = mRadiosityEffect->GetTechniqueByIndex(2);
		}
	}

	mFrameBuffer->AttachRTV(ATT_Color0, mRawIIRTV);
	mDevice->BindFrameBuffer(mFrameBuffer);
	mDevice->DrawFSTriangle(radiosityTech);
}

void DeepGBufferRadiosity::TemporalFiltering()
{
	mTemporalFilterEffect->GetParameterByName("UnfilteredValueBuffer")->SetValue(mRawIIBuffer->GetShaderResourceView());
	mTemporalFilterEffect->GetParameterByName("PreviousValueBuffer")->SetValue(mPreviousRawIIBuffer->GetShaderResourceView());
	mTemporalFilterEffect->GetParameterByName("PreviousDepthBuffer")->SetValue(mPreviousDepthBuffer->GetShaderResourceView());
	mTemporalFilterEffect->GetParameterByName("DepthBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::DepthStencil));
	mTemporalFilterEffect->GetParameterByName("SSVelocityBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::ScreenSpaceVelocity));

	mTemporalFilterEffect->GetParameterByName("InvView")->SetValue(mInvViewMatrix);
	mTemporalFilterEffect->GetParameterByName("PreviousInvView")->SetValue(mPrevInvViewMatrix);
	mTemporalFilterEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
	mTemporalFilterEffect->GetParameterByName("ClipInfo")->SetValue(mClipInfo);

	float2 BufferSize(float(mBufferWidth-1), float(mBufferHeight-1));
	mTemporalFilterEffect->GetParameterByName("BufferSize")->SetValue(BufferSize);

	mTemporalFilterEffect->GetParameterByName("Alpha")->SetValue(mSettings.TemporalFilterSettings.Alpha);
	mTemporalFilterEffect->GetParameterByName("FalloffStartDistance")->SetValue(mSettings.TemporalFilterSettings.FalloffStartDistance);
	mTemporalFilterEffect->GetParameterByName("FalloffEndDistance")->SetValue(mSettings.TemporalFilterSettings.FalloffEndDistance);

	mFrameBuffer->AttachRTV(ATT_Color0, mTempFiltedResultRTV);
	mDevice->BindFrameBuffer(mFrameBuffer);
	mDevice->DrawFSTriangle(mTemporalFilterEffect->GetTechniqueByIndex(0));	
}

void DeepGBufferRadiosity::RadiosityBlur()
{
	if (mSettings.BlurRadius != 0)
	{
		//alwaysAssertM(settings.blurRadius >= 0, "The AO blur radius must be a nonnegative number/");
		//alwaysAssertM(settings.blurStepSize > 0, "Must use a positive blur step size");

		mRadiosityBlurEffect->GetParameterByName("CSZBuffer")->SetValue( mCSZBuffer->GetShaderResourceView() );
		mRadiosityBlurEffect->GetParameterByName("NormalBuffer")->SetValue( mGBuffer.GetTextureSRV(GBuffer::Normal) );
		mRadiosityBlurEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
		
		
		// Horizontal
		mFrameBuffer->AttachRTV(ATT_Color0, mTempBlurRTV);
		mDevice->BindFrameBuffer(mFrameBuffer);

		mRadiosityBlurEffect->GetParameterByName("Axis")->SetValue(int2(1, 0)); 
		mRadiosityBlurEffect->GetParameterByName("SourceBuffer")->SetValue( mTempFiltedResultBuffer->GetShaderResourceView() );
		mDevice->DrawFSTriangle(mRadiosityBlurEffect->GetTechniqueByIndex(0));

		// Vertical
		mFrameBuffer->AttachRTV(ATT_Color0, mResultRTV);
		mDevice->BindFrameBuffer(mFrameBuffer);

		mRadiosityBlurEffect->GetParameterByName("Axis")->SetValue(int2(0, 1)); 
		mRadiosityBlurEffect->GetParameterByName("SourceBuffer")->SetValue( mTempBlurBuffer->GetShaderResourceView() );
		mDevice->DrawFSTriangle(mRadiosityBlurEffect->GetTechniqueByIndex(0));
	} // else the result is still in the rawAOBuffer 
}

void DeepGBufferRadiosity::ForwardShading()
{
	// Draw Sky box first
	const RenderBucket& bkgBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketBackground, false);
	for (const RenderQueueItem& item : bkgBucket)
		item.Renderable->Render();
}

void DeepGBufferRadiosity::DeferredShading()
{
	EffectTechnique* deferredTech = mDeepGBufferShadingEffect->GetTechniqueByIndex(1);

	mDeepGBufferShadingEffect->GetParameterByName("ProjInfo")->SetValue(mProjInfo);
	mDeepGBufferShadingEffect->GetParameterByName("ClipInfo")->SetValue(mClipInfo);
	mDeepGBufferShadingEffect->GetParameterByName("LightBoost")->SetValue(float2(mSettings.UnsaturatedBoost, mSettings.SaturatedBoost));
	mDeepGBufferShadingEffect->GetParameterByName("IndirectRadiosity")->SetValue(GetRadiosityTexture()->GetShaderResourceView());

	// Setup light
	const LightQueue& sceneLights = mSceneMan->GetLightQueue();

	Light* mainDirLight = sceneLights.front();
	assert(mainDirLight->GetLightType() == LT_DirectionalLight);

	float3 lightColor = mainDirLight->GetLightColor() * mainDirLight->GetLightIntensity();

	mDeepGBufferShadingEffect->GetParameterByName("LightDirection")->SetValue(mainDirLight->GetDerivedDirection());
	mDeepGBufferShadingEffect->GetParameterByName("LightColor")->SetValue(lightColor);
	mDeepGBufferShadingEffect->GetParameterByName("EnableSSAO")->SetValue(true);
	mDeepGBufferShadingEffect->GetParameterByName("AmbientOcclusion")->SetValue(mAmbientOcclusion->GetAmbientOcclusionSRV());

	const float envGlossyMIPConstant = log2f(mEnvLightProbeMap->GetWidth() * sqrtf(3));
	mDeepGBufferShadingEffect->GetParameterByName("EnvGlossyMIPConstant")->SetValue(envGlossyMIPConstant);
	mDeepGBufferShadingEffect->GetParameterByName("EnvironmentMap")->SetValue(mEnvLightProbeMap->GetShaderResourceView());

	// Setup shadow
	mDeepGBufferShadingEffect->GetParameterByName("InvView")->SetValue(mInvViewMatrix);
	mDeepGBufferShadingEffect->GetConstantBuffer("cbPossionDiskSamples")->SetBuffer(mShadowMan->mPossionSamplesCBuffer);
	mDeepGBufferShadingEffect->GetParameterByName("CascadeShadowMap")->SetValue( mShadowMan->mShadowTexture->GetShaderResourceView());
	mDeepGBufferShadingEffect->GetParameterByName("LightView")->SetValue(mShadowMan->mLightViewMatrix);
	mDeepGBufferShadingEffect->GetParameterByName("NumCascades")->SetValue((int)mainDirLight->GetShadowCascades());
	mDeepGBufferShadingEffect->GetParameterByName("BorderPaddingMinMax")->SetValue(mShadowMan->mBorderPaddingMinMax);
	mDeepGBufferShadingEffect->GetParameterByName("CascadeScale")->SetValue(&mShadowMan->mShadowCascadeScale[0], MAX_CASCADES);
	mDeepGBufferShadingEffect->GetParameterByName("CascadeOffset")->SetValue(&mShadowMan->mShadowCascadeOffset[0], MAX_CASCADES); 
	mDeepGBufferShadingEffect->GetParameterByName("InvShadowMapSize")->SetValue(1.0f / SHADOW_MAP_SIZE);
	

	mDevice->DrawFSTriangle(deferredTech);
}

}