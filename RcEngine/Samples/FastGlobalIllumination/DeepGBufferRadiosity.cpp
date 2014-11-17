#include "DeepGBufferRadiosity.h"
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Material.h>
#include <Graphics/Camera.h>
#include <Graphics/Effect.h>
#include <Scene/SceneManager.h>
#include <Scene/Light.h>
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

	mTextures[DepthStencil] = factory->CreateTexture2D(
									width, height, 
									PF_D32F,
									1,  // layers
									1,  // levels
									1, 0, // samples
									EAH_GPU_Read | EAH_GPU_Write,
									TexCreate_DepthStencilTarget | TexCreate_ShaderResource,
									NULL);
	
	mTextures[Lambertain] = factory->CreateTexture2D(
									width, height, 
									PF_RGBA8_UNORM, 
									1,  // layers
									1,  // levels
									1, 0, // samples
									EAH_GPU_Read | EAH_GPU_Write,
									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
									NULL);

	mTextures[Glossy] = factory->CreateTexture2D(
									width, height, 
									PF_RGBA8_UNORM, 
									1,  // layers
									1,  // levels
									1, 0, // samples
									EAH_GPU_Read | EAH_GPU_Write,
									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
									NULL);

	mTextures[Normal] = factory->CreateTexture2D(
									width, height, 
									PF_RGB10A2, //PF_RGB16F
									1,  // layers
									1,  // levels
									1, 0, // samples
									EAH_GPU_Read | EAH_GPU_Write,
									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
									NULL);

	mTextures[ScreenSpaceVelocity] = factory->CreateTexture2D(
									width, height, 
									PF_RG16F,
									1,  // layers
									1,  // levels
									1, 0, // samples
									EAH_GPU_Read | EAH_GPU_Write,
									TexCreate_RenderTarget | TexCreate_GenerateMipmaps | TexCreate_ShaderResource,
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
								PF_RGB16F,
								1,  // layers
								1,  // levels
								1, 0, // samples
								EAH_GPU_Read | EAH_GPU_Write,
								TexCreate_RenderTarget | TexCreate_ShaderResource,
								NULL);

	mPeeledLambertDirectBuffer = factory->CreateTexture2D(
								width, height, 
								PF_RGB16F,
								1,  // layers
								1,  // levels
								1, 0, // samples
								EAH_GPU_Read | EAH_GPU_Write,
								TexCreate_RenderTarget | TexCreate_ShaderResource,
								NULL);

	mLambertianDirectFrameBuffer->AttachRTV(ATT_Color0, factory->CreateRenderTargetView2D(mLambertDirectBuffer, 0, 0));
	mPeeledLambertianDirectFrameBuffer->AttachRTV(ATT_Color0, factory->CreateRenderTargetView2D(mPeeledLambertDirectBuffer, 0, 0));
}


void DeepGBufferRadiosity::GenerateGBuffers()
{
	// Todo: update render queue with render bucket filter
	mSceneMan->UpdateRenderQueue(*mCamera, RO_None);   

	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	
	
	mDevice->BindFrameBuffer(mGBuffer->mFrameBuffer);
	mGBuffer->mFrameBuffer->Clear(CF_Color | CF_Depth | CF_Stencil, ColorRGBA(0, 0, 0, 0), 1.0f, 0);

	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		renderItem.Renderable->GetMaterial()->SetCurrentTechnique("GBuffer");
		renderItem.Renderable->Render();
	}

	mDevice->BindFrameBuffer(mPeeledGBuffer->mFrameBuffer);
	mPeeledGBuffer->mFrameBuffer->Clear(CF_Color | CF_Depth | CF_Stencil, ColorRGBA(0, 0, 0, 0), 1.0f, 0);

	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		shared_ptr<Effect> effect = renderItem.Renderable->GetMaterial()->GetEffect();
		effect->SetCurrentTechnique("PeeledGBuffer");
		effect->GetParameterByName("ClosestDepthBuffer")->SetValue( mGBuffer->GetTextureSRV(GBuffer::DepthStencil) );
		
		renderItem.Renderable->Render();
	}
}

void DeepGBufferRadiosity::RenderLambertianOnly()
{
	mSceneMan->UpdateLightQueue(*mCamera);

	const LightQueue& sceneLights = mSceneMan->GetLightQueue();

	Light* mainDirLight = sceneLights.front();
	assert(mainDirLight->GetLightType() == LT_DirectionalLight);

	float3 lightColor = mainDirLight->GetLightColor() * mainDirLight->GetLightIntensity();
	mLambertianOnlyEffect->GetParameterByUsage(EPU_Light_Color)->SetValue(lightColor);

	const float3& worldDirection = mainDirLight->GetDerivedDirection();
	float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
	mLambertianOnlyEffect->GetParameterByUsage(EPU_Light_Dir)->SetValue(lightDir);

	mLambertianOnlyEffect->GetParameterByName("GBufferLambertain")->SetValue( mGBuffer->GetTextureSRV(GBuffer::Lambertain) );
	mLambertianOnlyEffect->GetParameterByName("GBufferNormal")->SetValue( mGBuffer->GetTextureSRV(GBuffer::Normal) );
	mLambertianOnlyEffect->GetParameterByName("GBufferSSVelocity")->SetValue( mGBuffer->GetTextureSRV(GBuffer::ScreenSpaceVelocity) );
	mLambertianOnlyEffect->GetParameterByName("GBufferDepth")->SetValue( mGBuffer->GetTextureSRV(GBuffer::DepthStencil) );

	mLambertianOnlyEffect->GetParameterByName("PrevDepthBuffer")->SetValue( mPreviousDepthBuffer->GetShaderResourceView() );
	mLambertianOnlyEffect->GetParameterByName("PrevIndirectRadiosityBuffer")->SetValue( mPreviousRadiosityBuffer->GetShaderResourceView() );

	float4x4 InvViewProj = mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
	InvViewProj = MatrixInverse(InvViewProj);
	mLambertianOnlyEffect->GetParameterByName("InvViewProj")->SetValue(InvViewProj);
	mLambertianOnlyEffect->GetParameterByName("CameraPosition")->SetValue(mCamera->GetPosition());

	mLambertianOnlyEffect->GetParameterByName("LightDirection")->SetValue(lightDir);
	mLambertianOnlyEffect->GetParameterByName("LightColor")->SetValue(lightColor);

	mLambertianOnlyEffect->GetParameterByName("PropagationDamping")->SetValue(mSettings.PropagationDamping);

	EffectTechnique* lambertianTech = mLambertianOnlyEffect->GetTechniqueByIndex(0);

	// Closet Layer
	mDevice->BindFrameBuffer(mLambertianDirectFrameBuffer);
	mLambertianDirectFrameBuffer->Clear(CF_Color, ColorRGBA(0, 0, 0, 0), 1.0f, 0);
	
	mDevice->Draw(lambertianTech, mFullscreenTrangle);

	if (mSettings.Enabled && mSettings.UseDepthPeelBuffer)
	{
		// Peeled Layer
		mDevice->BindFrameBuffer(mPeeledLambertianDirectFrameBuffer);
		mPeeledLambertianDirectFrameBuffer->Clear(CF_Color, ColorRGBA(0, 0, 0, 0), 1.0f, 0);

		mDevice->Draw(lambertianTech, mFullscreenTrangle);
	}
}



}