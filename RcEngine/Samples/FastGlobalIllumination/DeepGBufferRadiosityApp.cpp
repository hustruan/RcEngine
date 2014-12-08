#include <MainApp/Application.h>
#include <MainApp/Window.h>
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
#include <Graphics/CameraController1.h>
#include <Graphics/AmbientOcclusion.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <GUI/Label.h>
#include <GUI/Slider.h>
#include <GUI/UIWindow.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <GUI/CheckBox.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Math/MathUtil.h>
#include "DeepGBufferRadiosity.h"

using namespace RcEngine;

class FastGIApp : public Application
{
public:
	FastGIApp(const String& config)
		: Application(config),
		   mFramePerSecond(0)
	{

	}
	~FastGIApp(void)
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

		mRenderPath = std::make_shared<DeepGBufferRadiosity>();
		mRenderPath->OnGraphicsInit(mMainCamera);

		mMainCamera->CreateLookAt(float3(-18.415079, 5.102501, 0.825465), float3(-17.415262, 5.120544, 0.831733));
		mMainCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 0.5f, 100.0f );

		mCameraControler = std::make_shared<Test::FPSCameraControler>(); 
		mCameraControler->AttachCamera(*mMainCamera);
		mCameraControler->SetMoveSpeed(10.0f);
		mCameraControler->SetMoveInertia(true);
	}

	void LoadContent()
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

		const float SponzaScale = 0.02f;
		sponzaNode->SetScale(float3(SponzaScale, SponzaScale, SponzaScale));
		sponzaNode->AttachObject(sponzaEnt);

		Entity* lucyEnt = sceneMan->CreateEntity("Sponza", "./Lucy/Lucy.mesh", "Custom");
		SceneNode* lucyNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Lucy");

		const float lucyScale = 0.005f;
		lucyNode->SetScale(float3(lucyScale, lucyScale, lucyScale));
		lucyNode->SetPosition(float3(0, 2.95f, -3));
		lucyNode->SetRotation( QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::ToRadian(90.0f)) );
		lucyNode->AttachObject(lucyEnt);


		//auto aabb = sceneMan->GetRootSceneNode()->GetWorldBoundingBox();
		//auto extent = aabb.Max - aabb.Min;
	}

	void UnloadContent() {}
	void Update(float deltaTime)
	{
		mCameraControler->Update(deltaTime);

		//auto pos = mMainCamera->GetPosition();
 		//printf("Pos: %f, %f, %f\n", pos.X(), pos.Y(), pos.Z());
	}

	void Render()
	{
		mRenderPath->RenderScene();

		mDevice->GetScreenFrameBuffer()->SwapBuffers();
	}

	void CalculateFrameRate()
	{

	}
	
	void WindowResize(uint32_t width, uint32_t height)
	{

	}

private:
	int mFramePerSecond;
	RenderDevice* mDevice;
	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Test::FPSCameraControler> mCameraControler;
};


int main()
{
	FastGIApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}