#include <Graphics/Animation.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>
#include <Scene/SceneManager.h>
#include <IO/FileStream.h>
#include <Core/Environment.h>
#include <Core/Exception.h>

namespace RcEngine {

AnimationPlayer::AnimationPlayer( )
	: mCurrentClipState(nullptr)
{
	mController = Environment::GetSingleton().GetSceneManager()->GetAnimationController();
}

AnimationPlayer::~AnimationPlayer()
{
	for (auto iter = mAnimationStates.begin(); iter != mAnimationStates.end(); ++iter)
		delete iter->second;

	mAnimationStates.clear();
}

AnimationState* AnimationPlayer::GetClip( const String& clipName ) const
{
	unordered_map<String, AnimationState*>::const_iterator found;
	
	found = mAnimationStates.find(clipName);
	if (found == mAnimationStates.end())
		return nullptr;

	return found->second;
}

AnimationState* AnimationPlayer::AddClip( const shared_ptr<AnimationClip>& clip )
{
	if (!clip->IsLoaded())
		clip->Load();

	if (mAnimationStates.find(clip->GetClipName()) != mAnimationStates.end())
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Same animation clip exits", "Animation::AddClip");

	AnimationState* newClipState = new AnimationState(*this, clip);
	mAnimationStates.insert( std::make_pair(clip->GetClipName(),  newClipState) );

	return newClipState;
}

void AnimationPlayer::StopAll()
{
	for (auto& kv : mAnimationStates)
		kv.second->Stop();
}

bool AnimationPlayer::IsPlaying( const String& clipName ) const
{
	unordered_map<String, AnimationState*>::const_iterator iter;
	iter = mAnimationStates.find(clipName);

	if ( iter == mAnimationStates.end())
		return false;

	return iter->second->IsPlaying();
}

void AnimationPlayer::PlayClip( const String& clipName )
{
	if (mAnimationStates.find(clipName) != mAnimationStates.end())
		mAnimationStates[clipName]->Play();
}

void AnimationPlayer::PauseClip( const String& clipName )
{
	if (mAnimationStates.find(clipName) != mAnimationStates.end())
		mAnimationStates[clipName]->Pause();
}

void AnimationPlayer::ResumeClip( const String& clipName )
{
	if (mAnimationStates.find(clipName) != mAnimationStates.end())
		mAnimationStates[clipName]->Resume();
}

void AnimationPlayer::StopClip( const String& clipName )
{
	if (mAnimationStates.find(clipName) != mAnimationStates.end())
		mAnimationStates[clipName]->Stop();
}

SkinnedAnimationPlayer::SkinnedAnimationPlayer( const shared_ptr<Skeleton>& skeleton )
{
	assert(skeleton != nullptr);

	for (uint32_t i = 0; i < skeleton->GetNumBones(); ++i)
	{
		Bone* bone = skeleton->GetBone(i);
		mAnimateTargets.insert( std::make_pair(bone->GetName(), bone));
	}
}

SkinnedAnimationPlayer::~SkinnedAnimationPlayer()
{

}


}


