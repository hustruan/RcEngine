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

#define GIScene 1

class FastGIApp : public Application
{
public:
	FastGIApp(const String& config)
		: Application(config),
		   mFramePerSecond(0),
		   mAnimateLight(false)
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


#if GIScene == 0
		
		mMainCamera->CreateLookAt(float3(-24.278074, 3.664948, -1.303544), float3(-23.288984, 3.664948, -1.303544));
		mMainCamera->CreatePerspectiveFov(Mathf::ToRadian(77.49f), (float)mAppSettings.Width / (float)mAppSettings.Height, 0.1f, 150.0f );

#elif GIScene == 1

		mSceneScale = 0.02f;
		mMainCamera->CreateLookAt(float3(-842.031494, 1948.397339, 2762.599854) *mSceneScale, float3(-841.640015, 1948.157959, 2761.711426) *mSceneScale, float3(0.096503, 0.970922, -0.219084) *mSceneScale);
		mMainCamera->CreatePerspectiveFov(Mathf::PI / 4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 800.0f);

#endif

		mCameraControler = std::make_shared<Test::FPSCameraControler>();
		mCameraControler->AttachCamera(*mMainCamera);
		mCameraControler->SetMoveSpeed(15.0f);
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

#if GIScene == 0

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


		auto path = (static_pointer_cast_checked<DeepGBufferRadiosity>(mRenderPath));
		path->mSettings.Radius = 7.4f;
		path->mSettings.DepthPeelSeparationHint = 1.6f;
		path->mSettings.SaturatedBoost = 2.0f;
		path->mSettings.UnsaturatedBoost = 1.0f;

		auto aabb = sceneMan->GetRootSceneNode()->GetWorldBoundingBox();
		auto extent = (aabb.Max - aabb.Min);
		printf("Extent: %f, %f\n", extent.X(), extent.Y());

#elif GIScene == 1

		mLightRotAngleStart = 160.0f;
		mLightRotAngleEnd = 200.0f;
		mLightRotAngle = 180;
		mLightRotDir = -1.0f;

		float x = sinf(Mathf::ToRadian(mLightRotAngle));
		float z = cosf(Mathf::ToRadian(mLightRotAngle));
		mLightDirection = float3::Zero() - float3(x, 0.28, z);
		//printf("%f, %f, %f\n", mLightDirection.X(), mLightDirection.Y(), mLightDirection.Z());

		mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		//mDirLight->SetDirection(float3(0, -1, 3.5));
		mDirLight->SetDirection(mLightDirection);
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetLightIntensity(8.0);
		mDirLight->SetCastShadow(true);
		mDirLight->SetShadowCascades(3);
		sceneMan->GetRootSceneNode()->AttachObject(mDirLight);

		Entity* arena = sceneMan->CreateEntity("Arena", "./GIRoom/GIRoom.mesh", "Custom");
		SceneNode* castleNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("GIRoom");

		castleNode->SetScale(float3(mSceneScale, mSceneScale, mSceneScale));
		castleNode->AttachObject(arena);

		auto aabb = sceneMan->GetRootSceneNode()->GetWorldBoundingBox();
		auto extent = (aabb.Max - aabb.Min);
		//printf("Extent: %f, %f\n", extent.X(), extent.Y());

		auto path = (static_pointer_cast_checked<DeepGBufferRadiosity>(mRenderPath));
		path->mSettings.Radius = 60.4f;
		path->mSettings.DepthPeelSeparationHint = 1.6f;
		path->mSettings.SaturatedBoost = 1.0f;
		path->mSettings.UnsaturatedBoost = 1.0f;
#endif

		// Environment lighting 
		shared_ptr<TextureResource> envLightMapResource = resMan.GetResourceByName<TextureResource>(RT_Texture, "/CubeMap/cube.dds", "General");
		(static_pointer_cast_checked<DeepGBufferRadiosity>(mRenderPath))->SetEnvironmentLightingProbe(envLightMapResource->GetTexture());

		// SkyBox
		SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/noonclouds.dds", "General");
		//SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/cube.dds", "General");
		sceneMan->GetSkySceneNode()->AttachObject(skybox);
	}

	void UnloadContent() {}
	void Update(float deltaTime)
	{
		CalculateFrameRate();
		mCameraControler->Update(deltaTime);

		if (mAnimateLight)
		{
			mLightRotAngle = mLightRotAngle + mLightRotDir * 0.7f * deltaTime;
			if (mLightRotAngle >= mLightRotAngleEnd)
			{
				mLightRotAngle = mLightRotAngleEnd;
				mLightRotDir *= -1.0f;
			}
			else if (mLightRotAngle <= mLightRotAngleStart)
			{
				mLightRotAngle = mLightRotAngleStart;
				mLightRotDir *= -1.0f;
			}

			float x = sinf(Mathf::ToRadian(mLightRotAngle));
			float z = cosf(Mathf::ToRadian(mLightRotAngle));
			mLightDirection = float3::Zero() - float3(x, 0.28, z);
			mDirLight->SetDirection(mLightDirection);
			//printf("mLightRotAngle=%f\n", mLightRotAngle);
		}

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
		else if (InputSystem::GetSingleton().KeyPress(KC_E) )
		{
			mAnimateLight = !mAnimateLight;
		}
		
		char title[255];
		//sprintf_s(title, 255, "FPS %d, Camera: %.1f, %.1f, %.1f", mFramePerSecond, eye[0], eye[1], eye[2]);
		//mMainWindow->SetTitle(title);
		mMainWindow->SetTitle("GI Room");
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
	
	void WindowResize(uint32_t width, uint32_t height)
	{

	}

private:

	float mLightRotAngle;
	float mLightRotAngleStart;
	float mLightRotAngleEnd;
	float mLightRotDir;
	float3 mLightDirection;

	bool mAnimateLight;

	Light* mDirLight;

	int mFramePerSecond;
	float mSceneScale;
	RenderDevice* mDevice;
	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Test::FPSCameraControler> mCameraControler;
};

#ifdef _WINDOWS
	#include <windows.h>
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#else
	int main()
	{
#endif

	FastGIApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}