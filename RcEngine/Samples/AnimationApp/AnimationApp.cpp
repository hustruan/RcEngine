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
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
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
#include <random>
#include <windows.h>

using namespace RcEngine;

//std::mt19937 rng(1337);
//std::uniform_real<float> AngleDist(0.0f, Mathf::TWO_PI);
//
//class TesselationApp : public Application
//{
//public:
//	TesselationApp(const String& config)
//		: Application(config), mFramePerSecond(60)
//	{
//
//	}
//
//	virtual ~TesselationApp(void)
//	{
//
//	}
//
//protected:
//	void Initialize()
//	{
//		mCamera = std::make_shared<Camera>();
//		mCamera->CreateLookAt(float3(0, 20, 0), float3(1, 20, 0));
//		//mCamera->CreateLookAt(float3(0, 150, -250), float3(0, 150, 0));
//		//mCamera->CreateLookAt(float3(-395.7, 839.9, 2061.9), float3(-395.4, 839.6, 2061.0));
//		mCamera->CreatePerspectiveFov(Mathf::PI / 4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 8000.0f);
//
//		auto view = mCamera->GetViewMatrix();
//		auto proj = mCamera->GetProjMatrix();
//
//		mCameraControler = new RcEngine::Test::FPSCameraControler;
//		mCameraControler->AttachCamera(*mCamera);
//		mCameraControler->SetMoveSpeed(100.0f);
//		mCameraControler->SetMoveInertia(true);
//
//		mRenderPath = std::make_shared<ForwardPath>();
//		mRenderPath->OnGraphicsInit(mCamera);
//
//		DebugDrawManager::Initialize();
//		DebugDrawManager::GetSingleton().OnGraphicsInitialize();
//	}
//
//	void LoadContent()
//	{
//		FileSystem& fileSys = FileSystem::GetSingleton();
//		ResourceManager& resMan = ResourceManager::GetSingleton();
//		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
//		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
//
//		LoadDudeEntity();
//
//		mFont = resMan.GetResourceByName<Font>(RT_Font, "Consolas Regular", "General");
//		auto mSDFEffect = resMan.GetResourceByName<Effect>(RT_Effect, "Font.effect.xml", "General");
//		mSpriteBatch = sceneMan->CreateSpriteBatch(mSDFEffect);
//
//		// Set as default camera
//		auto screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
//		screenFB->SetCamera(mCamera);
//
//		Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
//		mDirLight->SetDirection(float3(1, -0.5, 0));
//		mDirLight->SetLightColor(float3(1, 1, 1));
//		mDirLight->SetLightIntensity(1.0);
//		mDirLight->SetCastShadow(false);
//		mDirLight->SetShadowCascades(4);
//		sceneMan->GetRootSceneNode()->AttachObject(mDirLight);
//	}
//
//	void LoadDudeEntity()
//	{
//		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
//		ResourceManager& resMan = ResourceManager::GetSingleton();
//
//		String modelName = "./DivineWarrior/divine_warrior.mesh";
//		SceneNode* blitzcrankSceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("blitzcrank");
//		{
//			Entity* blitzcrankEntity = sceneMan->CreateEntity("blitzcrank", modelName, "Custom");
//			blitzcrankSceneNode->AttachObject(blitzcrankEntity);
//
//			AnimationPlayer* animPlayer = blitzcrankEntity->GetAnimationPlayer();
//			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/dance.anim", "Custom"));
//
//
//			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/Take 001.anim", "Custom"));
//
//			AnimationState* takeClip = animPlayer->GetClip("Take 001");
//			takeClip->WrapMode = AnimationState::Wrap_Loop;
//			takeClip->Play();
//
//			blitzcrankSceneNode->SetScale(float3(0.3, 0.3, 0.3));
//			//blitzcrankSceneNode->SetPosition(float3(0, 50, 0));
//
//			mDudeEntity = blitzcrankEntity;
//		}
//
//		//Entity* sponzaEnt = sceneMan->CreateEntity("Sponza", "./Sponza/Sponza.mesh", "Custom");
//		//sceneMan->GetRootSceneNode()->AttachObject(sponzaEnt);
//	}
//
//	void UnloadContent()
//	{
//
//	}
//
//	void CalculateFrameRate()
//	{
//		static int frameCount = 0;
//		static float baseTime = 0;
//
//		frameCount++;
//
//		if (mTimer.GetGameTime() - baseTime >= 1.0f)
//		{
//			mFramePerSecond = frameCount;
//			frameCount = 0;
//			baseTime += 1.0f;
//		}
//	}
//
//	void Update(float deltaTime)
//	{
//		CalculateFrameRate();
//		mCameraControler->Update(deltaTime);
//
//		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
//
//		auto center = mCamera->GetPosition() + mCamera->GetForward() * 200.0f;
//
//		wchar_t buffer[255];
//		std::swprintf(buffer, L"Camera (%.1f, %.1f, %.1f), FPS: %d", center.X(), center.Y(), center.Z(), mFramePerSecond);
//
//		mSpriteBatch->Begin();
//		mFont->DrawString(*mSpriteBatch, buffer, 25, float2(20, 750), ColorRGBA::White);
//		mSpriteBatch->End();
//		mSpriteBatch->Flush();
//
//		if (InputSystem::GetSingleton().KeyPress(KC_Q))
//		{
//			auto target = mCamera->GetLookAt();
//			auto eye = mCamera->GetPosition();
//			auto up = mCamera->GetUp();
//
//			FILE* f = fopen("E:/camera.txt", "w");
//			fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
//				eye[0], eye[1], eye[2],
//				target[0], target[1], target[2],
//				up[0], up[1], up[2]);
//			fclose(f);
//		}
//	}
//
//	void Render()
//	{
//		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
//		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
//
//		shared_ptr<FrameBuffer> screenFB = device->GetScreenFrameBuffer();
//		device->BindFrameBuffer(screenFB);
//		screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1.0, 0);
//
//		mRenderPath->RenderScene();
//
//		screenFB->SwapBuffers();
//	}
//
//	void WindowResize(uint32_t width, uint32_t height)
//	{
//		mRenderPath->OnWindowResize(width, height);
//	}
//
//protected:
//
//	shared_ptr<RenderPath> mRenderPath;
//	shared_ptr<Camera> mCamera;
//	Test::FPSCameraControler* mCameraControler;
//
//	Entity* mDudeEntity;
//	SceneNode* mDudeSceneNode;
//
//	int mFramePerSecond;
//
//	shared_ptr<Font> mFont;
//	SpriteBatch* mSpriteBatch;
//};
//
//
//int main()
//{
//	TesselationApp app("../Config.xml");
//	app.Create();
//	app.RunGame();
//	app.Release();
//	return 0;
//}



class TesselationApp : public Application
{
public:
	TesselationApp(const String& config)
		: Application(config), mFramePerSecond(60)
	{

	}

	virtual ~TesselationApp(void)
	{

	}

protected:
	void Initialize()
	{

	}

	void LoadContent()
	{

	}

	void LoadDudeEntity()
	{

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
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		shared_ptr<FrameBuffer> screenFB = device->GetScreenFrameBuffer();
		device->BindFrameBuffer(screenFB);
		screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::ColorRGBA(1, 1, 0, 1), 1.0, 0);


		screenFB->SwapBuffers();
	}

	void WindowResize(uint32_t width, uint32_t height)
	{
		
	}

protected:

	int mFramePerSecond;
};


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	TesselationApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}