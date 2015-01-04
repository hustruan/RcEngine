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
#include <Graphics/TextureResource.h>
#include <Graphics/Sky.h>
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

		mMainCamera->CreateLookAt(float3(-24.278074, 3.664948, -1.303544), float3(-23.288984, 3.664948, -1.303544));
		mMainCamera->CreatePerspectiveFov(Mathf::ToRadian(77.49f), (float)mAppSettings.Width / (float)mAppSettings.Height, 0.1f, 150.0f );

		mCameraControler = std::make_shared<Test::FPSCameraControler>(); 
		mCameraControler->AttachCamera(*mMainCamera);
		mCameraControler->SetMoveSpeed(7.0f);
		mCameraControler->SetMoveInertia(true);

		// Render Path
		mRenderPath = std::make_shared<DeepGBufferRadiosity>();
		mRenderPath->OnGraphicsInit(mMainCamera);
	}

	void LoadContent()
	{
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		//Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		////dirLight->SetDirection(float3(0, -1.5, -0.5));
		//dirLight->SetDirection(float3(0, -0.5, -1));
		//dirLight->SetLightColor(float3(1.0, 1.0, 1.0));
		//dirLight->SetLightIntensity(5.0);
		//dirLight->SetCastShadow(false);
		//dirLight->SetShadowCascades(4);
		//sceneMan->GetRootSceneNode()->AttachObject(dirLight);

		Light* dirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		dirLight->SetDirection(float3(0, -2.0, -0.58));
		//dirLight->SetDirection(float3(0, -0.5, -1));
		dirLight->SetLightColor(float3(1.0, 1.0, 1.0));
		dirLight->SetLightIntensity(10.0);
		dirLight->SetCastShadow(true);
		dirLight->SetShadowCascades(3);
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
		lucyNode->SetPosition(float3(-14.0f, 2.95f, -3.0));
		lucyNode->SetRotation( QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::ToRadian(90.0f)) );
		lucyNode->AttachObject(lucyEnt);

		// Environment lighting 
		shared_ptr<TextureResource> envLightMapResource = resMan.GetResourceByName<TextureResource>(RT_Texture, "/CubeMap/cube.dds", "General");
		(static_pointer_cast_checked<DeepGBufferRadiosity>(mRenderPath))->SetEnvironmentLightingProbe(envLightMapResource->GetTexture());

		// SkyBox
		SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/noonclouds.dds", "General");
		//SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/cube.dds", "General");
		sceneMan->GetSkySceneNode()->AttachObject(skybox);

		//auto aabb = sceneMan->GetRootSceneNode()->GetWorldBoundingBox();
		//auto extent = aabb.Max - aabb.Min;
	}

	void UnloadContent() {}
	void Update(float deltaTime)
	{
		mCameraControler->Update(deltaTime);

		auto target = mMainCamera->GetLookAt();
		auto eye = mMainCamera->GetPosition();
		auto up = mMainCamera->GetUp();

		if ( InputSystem::GetSingleton().KeyPress(KC_Q) )
		{
			FILE* f = fopen("E:/camera.txt", "w");
			fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
				eye[0], eye[1], eye[2], 
				target[0], target[1], target[2],
				up[0], up[1], up[2]);
			fclose(f);
		}

		//char title[255];
		//sprintf_s(title, 255, "Camera: %.1f, %.1f, %.1f", eye[0], eye[1], eye[2]);
		//mMainWindow->SetTitle(title);
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