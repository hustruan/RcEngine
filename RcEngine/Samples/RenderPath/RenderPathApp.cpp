#include "RenderPathApp.h"
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/CameraController1.h>
#include <Graphics/RenderPath.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Camera.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Font.h>
#include <GUI/Label.h>
#include <GUI/UIWindow.h>
#include <GUI/UIManager.h>
#include <GUI/CheckBox.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Math/MathUtil.h>
#include "LightAnimation.h"

using namespace RcEngine;

class RenderPathApp : public Application
{
public:
	RenderPathApp(const String& config)
		:Application(config), 
		 mCameraControler(0),
		 mFramePerSecond(0),
		 mAnimateLights(true)
	{

	}
	~RenderPathApp(void) {}

protected:

	void Initialize()
	{
		mDevice = Environment::GetSingleton().GetRenderDevice();
		assert(mDevice);

		// Create camera 
		mMainCamera = std::make_shared<Camera>();

		// Bind default camera
		mDevice->GetScreenFrameBuffer()->SetCamera(mMainCamera);

		// Create render path
		mRenderPath = std::make_shared<TiledDeferredPath>();
		mRenderPath->OnGraphicsInit(mMainCamera);

		mMainCamera->CreateLookAt(float3(0, 20, 0), float3(1, 20, 0));
		mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mMainCamera);
		mCameraControler->SetMoveSpeed(300.0f);
		mCameraControler->SetMoveInertia(true);
		mCameraControler->Update(0);

		InitGUI();

		String title = (mAppSettings.RHDeviceType == RD_OpenGL) ? "OpenGL Application" : "Direct3D11 Application";
		mMainWindow->SetTitle(title);	
	}

	void InitGUI()
	{
		UIElement* rootElem = UIManager::GetSingleton().GetRoot();

		mUIWindow = new UIWindow;
		mUIWindow->InitGuiStyle(nullptr);
		mUIWindow->SetName("TBDR");
		mUIWindow->SetTitle(L"TBDR");
		mUIWindow->SetPosition(int2(810, 640));
		mUIWindow->SetSize(int2(200, 120));
		rootElem->AddChild( mUIWindow );

		uint32_t uiY = 30;

		mLightLabel = new Label;
		mLightLabel->InitGuiStyle(nullptr);
		mLightLabel->SetFontSize(20.0f);
		mLightLabel->SetPosition(int2(8, uiY));
		mLightLabel->SetSize(int2(150, 20));
		mLightLabel->SetTextAlignment(AlignLeft | AlignVCenter);
		mUIWindow->AddChild( mLightLabel );   

		uiY += mLightLabel->GetSize().Y() + 10;

		mFPSLabel = new Label;
		mFPSLabel->InitGuiStyle(nullptr);
		mFPSLabel->SetFontSize(20.0f);
		mFPSLabel->SetPosition(int2(8, uiY));
		mFPSLabel->SetSize(int2(150, 20));
		mFPSLabel->SetTextAlignment(AlignLeft | AlignVCenter);
		mUIWindow->AddChild( mFPSLabel );   

		uiY += mLightLabel->GetSize().Y() + 10;

		mAnimLightsCheckBox = new CheckBox();
		mAnimLightsCheckBox->InitGuiStyle(nullptr);
		mAnimLightsCheckBox->SetPosition(int2(8, uiY));
		mAnimLightsCheckBox->SetSize(int2(150, mAnimLightsCheckBox->GetSize().Y()));
		mAnimLightsCheckBox->SetText(L"Animate Lights");
		mAnimLightsCheckBox->SetChecked(mAnimateLights);
		mAnimLightsCheckBox->EventStateChanged.bind(this, &RenderPathApp::AnimateLightsClicked);
		mUIWindow->AddChild( mAnimLightsCheckBox );      
	}

	void AnimateLightsClicked(bool bChecked)
	{
		mAnimateLights = bChecked;	
	}

	void LoadContent()
	{
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		// Load Sponza
		Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
		sceneMan->GetRootSceneNode()->AttachObject(sponzaEnt);

		mLightAnimation.SetupLights();
		mLightLabel->SetText(L"NumLights: " + std::to_wstring(mLightAnimation.GetNumLights()));
	}

	void UnloadContent() {}

	void Update(float deltaTime)
	{
		CalculateFrameRate();
		mCameraControler->Update(deltaTime);

		if (mAnimateLights)
			mLightAnimation.Move(deltaTime);

	/*	wchar_t buffer[128];
		std::swprintf(buffer, L"FPS: %d", mFramePerSecond);
		mFPSLabel->SetText(buffer);*/
	}

	void Render()
	{
		mRenderPath->RenderScene();

		// Update overlays
		UIManager& uiMan = UIManager::GetSingleton();
		uiMan.Render();

		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		sceneMan->UpdateOverlayQueue();

		RenderBucket& guiBucket =sceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay, false);   
		for (const RenderQueueItem& renderItem : guiBucket) 
			renderItem.Renderable->Render();

		mDevice->GetScreenFrameBuffer()->SwapBuffers();
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
		mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)width/(float)height, 1.0f, 3000.0f );
		mRenderPath->OnWindowResize(width, height);
	}

protected:
	RenderDevice* mDevice;

	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;

	RcEngine::Test::FPSCameraControler* mCameraControler;

	int mFramePerSecond;

	bool mAnimateLights;
	LightAnimation mLightAnimation;

	UIWindow* mUIWindow;
	CheckBox* mAnimLightsCheckBox;
	Label* mLightLabel;
	Label* mFPSLabel;
};



int main(int argc, char* argv[])
{	
	RenderPathApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}