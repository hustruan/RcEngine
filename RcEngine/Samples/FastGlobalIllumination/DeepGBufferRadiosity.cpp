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

namespace RcEngine {

//void DeepGBufferRadiosity::MipMappedBuffers::CreateBuffers( uint32_t width, uint32_t height )
//{
//	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
//
//	ColorBuffer = factory->CreateTexture2D(
//									width, height, 
//									PF_RGBA8_UNORM, 
//									1, 
//									0, // Mipmaped 
//									1, 0, 
//									EAH_GPU_Read | EAH_GPU_Write,
//									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
//									NULL);
//
//	NormalBuffer = factory->CreateTexture2D(
//									width, height, 
//									PF_RGBA16F,
//									1, 
//									0, // Mipmaped 
//									1, 0, 
//									EAH_GPU_Read | EAH_GPU_Write,
//									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
//									NULL);
//
//	
//	
//	if (HasPeeledLayer)
//	{
//		PeeledColorBuffer = factory->CreateTexture2D(
//									width, height, 
//									PF_RGBA8_UNORM, 
//									1, 
//									0, // Mipmaped 
//									1, 0, 
//									EAH_GPU_Read | EAH_GPU_Write,
//									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
//									NULL);
//
//		PeeledNormalBuffer = factory->CreateTexture2D(
//									width, height, 
//									PF_RGBA16F,
//									1, 
//									0, // Mipmaped 
//									1, 0, 
//									EAH_GPU_Read | EAH_GPU_Write,
//									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
//									NULL);
//	}
//}

#ifdef _DEBUG
	static uint32_t BufferAccessHint = EAH_CPU_Read | EAH_GPU_Read | EAH_GPU_Write;
#else
	static uint32_t BufferAccessHint = EAH_GPU_Read | EAH_GPU_Write;
#endif

//////////////////////////////////////////////////////////////////////////
void DeepGBufferRadiosity::GBuffer::CreateBuffers( uint32_t width, uint32_t height )
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

	mTextures[GBuffer::Glossy] = factory->CreateTexture2D(
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

DeepGBufferRadiosity::DeepGBufferRadiosity(void)
{
}

DeepGBufferRadiosity::~DeepGBufferRadiosity(void)
{
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

	if (!mLambertianDirectFrameBuffer)
	{
		mLambertianDirectFrameBuffer = factory->CreateFrameBuffer(width, height);
		mPeeledLambertianDirectFrameBuffer = factory->CreateFrameBuffer(width, height);
	}
	else
	{
		mLambertianDirectFrameBuffer->DetachAll();
		mLambertianDirectFrameBuffer->Resize(width, height);

		mPeeledLambertianDirectFrameBuffer->DetachAll();
		mPeeledLambertianDirectFrameBuffer->Resize(width, height);
	}

	mLambertDirectBuffer = factory->CreateTexture2D(
								width, height, 
								PF_RGBA16F,
								1,  // layers
								1,  // levels
								1, 0, // samples
								BufferAccessHint,
								TexCreate_RenderTarget | TexCreate_ShaderResource,
								NULL);

	mPeeledLambertDirectBuffer = factory->CreateTexture2D(
								width, height, 
								PF_RGBA16F,
								1,  // layers
								1,  // levels
								1, 0, // samples
								BufferAccessHint,
								TexCreate_RenderTarget | TexCreate_ShaderResource,
								NULL);

	mLambertianDirectFrameBuffer->AttachRTV(ATT_Color0, factory->CreateRenderTargetView2D(mLambertDirectBuffer, 0, 0));
	mPeeledLambertianDirectFrameBuffer->AttachRTV(ATT_Color0, factory->CreateRenderTargetView2D(mPeeledLambertDirectBuffer, 0, 0));

	// Bind shader parameters
	mLambertianOnlyEffect->GetParameterByName("GBufferLambertain")->SetValue( mGBuffer.GetTextureSRV(GBuffer::Lambertain) );
	mLambertianOnlyEffect->GetParameterByName("GBufferNormal")->SetValue( mGBuffer.GetTextureSRV(GBuffer::Normal) );
	mLambertianOnlyEffect->GetParameterByName("GBufferSSVelocity")->SetValue( mGBuffer.GetTextureSRV(GBuffer::ScreenSpaceVelocity) );
	mLambertianOnlyEffect->GetParameterByName("GBufferDepth")->SetValue( mGBuffer.GetTextureSRV(GBuffer::DepthStencil) );
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
	
	CreateBuffers(windowWidth, windowHeight);
}

void DeepGBufferRadiosity::OnWindowResize(uint32_t width, uint32_t height)
{
	CreateBuffers(width, height);
}

void DeepGBufferRadiosity::RenderScene()
{
	GenerateGBuffers();

	mDevice->BindFrameBuffer(mDevice->GetScreenFrameBuffer());

	mBlitEffect->GetParameterByName("SourceMap")->SetValue(mGBuffer.GetTextureSRV(GBuffer::ScreenSpaceVelocity));
	mDevice->DrawFSTriangle(mBlitEffect->GetTechniqueByName("BlitColor"));


	mPrevViewMatrix = mCamera->GetViewMatrix();
}

void DeepGBufferRadiosity::GenerateGBuffers()
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
			mLambertianOnlyEffect->GetParameterByName("PrevIndirectRadiosityBuffer")->SetValue( mDeepGBufferRadiosityBuffer->GetShaderResourceView() );
		}
		
		lambertianTech = mLambertianOnlyEffect->GetTechniqueByIndex(1);
	}

	mDevice->BindFrameBuffer(mLambertianDirectFrameBuffer);
	mLambertianDirectFrameBuffer->Clear(CF_Color, ColorRGBA(0, 0, 0, 0), 1.0f, 0);
	mDevice->DrawFSTriangle(lambertianTech);
}

}