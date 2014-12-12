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
		mDevice = Environment::GetSingleton().GetRenderDevice();

		// Create camera 
		mMainCamera = std::make_shared<Camera>();

		// Bind default camera
		mDevice->GetScreenFrameBuffer()->SetCamera(mMainCamera);

		mRenderPath = std::make_shared<ForwardPath>();
		mRenderPath->OnGraphicsInit(mMainCamera);

		//mMainCamera->CreateLookAt(float3(-24.278074, 3.664948, -1.303544), float3(-23.288984, 3.664948, -1.303544));
		mMainCamera->CreateLookAt(float3(-24.278074, 3.664948, -1.303544), float3(-23.288984, 3.648995, -1.449993), float3(0.015781, 0.999873, -0.002337));
		mMainCamera->CreatePerspectiveFov(Mathf::ToRadian(77.49f), (float)mAppSettings.Width / (float)mAppSettings.Height, 0.1f, 80.0f );

		mCameraControler = std::make_shared<Test::FPSCameraControler>(); 
		mCameraControler->AttachCamera(*mMainCamera);
		mCameraControler->SetMoveSpeed(7.0f);
		mCameraControler->SetMoveInertia(true);
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		dirLight->SetDirection(float3(0, -2.5, -0.5));
		//dirLight->SetDirection(float3(0, -0.5, -1));
		dirLight->SetLightColor(float3(1.0, 1.0, 1.0));
		dirLight->SetLightIntensity(5.0);
		dirLight->SetCastShadow(true);
		dirLight->SetShadowCascades(4);
		sceneMan->GetRootSceneNode()->AttachObject(dirLight);

		// Load Sponza
		Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
		SceneNode* sponzaNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sponza");

		const float SponzaScale = 0.02f;
		sponzaNode->SetScale(float3(SponzaScale, SponzaScale, SponzaScale));
		sponzaNode->AttachObject(sponzaEnt);

		Entity* lucyEnt = sceneMan->CreateEntity("Sponza", "./Lucy/Lucy.mesh", "Custom");
		SceneNode* lucyNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Lucy");

		const float lucyScale = 0.005f;
		lucyNode->SetScale(float3(lucyScale, lucyScale, lucyScale));
		lucyNode->SetPosition(float3(-9.0f, 2.95f, -3.5));
		lucyNode->SetRotation( QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::ToRadian(90.0f)) );
		lucyNode->AttachObject(lucyEnt);

		auto aabb = sceneMan->GetRootSceneNode()->GetWorldBoundingBox();
		auto extent = aabb.Max - aabb.Min;
	}


	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		CalculateFrameRate();
		mCameraControler->Update(deltaTime);
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
		mRenderPath->OnWindowResize(width, height);
	}

protected:
	int mFramePerSecond;
	RenderDevice* mDevice;
	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Test::FPSCameraControler> mCameraControler;
};


int main()
{

	MeshTestApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}