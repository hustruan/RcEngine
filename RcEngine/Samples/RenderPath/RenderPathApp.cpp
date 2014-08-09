#include "RenderPathApp.h"
#include <Graphics/CameraController1.h>
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/CascadedShadowMap.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/RenderPath.h>
#include <GUI/Label.h>
#include <GUI/Slider.h>
#include <GUI/UIWindow.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <GUI/CheckBox.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Math/MathUtil.h>


using namespace RcEngine;

RenderPathApp::RenderPathApp( const String& config ) 
	:Application(config), mCameraControler(0), mFramePerSecond(0)
{

}

RenderPathApp::~RenderPathApp( void )
{

}

void RenderPathApp::Initialize()
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

	// FPS camera controller
	//mMainCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
	
	mMainCamera->CreateLookAt(float3(0, 20, 0), float3(1, 20, 0));
	mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mMainCamera);
	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);
	mCameraControler->Update(0);

	InitGUI();
}

void RenderPathApp::InitGUI()
{
	//UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	//mLabel = new Label();
	//mLabel->InitGuiStyle(nullptr);
	//mLabel->SetName("FPSLabel");
	//mLabel->SetPosition(int2(10, 700));
	//mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	//mLabel->SetSize(int2(100, 100));
	////mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	//rootElem->AddChild( mLabel );	

	//mWindow = new UIWindow;
	//mWindow->InitGuiStyle(nullptr);
	//mWindow->SetName("Panel");
	//mWindow->SetTitle(L"Window Title");
	//mWindow->SetPosition(int2(650, 430));
	//mWindow->SetSize(int2(350, 300));
	//rootElem->AddChild( mWindow );

	//int uiY = 50;

	//mSunShadowCheckBox = new CheckBox();
	//mSunShadowCheckBox->InitGuiStyle(nullptr);
	//mSunShadowCheckBox->SetName("Sun Shadow");
	//mSunShadowCheckBox->SetPosition(int2(20, uiY));
	//mSunShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	//mSunShadowCheckBox->SetText(L"Sun Shadow");
	//mSunShadowCheckBox->SetChecked(true);
	//mSunShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SunCastShadow);
	//mWindow->AddChild( mSunShadowCheckBox );	

	//mSpotShadowCheckBox = new CheckBox();
	//mSpotShadowCheckBox->InitGuiStyle(nullptr);
	//mSpotShadowCheckBox->SetName("Spot Shadow");
	//mSpotShadowCheckBox->SetPosition(int2(20 + mSunShadowCheckBox->GetSize().X() + 20, uiY));
	//mSpotShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	//mSpotShadowCheckBox->SetText(L"Spot Shadow");
	//mSpotShadowCheckBox->SetChecked(true);
	//mSpotShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SpotCastShadow);
	//mWindow->AddChild( mSpotShadowCheckBox );	

	//
	//uiY += mSunShadowCheckBox->GetSize().Y() + 18;

	//mVisualizeCascadesCheckBox = new CheckBox();
	//mVisualizeCascadesCheckBox->InitGuiStyle(nullptr);
	//mVisualizeCascadesCheckBox->SetName("CheckBox");
	//mVisualizeCascadesCheckBox->SetPosition(int2(20, uiY));
	//mVisualizeCascadesCheckBox->SetSize(int2(150, mVisualizeCascadesCheckBox->GetSize().Y()));
	//mVisualizeCascadesCheckBox->SetText(L"Visualize Cascades");
	//mVisualizeCascadesCheckBox->SetChecked(false);
	//mVisualizeCascadesCheckBox->EventStateChanged.bind(this, &RenderPathApp::VisualizeCascades);
	//mWindow->AddChild( mVisualizeCascadesCheckBox );	

	//uiY += mVisualizeCascadesCheckBox->GetSize().Y() + 18;

	//mBlendAreaLabel = new Label();
	//mBlendAreaLabel->InitGuiStyle(nullptr);
	//mBlendAreaLabel->SetName("BlendArea");
	//mBlendAreaLabel->SetPosition(int2(20, uiY));
	//mBlendAreaLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	//mBlendAreaLabel->SetTextAlignment(AlignLeft);
	//mBlendAreaLabel->SetSize(int2(150, 30));
	//mBlendAreaLabel->SetText(L"BlendArea: 0.01");
	//mBlendAreaLabel->SetFontSize(20.0f);
	////mBlendAreaLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	//mWindow->AddChild( mBlendAreaLabel );	

	//mBlendAreaSlider = new Slider(UI_Horizontal);
	//mBlendAreaSlider->InitGuiStyle(nullptr);
	//mBlendAreaSlider->SetName("Slider");	
	//mBlendAreaSlider->SetPosition(int2(20 + mBlendAreaLabel->GetSize().X(), uiY + 5));
	//mBlendAreaSlider->SetTrackLength(120);
	//mBlendAreaSlider->SetValue(50);
	//mBlendAreaSlider->EventValueChanged.bind(this, &RenderPathApp::BlendAreaSliderValueChange);
	//mWindow->AddChild( mBlendAreaSlider );	

	//uiY += mBlendAreaLabel->GetSize().Y();

	//mSaveCameraBtn = new Button();
	//mSaveCameraBtn->InitGuiStyle(nullptr);
	//mSaveCameraBtn->SetName("Button");	
	//mSaveCameraBtn->SetPosition(int2(20, uiY));
	//mSaveCameraBtn->SetSize(int2(120, 30));
	//mSaveCameraBtn->SetText(L"Dump Camera");
	//mSaveCameraBtn->EventButtonClicked.bind(this, &RenderPathApp::DumpCameraBtnClicked);
	//mWindow->AddChild( mSaveCameraBtn );	
}

