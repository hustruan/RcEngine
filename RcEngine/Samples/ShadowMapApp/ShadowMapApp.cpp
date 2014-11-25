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
#include <Graphics/AmbientOcclusion.h>

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
	mRenderPath = std::make_shared<DeferredPath>();
	mRenderPath->OnGraphicsInit(mMainCamera);

	// FPS camera controller
	//mMainCamera->CreateLookAt(float3(-129.696564, 97.300003, 0.699008), float3(-128.703964, 97.207588, 0.777768), float3(0.092125, 0.995721, 0.007309));
	mMainCamera->CreateLookAt(float3(-18.415079, 5.102501, 0.825465), float3(-17.415262, 5.120544, 0.831733));
	
	mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 0.5f, 3000.0f );

	mCameraControler = std::make_shared<Test::FPSCameraControler>(); 
	mCameraControler->AttachCamera(*mMainCamera);
	mCameraControler->SetMoveSpeed(30.0f);
	mCameraControler->SetMoveInertia(true);
}

void ShadowMapApp::LoadContent()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
	dirLight->SetDirection(float3(0, -0.5, -1));
	dirLight->SetLightColor(float3(1.0, 1.0, 1.0));
	dirLight->SetLightIntensity(1.0);
	dirLight->SetCastShadow(false);
	dirLight->SetShadowCascades(4);
	sceneMan->GetRootSceneNode()->AttachObject(dirLight);

	// Load Sponza
	Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
	SceneNode* sponzaNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sponza");

	sponzaNode->SetScale(float3(0.05, 0.05, 0.05));
	//sponzaNode->SetWorldPosition(float3(-30, 325.0, 20));
	sponzaNode->AttachObject(sponzaEnt);
}

void ShadowMapApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mCameraControler->Update(deltaTime);

	if ( InputSystem::GetSingleton().KeyPress(KC_Q) )
	{
		auto target = mMainCamera->GetLookAt();
		auto eye = mMainCamera->GetPosition();
		auto up = mMainCamera->GetUp();

		FILE* f = fopen("E:/camera.txt", "w");
		fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
			eye[0], eye[1], eye[2], 
			target[0], target[1], target[2],
			up[0], up[1], up[2]);
		fclose(f);
	}


	String title = " FPS: " + std::to_string(mFramePerSecond);
	mMainWindow->SetTitle(title);
}

void ShadowMapApp::Render()
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
