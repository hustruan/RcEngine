#include "DeepGBufferRadiosity.h"
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Material.h>
#include <Graphics/Camera.h>
#include <Graphics/Effect.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Light.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Environment.h>

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

void DeepGBufferRadiosity::OnGraphicsInit(const shared_ptr<Camera>& camera)
{
	RenderPath::OnGraphicsInit(camera);

	Window* appWindow = Application::msApp->GetMainWindow();
	const uint32_t windowWidth = appWindow->GetWidth();
	const uint32_t windowHeight = appWindow->GetHeight();

	ResourceManager& resMan = ResourceManager::GetSingleton();
	mBlitEffect = resMan.GetResourceByName<Effect>(RT_Effect, "FSQuad.effect.xml", "General");	
	mLambertianOnlyEffect = resMan.GetResourceByName<Effect>(RT_Effect, "LambertianOnly.effect.xml", "General");
	mRadiosityEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/DeepGBufferRadiosity.effect.xml", "General");
	mReconstrctCSZEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeepGBufferRadiosity/Reconstruct.effect.xml", "General");
	CreateBuffers(windowWidth, windowHeight);
}

void DeepGBufferRadiosity::CreateBuffers( uint32_t width, uint32_t height )
{
	mBufferWidth = width;
	mBufferHeight = height;

	// Viewport matrix
	mViewportMatrix = float4x4(width * 0.5f,	0,				0,		0,
		0,            	height *-0.5f,	0,		0,
		0,				0,				1,		0,
		width * 0.5f,    height *0.5f,   0,      1);

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

	mLambertDirectBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mLambertDirectRTV = factory->CreateRenderTargetView2D(mLambertDirectBuffer, 0, 0);

	mPeeledLambertDirectBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mPeeledLambertDirectRTV = factory->CreateRenderTargetView2D(mPeeledLambertDirectBuffer, 0, 0);

	mRawIIBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mRawIIRTV = factory->CreateRenderTargetView2D(mRawIIBuffer, 0, 0);

	mResultBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mTempFiltedResultBuffer = factory->CreateTexture2D(width, height, colorFormat, 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);

	mCSZBuffer = factory->CreateTexture2D(width, height, GetCSZBufferFormat(mSettings.UseDepthPeelBuffer), 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
	mCSZRTV = factory->CreateRenderTargetView2D(mCSZBuffer, 0, 0);

	// Bind shader parameter for SRV
	mReconstrctCSZEffect->GetParameterByName("DepthBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::DepthStencil));
	mReconstrctCSZEffect->GetParameterByName("PeeledDepthBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::DepthStencil));
	mReconstrctCSZEffect->GetParameterByName("NormalBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));

	mRadiosityEffect->GetParameterByName("CSZBuffer")->SetValue(mCSZBuffer->GetShaderResourceView());
	mRadiosityEffect->GetParameterByName("BounceBuffer")->SetValue(mLambertDirectBuffer->GetShaderResourceView());
	mRadiosityEffect->GetParameterByName("PeeledBounceBuffer")->SetValue(mPeeledLambertDirectBuffer->GetShaderResourceView());
	mRadiosityEffect->GetParameterByName("NormalBuffer")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Normal));
	mRadiosityEffect->GetParameterByName("PeeledNormalBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::Normal));
}

void DeepGBufferRadiosity::OnWindowResize(uint32_t width, uint32_t height)
{
	CreateBuffers(width, height);
}

void DeepGBufferRadiosity::RenderScene()
{
	RenderGBuffers();
	RenderLambertianOnly();
	RenderIndirectIllumination();

	if (mSettings.Enabled && mSettings.PropagationDamping < 1.0f)
		mGBuffer.GetTexture(GBuffer::DepthStencil)->CopyToTexture(*mPreviousDepthBuffer);

	mDevice->BindFrameBuffer(mDevice->GetScreenFrameBuffer());

	//mBlitEffect->GetParameterByName("SourceMap")->SetValue(mGBuffer.GetTextureSRV(GBuffer::Lambertain));
	mBlitEffect->GetParameterByName("SourceMap")->SetValue(mRawIIBuffer->GetShaderResourceView());
	mDevice->DrawFSTriangle(mBlitEffect->GetTechniqueByName("BlitColor"));


	mPrevViewMatrix = mCamera->GetViewMatrix();
}

void DeepGBufferRadiosity::RenderGBuffers()
{
	// Todo: update render queue with render bucket filter
	mSceneMan->UpdateRenderQueue(mCamera, RO_None, RenderQueue::BucketAll, 0);   
	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	

	const float4x4& view = mCamera->GetViewMatrix();
	const float4x4& proj = mCamera->GetProjMatrix();
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
		effect->GetParameterByName("ClipInfo")->SetValue(float2(proj.M33, proj.M43));
		effect->GetParameterByName("MinZSeparation")->SetValue(mSettings.DepthPeelSeparationHint);
		effect->GetParameterByName("PrevDepthBuffer")->SetValue( mGBuffer.GetTextureSRV(GBuffer::DepthStencil) );

		renderItem.Renderable->Render();
	}


	//RenderFactory* factory = mDevice->GetRenderFactory();
	//factory->SaveTextureToFile("E:/DeepGBuffer_Lambertain.tga", mGBuffer.GetTexture(GBuffer::Lambertain));
	//factory->SaveTextureToFile("E:/DeepGBuffer_Glossy.tga", mGBuffer.GetTexture(GBuffer::Glossy));
	//factory->SaveTextureToFile("E:DeepGBuffer_Lambertain.pfm", mGBuffer.GetTexture(GBuffer::Lambertain));
}

void DeepGBufferRadiosity::RenderLambertianOnly()
{
	mSceneMan->UpdateLightQueue(*mCamera);
	const LightQueue& sceneLights = mSceneMan->GetLightQueue();

	Light* mainDirLight = sceneLights.front();
	assert(mainDirLight->GetLightType() == LT_DirectionalLight);

	float3 lightColor = mainDirLight->GetLightColor() * mainDirLight->GetLightIntensity();
	const float3& lightDirection = mainDirLight->GetDerivedDirection();

	float4x4 InvViewProj = mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
	InvViewProj = MatrixInverse(InvViewProj);
	
	mLambertianOnlyEffect->GetParameterByName("InvViewProj")->SetValue(InvViewProj);
	mLambertianOnlyEffect->GetParameterByName("CameraPosition")->SetValue(mCamera->GetPosition());
	mLambertianOnlyEffect->GetParameterByName("LightDirection")->SetValue(lightDirection);
	mLambertianOnlyEffect->GetParameterByName("LightColor")->SetValue(lightColor);
	mLambertianOnlyEffect->GetParameterByName("PropagationDamping")->SetValue(mSettings.PropagationDamping);
	mLambertianOnlyEffect->GetParameterByName("InvViewport")->SetValue(float2(1.0f / mBufferWidth, 1.0f / mBufferHeight));
	mLambertianOnlyEffect->GetParameterByName("LightBoost")->SetValue(float2(mSettings.UnsaturatedBoost, mSettings.SaturatedBoost));

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
		//TemporalFiltering();
	}
}

void DeepGBufferRadiosity::ComputeMipmapedBuffers()
{
	EffectTechnique* reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(0);
	
	mFrameBuffer->AttachRTV(ATT_Color0, mCSZRTV);
	if (mSettings.UseDepthPeelBuffer)
	{
		if (!mSettings.UseOct16)
		{
			reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(1);
		}
		else
		{
			// lazy created if needed
			if (!mPackedNormalBuffer || mPackedNormalBuffer->GetWidth() != mBufferWidth || mPackedNormalBuffer->GetHeight() != mBufferHeight)
			{
				RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

				const uint32_t texCreateFlags = TexCreate_RenderTarget | TexCreate_ShaderResource;
				mPackedNormalBuffer = factory->CreateTexture2D(mBufferWidth, mBufferHeight, GetNormalFormat(mSettings.UseOct16), 1, 1, 1, 0,  BufferAccessHint, texCreateFlags, NULL);
				mPackedNormalRTV = factory->CreateRenderTargetView2D(mPackedNormalBuffer, 0, 0);
				
				// Bind shader parameter for SRV
				mReconstrctCSZEffect->GetParameterByName("PeeledNormalBuffer")->SetValue(mPeeledGBuffer.GetTextureSRV(GBuffer::Normal));
			}
				
			reconstrctTech = mReconstrctCSZEffect->GetTechniqueByIndex(2);
			mFrameBuffer->AttachRTV(ATT_Color2, mPackedNormalRTV);
		}
	}

	const float4x4& proj = mCamera->GetProjMatrix();
	mReconstrctCSZEffect->GetParameterByName("ClipInfo")->SetValue(float2(proj.M33, proj.M43));

	mDevice->BindFrameBuffer(mFrameBuffer);
	mDevice->DrawFSTriangle(reconstrctTech);
}

void DeepGBufferRadiosity::ComputeRawII()
{
	const float4x4& proj = mCamera->GetProjMatrix();
	float4 projInfo(2.0f/(mBufferWidth*proj.M11), -2.0f/(mBufferHeight*proj.M22), -1.0f/proj.M11, 1.0f/proj.M22);
	float projScale = 0.5f * proj.M22 * mBufferHeight;

	mSettings.Radius = 74.0f;

	mRadiosityEffect->GetParameterByName("ProjInfo")->SetValue(projInfo);
	mRadiosityEffect->GetParameterByName("ProjScale")->SetValue(projScale);
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

}

const shared_ptr<Texture> DeepGBufferRadiosity::GetRadiosityTexture() const
{
	if (mSettings.BlurRadius != 0) 
	{
		return mResultBuffer;
	} 
	else
	{ // No blur passes, so pull out the raw buffer!
		return mTempFiltedResultBuffer;
	}
}






}