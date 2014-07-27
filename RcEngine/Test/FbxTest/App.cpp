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
		mCamera = std::make_shared<Camera>();
		mCamera->CreateLookAt(float3(-7.404108, 39.924358, -153.436646), float3(-7.288536, 39.863396, -152.445221), float3(0.007056, 0.998140, 0.060553));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		auto view = mCamera->GetViewMatrix();
		auto proj = mCamera->GetProjMatrix();

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(100.0f);
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
		
		LoadDudeEntity();

		// Set as default camera
		auto screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
		screenFB->SetCamera(mCamera);

		Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		mDirLight->SetDirection(float3(0, -1, 0.5));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetCastShadow(false);
		sceneMan->GetRootSceneNode()->AttachObject(mDirLight);
	}

	void LoadDudeEntity()
	{
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
		ResourceManager& resMan = ResourceManager::GetSingleton();

		/*SceneNode* sirisSceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Siris");
		{
		Entity* dudeEntity = sceneMan->CreateEntity("dude", "./InfinityBlade/Siris.mesh",  "Custom");

		AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();

		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_big_down.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_big_left.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_big_center.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_big_right.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_small_down.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_small_left.anim", "Custom"));
		animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./InfinityBlade/attack_small_right.anim", "Custom"));

		AnimationState* takeClip = animPlayer->GetClip("attack_big_down");
		takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
		takeClip->Play();

		sirisSceneNode->AttachObject(dudeEntity);

		mDudeEntity = dudeEntity;
		mDudeSceneNode = sirisSceneNode;
		}*/


		//SceneNode* dudeSceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Dude");
		//{
		//	Entity* dudeEntity = sceneMan->CreateEntity("dude", "./Dude/dude_gpu.mesh",  "Custom");

		//	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
		//	animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Dude/Walk.anim", "Custom"));

		//	AnimationState* takeClip = animPlayer->GetClip("Walk");
		//	takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
		//	takeClip->Play();

		//	dudeSceneNode->SetRotation(QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::PI));
		//	dudeSceneNode->SetPosition(float3(100, 0, 0));
		//	dudeSceneNode->AttachObject(dudeEntity);

		//	mDudeEntity = dudeEntity;
		//	mDudeSceneNode = dudeSceneNode;
		//}

		//SceneNode* arthasSceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Arthas");
		//{
		//	Entity* arthasEntity = sceneMan->CreateEntity("dude", "./Arthas/Arthas.mesh",  "Custom");
		//	Entity* swoardEntity = sceneMan->CreateEntity("swoard", "./Arthas/Sword.mesh",  "Custom");
		//	BoneSceneNode* weaponNode = arthasEntity->CreateBoneSceneNode("Weapon", "wepson");
		//	weaponNode->SetPosition(float3(4.2, -7.8, 0));
		//	weaponNode->AttachObject(swoardEntity);

		//	AnimationPlayer* animPlayer = arthasEntity->GetAnimationPlayer();

		//	//animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Arthas/Walk.anim", "Custom"));
		//	animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Arthas/Random.anim", "Custom"));
		//	//animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Arthas/Standby.anim", "Custom"));
		//	//animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Arthas/Casting.anim", "Custom"));
		//	//animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./Arthas/FightingStandby.anim", "Custom"));

		//	AnimationState* takeClip = animPlayer->GetClip("Random");
		//	takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
		//	takeClip->Play();

		//	arthasSceneNode->SetScale(float3(4, 4, 4));
		//	arthasSceneNode->SetPosition(float3(-100, 0, 0));
		//	arthasSceneNode->AttachObject(arthasEntity);
		//}

		SceneNode* citySceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("AncientCity");
		{
			Entity* arthasEntity = sceneMan->CreateEntity("dude", "./AncientCity/AncientCity.mesh",  "Custom");	
			citySceneNode->SetScale(float3(10, 10, 10));
			citySceneNode->AttachObject(arthasEntity);
		}

		String modelName = "./LOL/blitzcrank/blitzcrank.mesh";
		//String modelName = "./LOL/Ahri/ahri.mesh";
		SceneNode* blitzcrankSceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("blitzcrank");
		{
			Entity* blitzcrankEntity = sceneMan->CreateEntity("blitzcrank", modelName,  "Custom");	
			blitzcrankSceneNode->AttachObject(blitzcrankEntity);

			AnimationPlayer* animPlayer = blitzcrankEntity->GetAnimationPlayer();
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/dance.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/run.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/laugh.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/idle1.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/idle2.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/idle3.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/attack1.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/attack2.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/attack3.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/death.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/windup.anim", "Custom"));
			animPlayer->AddClip(resMan.GetResourceByName<AnimationClip>(RT_Animation, "./LOL/blitzcrank/taunt.anim", "Custom"));

			AnimationState* takeClip = animPlayer->GetClip("run");
			takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
			takeClip->Play();

			//blitzcrankSceneNode->SetScale(float3(0.3, 0.3, 0.3));
			//blitzcrankSceneNode->SetPosition(float3(0, 50, 0));

			mDudeEntity = blitzcrankEntity;
		}
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

		//auto bbox = mDudeSceneNode->GetWorldBoundingBox();
		//DebugDrawManager::GetSingleton().DrawBoundingBox(bbox, ColorRGBA::Red);

		shared_ptr<Skeleton> skeleton = mDudeEntity->GetSkeleton();
		//DebugDrawManager::GetSingleton().DrawSkeleton(mDudeEntity->GetWorldTransform(), skeleton, ColorRGBA::Red);

		/*static bool sb = false;
		for (size_t i = 0; i < skeleton->GetNumBones(); ++i)
		{
			Bone* bone = skeleton->GetBone(i);

			float3 center = bone->GetWorldPosition();
			center = Transform(center, mDudeEntity->GetWorldTransform());

			ColorRGBA color = ColorRGBA::Red;
			if (bone->GetName().find("L_") != String::npos)
				color = ColorRGBA::Green;
			else if (bone->GetName().find("R_") != String::npos)
				color = ColorRGBA::Blue;

			DebugDrawManager::GetSingleton().DrawSphere(center, 1.0, color, true);
		}
*/

		//for (size_t i = 0; i < mDudeCollisionSpheres.size(); ++i)
		//{
		//	Bone* bone = skeleton->GetBone(mDudeCollisionSpheres[i].BoneName);

		//	float3 center(mDudeCollisionSpheres[i].Offset, 0, 0);
		//	center = Transform(center, bone->GetWorldTransform());
		//	center = bone->GetWorldPosition();

		//	DebugDrawManager::GetSingleton().DrawSphere(center, mDudeCollisionSpheres[i].Radius * 0.5, ColorRGBA::Red);
		//}


		/*Bone* bone = skeleton->GetBone("L_forearm");
		DebugDrawManager::GetSingleton().DrawSphere(bone->GetWorldPosition(), 0.5, ColorRGBA::Red);

		bone = skeleton->GetBone("R_forearm");
		DebugDrawManager::GetSingleton().DrawSphere(bone->GetWorldPosition(), 0.5, ColorRGBA::Blue);*/
		

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

	Entity* mDudeEntity;
	SceneNode* mDudeSceneNode;

	int mFramePerSecond;

	struct SkinnedSphere
	{
		String BoneName;
		float Radius;
		float Offset;
	};
	std::vector<SkinnedSphere> mDudeCollisionSpheres;
};


int main()
{
	TesselationApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}