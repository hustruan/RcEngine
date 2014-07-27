#include "Character.h"
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/SamplerState.h>
#include <Graphics/Skeleton.h>
#include <IO/FileSystem.h>
#include <Core/Context.h>
#include <Math/MathUtil.h>
#include <Graphics/AnimationClip.h>
#include <Input/InputSystem.h>

#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

Character::Character()
{

}

Character::~Character()
{
}

void Character::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	mActorEntity = sceneMan.CreateEntity("Arthas", "Arthas/Mesh_ArthasLichKing.mesh",  "Custom");
	mActorNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Arthas");
	mActorNode->SetPosition(float3(0, 0, 0));
	mActorNode->SetScale(float3(3, 3, 3));
	mActorNode->SetRotation(QuaternionFromYawPitchRoll(Mathf::ToRadian(-90.0f), 0.0f, 0.0f));
	mActorNode->AttachObject(mActorEntity);

	mSwordEntity = sceneMan.CreateEntity("Sword", "Arthas/Mesh_Sword_2H_Frostmourne_D_01.mesh",  "Custom");
	BoneFollower* swordNode = mActorEntity->AttachObjectToBone("wepson", mSwordEntity);
	swordNode->SetPosition(float3(4.5, -7, 0));

	mAnimationPlayer = (SkinnedAnimationPlayer*)mActorEntity->GetAnimationPlayer();
	mAnimationState[Anim_Random] = mAnimationPlayer->GetClip("Take 001");
	mAnimationState[Anim_Random]->WrapMode = AnimationState::Wrap_Once;
	mAnimationState[Anim_Random]->EndNotify.bind(this, &Character::AnimationEnd);

	String clipName = "Arthas/Walk.anim";
	shared_ptr<AnimationClip> clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Walk");
	mAnimationState[Anim_Walk] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Anim_Walk]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Run.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Run");
	mAnimationState[Anim_Run] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Anim_Run]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Fighting_Standby.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Fighting_Standby");
	mAnimationState[Anim_Fighting_Standby] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Anim_Fighting_Standby]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Standby.anim";
	clip = std::static_pointer_cast<AnimationClip>(
	ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Standby");
	mAnimationState[Anim_Standby] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Anim_Standby]->WrapMode = AnimationState::Wrap_Loop;
	
	clipName = "Arthas/Casting.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Casting");
	mAnimationState[Anim_Casting] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Anim_Casting]->WrapMode = AnimationState::Wrap_Once;
	mAnimationState[Anim_Casting]->EndNotify.bind(this, &Character::AnimationEnd);

	//for (size_t i = 0; i < Anim_Count; ++i)
	//{
	//	mAnimationState[i]->PlayBackSpeed *= 0.5f;
	//}

	mAnimationState[Anim_Walk]->Play();
	mCurrAnimID = Anim_Walk;
}

//void Character::LoadContent()
//{
//	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
//	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
//	ResourceManager& resMan = ResourceManager::GetSingleton();
//	
//	mActorEntity = sceneMan.CreateEntity("Arthas", "Arthas/Mesh_ArthasLichKing.mesh",  "Custom");
//	mActorNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Dude");
//	mActorNode->SetPosition(float3(0, 0, 0));
//	mActorNode->SetScale(float3(3, 3, 3));
//	//mActorNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(-90.0f), 0.0f, 0.0f));
//	mActorNode->AttachObject(mActorEntity);
//	
//	mSwordEntity = sceneMan.CreateEntity("nurbsToPoly1", "Arthas/Mesh_Sword_2H_Frostmourne_D_01.mesh",  "Custom");
//	BoneFollower* swordNode = mActorEntity->AttachObjectToBone("wepson", mSwordEntity);
//	swordNode->SetPosition(float3(0, 0, 0));
//	
//	//mSwordNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sword");
//	//mSwordNode->SetPosition(float3(28, 31, 2));
//	////mSwordNode->SetPosition(float3(-1.3f, 2.1f, 0.1f));
//	//mSwordNode->AttachObject(mSwordEntity);
//	
//	mAnimationPlayer = (SkinnedAnimationPlayer*)mActorEntity->GetAnimationPlayer();
//	mAnimationState[Anim_Random] = mAnimationPlayer->GetClip("Take 001");
//	mAnimationState[Anim_Random]->WrapMode = AnimationState::Wrap_Loop;
//	//mAnimationState[Anim_Random]->EndNotify.bind(this, &Character::AnimationEnd);
//
//	mCurrAnimID = Anim_Walk;
//	mAnimationState[mCurrAnimID]->Play();
//}

void Character::Update( float dt )
{
	InputSystem& inputSys =  InputSystem::GetSingleton();

	if (inputSys.KeyPress(KC_LeftShift))
	{	
		mAnimationState[mCurrAnimID]->CrossFade(mAnimationState[Anim_Run], 0.5f);
		mCurrAnimID = Anim_Run;
	}
	else if (inputSys.KeyPress(KC_RightShift))
	{
		mAnimationState[mCurrAnimID]->CrossFade(mAnimationState[Anim_Walk], 0.5f);
		mCurrAnimID = Anim_Walk;
	}
	else if (inputSys.KeyPress(KC_Q))
	{
		mAnimationState[mCurrAnimID]->Stop();

		mCurrAnimID = Anim_Random;
		mAnimationState[Anim_Random]->Play();
	}
	else if (inputSys.KeyPress(KC_E))
	{
		mAnimationState[mCurrAnimID]->Stop();

		mCurrAnimID = Anim_Casting;
		mAnimationState[Anim_Casting]->Play();
	}
}

void Character::AnimationEnd( AnimationState* state )
{
	mAnimationState[mCurrAnimID]->Stop();

	mCurrAnimID = Anim_Walk;
	mAnimationState[Anim_Walk]->Play();
}
