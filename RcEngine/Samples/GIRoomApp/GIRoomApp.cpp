#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Graphics/Sky.h>
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

using namespace RcEngine;

class PSSMApp : public Application
{
public:
	PSSMApp(const String& config)
		: Application(config), mFramePerSecond(60)
	{

	}

	virtual ~PSSMApp(void)
	{

	}

protected:
	void Initialize()
	{
		mCamera = std::make_shared<Camera>();
		mCamera->CreateLookAt(float3(-842.031494, 1948.397339, 2762.599854) *0.01, float3(-841.640015, 1948.157959, 2761.711426) *0.01, float3(0.096503, 0.970922, -0.219084) *0.01);
		//mCamera->CreateLookAt(float3(-618.376099, 349.327698, 466.727600), float3(-617.696777, 348.934235, 466.108124), float3(0.290724, 0.919344, -0.265116));
		//mCamera->CreateLookAt(float3(-18.394897, 196.775467, 244.656967), float3(-19.149876, 196.313293, 244.191772), float3(-0.393474, 0.886790, -0.242449));
		mCamera->CreatePerspectiveFov(Mathf::PI / 4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 300.0f);

		auto view = mCamera->GetViewMatrix();
		auto proj = mCamera->GetProjMatrix();

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(15.0f);
		mCameraControler->SetMoveInertia(true);

		mRenderPath = std::make_shared<ForwardPath>();
		mRenderPath->OnGraphicsInit(mCamera);

		DebugDrawManager::Initialize();
		DebugDrawManager::GetSingleton().OnGraphicsInitialize();
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		//Entity* arena = sceneMan->CreateEntity("Arena", "./Sinbad/Floor.mesh",  "Custom");
		Entity* arena = sceneMan->CreateEntity("Arena", "./GIRoom/GIRoom.mesh", "Custom");
		SceneNode* castleNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("GIRoom");

		float scale = 0.01;
		castleNode->SetScale(float3(scale, scale, scale));
		castleNode->AttachObject(arena);

		SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/noonclouds.dds", "General");
		sceneMan->GetSkySceneNode()->AttachObject(skybox);

		// Set as default camera
		shared_ptr<FrameBuffer> screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
		screenFB->SetCamera(mCamera);


		Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		mDirLight->SetDirection(float3(0, -1, 1.5));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetLightIntensity(1.0f);
		mDirLight->SetCastShadow(true);
		mDirLight->SetShadowCascades(4);
		sceneMan->GetRootSceneNode()->AttachObject(mDirLight);
	}

	void UnloadContent()
	{

	}

	void CalculateFrameRate()
	{
		static int frameCount = 0;
		static float baseTime = 0;

		frameCount++;

		if (mTimer.GetGameTime() - baseTime >= 1.0f)
		{
			mFramePerSecond = frameCount;
			frameCount = 0;
			baseTime += 1.0f;
		}
	}

	void Update(float deltaTime)
	{
		CalculateFrameRate();
		mCameraControler->Update(deltaTime);

		auto target = mCamera->GetLookAt();
		auto eye = mCamera->GetPosition();
		auto up = mCamera->GetUp();

		if (InputSystem::GetSingleton().KeyPress(KC_Q))
		{
			FILE* f = fopen("E:/camera.txt", "w");
			fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
				eye[0], eye[1], eye[2],
				target[0], target[1], target[2],
				up[0], up[1], up[2]);
			fclose(f);
		}

		char buffer[255];
		//std::sprintf(buffer, "PSSM+PCF FPS: %d", mFramePerSecond);
		std::sprintf(buffer, "float3(%f, %f, %f)", eye[0], eye[1], eye[2]);
		mMainWindow->SetTitle(buffer);
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		shared_ptr<FrameBuffer> screenFB = device->GetScreenFrameBuffer();
		device->BindFrameBuffer(screenFB);
		screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1.0, 0);

		mRenderPath->RenderScene();

		screenFB->SwapBuffers();
	}

	void WindowResize(uint32_t width, uint32_t height)
	{
		mRenderPath->OnWindowResize(width, height);
	}

protected:

	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Camera> mCamera;
	Test::FPSCameraControler* mCameraControler;

	int mFramePerSecond;
};


int main()
{
	PSSMApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}