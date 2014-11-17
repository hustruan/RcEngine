#include "CharacterApp.h"
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
#include <Graphics/DebugDrawManager.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Scene/Light.h>
#include <Math/MathUtil.h>
#include <Core/Profiler.h>
#include <Core/XMLDom.h>
#include <fstream>

CharacterApp::CharacterApp( const String& config )
	: Application(config), 
	  mFramePerSecond(60)
{

}


CharacterApp::~CharacterApp(void)
{
}

void CharacterApp::Initialize()
{
	mCamera = std::make_shared<Camera>();
	mCamera->CreateLookAt(float3(3.405924, 22.548273, -31.670168), float3(3.326280, 22.028830, -30.819384), float3(-0.048415, 0.854505, 0.517183));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 0.1f, 3000.0f );

	/*mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);
	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);*/

	mRenderPath = std::make_shared<ForwardPath>();
	mRenderPath->OnGraphicsInit(mCamera);

	DebugDrawManager::Initialize();
	DebugDrawManager::GetSingleton().OnGraphicsInitialize();
}

void CharacterApp::LoadContent()
{
	FileSystem& fileSys = FileSystem::GetSingleton();
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	// Set as default camera
	auto screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
	screenFB->SetCamera(mCamera);

	Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
	mDirLight->SetDirection(float3(0, -1, 0.5));
	mDirLight->SetLightColor(float3(1, 1, 1));
	mDirLight->SetCastShadow(true);
	sceneMan->GetRootSceneNode()->AttachObject(mDirLight);


	SceneNode* citySceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("AncientCity");
	{
		Entity* cityEntity = sceneMan->CreateEntity("dude", "./AncientCity/AncientCity.mesh",  "Custom");	
		citySceneNode->SetScale(float3(10, 10, 10));
		citySceneNode->AttachObject(cityEntity);
	}

	//Entity* arena = sceneMan->CreateEntity("Arena", "./Sinbad/Floor.mesh",  "Custom");
	//sceneMan->GetRootSceneNode()->AttachObject(arena);

	mSinbadController = new SinbadCharacterController(mCamera);
	mThirdPersonCamera = new ThirdPersonCamera(mSinbadController, mCamera);

	mCamera->CreateLookAt(float3(3.405924, 22.548273, -31.670168), float3(3.326280, 22.028830, -30.819384), float3(-0.048415, 0.854505, 0.517183));
}

void CharacterApp::CalculateFrameRate()
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

void CharacterApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mSinbadController->Update(deltaTime);
	mThirdPersonCamera->Update(deltaTime);

	//mCameraControler->Update(deltaTime);


	if ( InputSystem::GetSingleton().KeyPress(KC_P) )
	{
		auto target = mCamera->GetLookAt();
		auto eye = mCamera->GetPosition();
		auto up = mCamera->GetUp();

		FILE* f = fopen("E:/camera.txt", "w");
		fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
			eye[0], eye[1], eye[2], 
			target[0], target[1], target[2],
			up[0], up[1], up[2]);
		fclose(f);
	}

	char buffer[255];
	std::sprintf(buffer, "FPS: %d", mFramePerSecond);
	mMainWindow->SetTitle(buffer);
}

void CharacterApp::Render()
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	shared_ptr<FrameBuffer> screenFB = device->GetScreenFrameBuffer();
	device->BindFrameBuffer(screenFB);
	screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1.0, 0);

	mRenderPath->RenderScene();

	screenFB->SwapBuffers();
}

void CharacterApp::WindowResize( uint32_t width, uint32_t height )
{
	mRenderPath->OnWindowResize(width, height);
}


int main()
{
	CharacterApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}