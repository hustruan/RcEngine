#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Scene/Light.h>
#include <Math/MathUtil.h>
#include <Graphics/Image.h>

using namespace RcEngine;

#define MAX_MIP_LEVEL 5

class MeshTestApp : public Application
{
public:
	MeshTestApp(const String& config)
		: Application(config),
		  mFramePerSecond(0)
	{
		
	}

	virtual ~MeshTestApp(void)
	{

	}

protected:
	void Initialize()
	{
		ResourceManager& resMan = ResourceManager::GetSingleton();
		FileSystem& fileSys = FileSystem::GetSingleton();

		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		const int width = mMainWindow->GetWidth();
		const int height = mMainWindow->GetHeight();

		mFrameBuffer = factory->CreateFrameBuffer(width, height);
		mRTBuffer = factory->CreateTexture2D(width, height, PF_RGBA32F, 1, MAX_MIP_LEVEL, 1, 0,
			EAH_GPU_Read | EAH_GPU_Write, TexCreate_ShaderResource | TexCreate_RenderTarget , NULL);

		for (int i = 0; i < MAX_MIP_LEVEL; ++i)
		{
			mRenderViews.push_back(factory->CreateRenderTargetView2D(mRTBuffer, 0, i));
			mRTSRVs.push_back( factory->CreateTexture2DSRV(mRTBuffer, 0, std::max(1, i), 0, 1));
		}

		mMipLevel = 0;
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		mBlitEffect = resMan.GetResourceByName<Effect>(RT_Effect, "MipmapGen.effect.xml", "General");	
		mTexture = resMan.GetResourceByName<TextureResource>(RT_Texture, "StrechXY3.dds", "General")->GetTexture();
	}


	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		CalculateFrameRate();

		if (InputSystem::GetSingleton().KeyPress(KC_0))
			mMipLevel = 0;
		else if (InputSystem::GetSingleton().KeyPress(KC_1))
			mMipLevel = 1;
		else if (InputSystem::GetSingleton().KeyPress(KC_2))
			mMipLevel = 2;
		else if (InputSystem::GetSingleton().KeyPress(KC_3))
			mMipLevel = 3;
		else if (InputSystem::GetSingleton().KeyPress(KC_4))
			mMipLevel = 4;
		else if (InputSystem::GetSingleton().KeyPress(KC_5))
			mMipLevel = 5;
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		mFrameBuffer->AttachRTV(ATT_Color0, mRenderViews.front());
		device->BindFrameBuffer(mFrameBuffer);

		mBlitEffect->GetParameterByName("MipLevel")->SetValue(0);
		mBlitEffect->GetParameterByName("SourceMap")->SetValue(mTexture->GetShaderResourceView());
		device->DrawFSTriangle(mBlitEffect->GetTechniqueByName("BlitColor"));
		//mRTBuffer->BuildMipMap();

		////////////////////////////////////////////////////
#if 1
		uint32_t width = mMainWindow->GetWidth();
		uint32_t height = mMainWindow->GetHeight();

		mBlitEffect->GetParameterByName("SourceMap")->SetValue(mRTBuffer->GetShaderResourceView());

		for (int i = 1; i < MAX_MIP_LEVEL; ++i)
		{
			uint32_t levelWidth = std::max(1U,width >> 1);
			uint32_t levelHeight = std::max(1U, height >> 1);

			mFrameBuffer->Resize(levelWidth, levelHeight);
			mFrameBuffer->AttachRTV(ATT_Color0, mRenderViews[i]);
			device->BindFrameBuffer(mFrameBuffer);

			mBlitEffect->GetParameterByName("SourceMap")->SetValue(mRTSRVs[i]);
			mBlitEffect->GetParameterByName("PreviousMIP")->SetValue(int3(i-1, width, height));
			device->DrawFSTriangle(mBlitEffect->GetTechniqueByName("Minify"));

			width = levelWidth;
			height = levelHeight;
		}
#endif

		////////////////////////////////////////////////////
		auto screenFB = device->GetScreenFrameBuffer();
		device->BindFrameBuffer(screenFB);

		mBlitEffect->GetParameterByName("MipLevel")->SetValue(mMipLevel);
		mBlitEffect->GetParameterByName("SourceMap")->SetValue(mRTBuffer->GetShaderResourceView());
		device->DrawFSTriangle(mBlitEffect->GetTechniqueByName("BlitColor"));

		screenFB->SwapBuffers();
	}

	void CalculateFrameRate()
	{
		static int frameCount = 0;
		static float baseTime = 0;

		frameCount++;

		if (mTimer.GetGameTime()-baseTime >= 1.0f)
		{
			mFramePerSecond = frameCount;
			frameCount = 0;
			baseTime += 1.0f;
		}
	}

	void WindowResize(uint32_t width, uint32_t height)
	{
	
	}

protected:
	int mFramePerSecond;
	shared_ptr<FrameBuffer> mFrameBuffer;
	shared_ptr<Texture> mRTBuffer;
	vector<shared_ptr<RenderView> > mRenderViews;
	vector<shared_ptr<ShaderResourceView> > mRTSRVs; 

	int mMipLevel;
	shared_ptr<Texture> mTexture;
	shared_ptr<Effect> mBlitEffect;
};


int main()
{

	MeshTestApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}