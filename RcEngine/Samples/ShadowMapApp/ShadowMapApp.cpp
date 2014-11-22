#include "ShadowMapApp.h"
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

ShadowMapApp::ShadowMapApp( const String& config )
	: Application(config),
	  mFramePerSecond(0)
{

}


ShadowMapApp::~ShadowMapApp(void)
{
}

void ShadowMapApp::Initialize()
{
	mDevice = Environment::GetSingleton().GetRenderDevice();

	// Create camera 
	mMainCamera = std::make_shared<Camera>();

	// Bind default camera
	mDevice->GetScreenFrameBuffer()->SetCamera(mMainCamera);

	// Create render path
	mRenderPath = std::make_shared<ForwardPath>();
	mRenderPath->OnGraphicsInit(mMainCamera);

	// FPS camera controller
	//mMainCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));

	mMainCamera->CreateLookAt(float3(0, 20, 0), float3(1, 20, 0));
	mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

	mCameraControler = std::make_shared<Test::FPSCameraControler>(); 
	mCameraControler->AttachCamera(*mMainCamera);
	mCameraControler->SetMoveSpeed(300.0f);
	mCameraControler->SetMoveInertia(true);
}

void ShadowMapApp::LoadContent()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
	dirLight->SetDirection(float3(0, -1.5, -0.5));
	dirLight->SetLightColor(float3(1.0, 1.0, 1.0));
	dirLight->SetLightIntensity(1.0);
	dirLight->SetCastShadow(true);
	dirLight->SetShadowCascades(4);
	sceneMan->GetRootSceneNode()->AttachObject(dirLight);

	// Load Sponza
	Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
	sceneMan->GetRootSceneNode()->AttachObject(sponzaEnt);
}

void ShadowMapApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mCameraControler->Update(deltaTime);

	String title = " FPS: " + std::to_string(mFramePerSecond);
	mMainWindow->SetTitle(title);
}

void ShadowMapApp::Render()
{
	mRenderPath->RenderScene();
	mDevice->GetScreenFrameBuffer()->SwapBuffers();
}

void ShadowMapApp::CalculateFrameRate()
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

void ShadowMapApp::WindowResize( uint32_t width, uint32_t height )
{
	mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)width/(float)height, 1.0f, 3000.0f );
	mRenderPath->OnWindowResize(width, height);
}

int main(int argc, char* argv[])
{	
	ShadowMapApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}
