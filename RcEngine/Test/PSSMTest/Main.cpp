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
		//mCamera->CreateLookAt(float3(165.303284, 195.397446, -139.612320), float3(164.771561, 194.696701, -139.136673), float3(-0.522273, 0.713417, 0.467192));
		mCamera->CreateLookAt(float3(301.375397, 230.045181, 162.783783), float3(300.737335, 229.423859, 162.329010), float3(-0.505969, 0.783551, -0.360616));
		//mCamera->CreateLookAt(float3(-18.394897, 196.775467, 244.656967), float3(-19.149876, 196.313293, 244.191772), float3(-0.393474, 0.886790, -0.242449));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		auto view = mCamera->GetViewMatrix();
		auto proj = mCamera->GetProjMatrix();

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(300.0f);
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
		Entity* arena = sceneMan->CreateEntity("Arena", "./ShuangMaCheng/Castle.mesh",  "Custom");
		SceneNode* castleNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Castle");
		castleNode->SetScale(float3(3, 3, 3));
		castleNode->AttachObject(arena);

		SkyBox* skybox = sceneMan->CreateSkyBox("noonclouds", "/CubeMap/noonclouds.dds", "General");


		//const float CHAR_SCALE = 5.0f;
		//const float CHAR_HEIGHT = 5.0f * CHAR_SCALE;
		//Entity* mSinbady = sceneMan->CreateEntity("Sinbad", "./Sinbad/Sinbad.mesh",  "Custom");
		//SceneNode* mSinbadNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sinbad");
		//mSinbadNode->SetScale(float3(CHAR_SCALE, CHAR_SCALE, CHAR_SCALE));
		//mSinbadNode->Translate(float3(0, CHAR_HEIGHT, 0));
		//mSinbadNode->AttachObject(mSinbady);
	
		//// Sword BoneSceneNode
		//SceneNode*mSheathLNode = mSinbady->CreateBoneSceneNode("SheathL", "Sheath.L");
		//SceneNode*mSheathRNode = mSinbady->CreateBoneSceneNode("SheathR", "Sheath.R");
		//SceneNode*mHandleLNode = mSinbady->CreateBoneSceneNode("HandL", "Handle.L");
		//SceneNode*mHandleRNode = mSinbady->CreateBoneSceneNode("HandR", "Handle.R");

		//Entity* mSwordL = sceneMan->CreateEntity("Swoard", "./Sinbad/Sword.mesh",  "Custom");
		//Entity* mSwordR = sceneMan->CreateEntity("Swoard", "./Sinbad/Sword.mesh",  "Custom");

		//mSheathLNode->AttachObject(mSwordL);
		//mSheathRNode->AttachObject(mSwordR);

		
		// Set as default camera
		shared_ptr<FrameBuffer> screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
		screenFB->SetCamera(mCamera);

		Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		mDirLight->SetDirection(float3(0, -1, 1));
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

		if (mTimer.GetGameTime()-baseTime >= 1.0f)
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

		if ( InputSystem::GetSingleton().KeyPress(KC_Q) )
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