void RenderPathApp::SetupScene()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	// Load Sponza
	Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
	sceneMan->GetRootSceneNode()->AttachObject(sponzaEnt);
}

void RenderPathApp::SetupLights()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	//Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
	//dirLight->SetDirection(float3(0, -0.5, -1));
	//dirLight->SetLightColor(float3(0.2, 0.2, 0.2));
	//dirLight->SetLightIntensity(1.0);
	//dirLight->SetCastShadow(false);
	//dirLight->SetShadowCascades(4);
	//sceneMan->GetRootSceneNode()->AttachObject(dirLight);

	//Light* mSpotLight = sceneMan->CreateLight("Spot", LT_SpotLight);
	//mSpotLight->SetDirection(float3(0, -1.5, -1));
	//mSpotLight->SetLightColor(float3(1, 1, 1));
	//mSpotLight->SetRange(300.0);
	//mSpotLight->SetPosition(float3(0.0f, 150.0f, 100.0f));
	//mSpotLight->SetAttenuation(1.0f, 0.0f);
	//mSpotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(60));
	//mSpotLight->SetCastShadow(false);
	//mSpotLight->SetSpotlightNearClip(10);
	//sceneMan->GetRootSceneNode()->AttachObject(mSpotLight);

	//{
	//	float3 direction = Normalize(float3(-111.5f, 398.1f, -3.6f) - float3(-111.1, 380.1, -73.4));
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		Light* spotLight = sceneMan->CreateLight("Spot" + std::to_string(i), LT_SpotLight);
	//		spotLight->SetLightColor(float3(1, 1, 0));
	//		spotLight->SetRange(150);
	//		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, -3.6f));
	//		spotLight->SetDirection(direction);
	//		spotLight->SetAttenuation(1.0f, 0.0f);
	//		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
	//		sceneMan->GetRootSceneNode()->AttachObject(spotLight);
	//	}

	//	direction = Normalize(float3(-111.5f, 398.1f, -35.7f) - float3(-111.1, 380.1, 111.3));
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		Light* spotLight = sceneMan->CreateLight("Spot", LT_SpotLight);
	//		spotLight->SetLightColor(float3(0, 1, 1));
	//		spotLight->SetRange(150);
	//		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, 35.7f));
	//		spotLight->SetDirection(direction);
	//		spotLight->SetAttenuation(1.0f, 0.0f);
	//		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
	//		sceneMan->GetRootSceneNode()->AttachObject(spotLight);
	//	}
	//}

	//(static_pointer_cast_checked<DeferredPath>(mRenderPath))->mVisualLightsWireframe = true;

	 mLightAnimation.LoadLights("E:/lights.txt");
}

void RenderPathApp::LoadContent()
{
	SetupScene();
	SetupLights();
}

void RenderPathApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mCameraControler->Update(deltaTime);

	//InputSystem& inputSys = InputSystem::GetSingleton();
	//if (inputSys.KeyPress(KC_Space))
	//{
	//	float4x4 viewProj = mMainCamera->GetViewMatrix();
	//	printf("View Matrix:\n\n");
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M11, viewProj.M12, viewProj.M13, viewProj.M14);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M21, viewProj.M22, viewProj.M23, viewProj.M24);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M31, viewProj.M32, viewProj.M33, viewProj.M34);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M41, viewProj.M42, viewProj.M43, viewProj.M44);

	//	viewProj = mMainCamera->GetEngineProjMatrix();
	//	printf("\nEngineProj Matrix:\n");
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M11, viewProj.M12, viewProj.M13, viewProj.M14);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M21, viewProj.M22, viewProj.M23, viewProj.M24);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M31, viewProj.M32, viewProj.M33, viewProj.M34);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M41, viewProj.M42, viewProj.M43, viewProj.M44);

	//	viewProj = mMainCamera->GetEngineViewProjMatrix();
	//	printf("\nEngineViewProj Matrix:\n");
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M11, viewProj.M12, viewProj.M13, viewProj.M14);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M21, viewProj.M22, viewProj.M23, viewProj.M24);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M31, viewProj.M32, viewProj.M33, viewProj.M34);
	//	printf("%.2f %.2f %.2f %.2f\n", viewProj.M41, viewProj.M42, viewProj.M43, viewProj.M44);

	//	printf("\n\n");
	//}

	char buffer[255];
	std::sprintf(buffer, "FPS: %d", mFramePerSecond);
	mMainWindow->SetTitle(buffer);
}

void RenderPathApp::Render()
{
	mRenderPath->RenderScene();
	mDevice->GetScreenFrameBuffer()->SwapBuffers();
}

void RenderPathApp::CalculateFrameRate()
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

void RenderPathApp::WindowResize( uint32_t width, uint32_t height )
{
	mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)width/(float)height, 1.0f, 3000.0f );

	mRenderPath->OnWindowResize(width, height);
}


int main(int argc, char* argv[])
{	
	RenderPathApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}