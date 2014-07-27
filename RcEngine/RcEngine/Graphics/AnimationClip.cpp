#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationController.h>
#include <Core/Exception.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <IO/PathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {


int32_t AnimationClip::AnimationTrack::GetKeyFrameIndex( float time ) const
{
	//if (time < 0) time = 0;
	int32_t index = 0;
	while (index < (int32_t)KeyFrames.size() && KeyFrames[index].Time < time)
		index ++;

	return (std::max)(0, index-1);
}


AnimationClip::AnimationClip(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Animation, creator, handle, name, group)
{

}

AnimationClip::~AnimationClip()
{
	
}

void AnimationClip::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();

	shared_ptr<Stream> clipStream = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *clipStream;	

	mClipName = PathUtil::GetFileName(mResourceName);
	mDuration = source.ReadFloat();

	// read track count
	uint32_t numTracks = source.ReadUInt();
	mAnimationTracks.resize(numTracks);

	for (uint32_t track = 0; track < numTracks; ++track)
	{
		String trackName = source.ReadString(); 

		AnimationClip::AnimationTrack& animTrack = mAnimationTracks[track];

		// read key frame count
		animTrack.Name = trackName;
		size_t numKeyframes = source.ReadUInt();
		animTrack.KeyFrames.resize(numKeyframes);

		for (uint32_t key = 0; key < numKeyframes; ++key)
		{
			AnimationClip::KeyFrame& animKey = animTrack.KeyFrames[key];

			animKey.Time = source.ReadFloat();
			source.Read(&animKey.Translation, sizeof(float3));
			source.Read(&animKey.Rotation, sizeof(Quaternionf));
			source.Read(&animKey.Scale, sizeof(float3));
		}
	}
}

void AnimationClip::UnloadImpl()
{

}

shared_ptr<Resource> AnimationClip::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<AnimationClip>(creator, handle, name, group);
}

//void AnimationClip::SetAnimationState( AnimationState* state )
//{
//	assert(mAnimationState == nullptr);
//	mAnimationState = state;
//}
//
//void AnimationClip::Play()
//{
//	if (mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
//	{
//		if (mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_Pause_Bit))
//		{
//			mAnimationState->ResetClipStateBit(AnimationState::Clip_Is_Pause_Bit);
//		}
//	}
//	else
//	{
//		mAnimationState->SetClipStateBit(AnimationState::Clip_Is_Playing_Bit);
//		mAnimationState->mAnimation.GetAnimationController()->Schedule(this);
//	}
//}
//
//void AnimationClip::Pause()
//{
//	if (mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit) &&
//		!mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_End_Bit))
//	{
//		mAnimationState->SetClipStateBit(AnimationState::Clip_Is_Pause_Bit);
//	}
//}
//
//void AnimationClip::Stop()
//{
//	if (mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
//	{
//		// Mark the clip to removed from the AnimationController.
//		mAnimationState->SetClipStateBit(AnimationState::Clip_Is_End_Bit);
//	}
//}
//
//bool AnimationClip::IsPlaying() const
//{
//	return mAnimationState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit);
//}
//
//void AnimationClip::CrossFade( AnimationClip* fadeClip, float fadeLength )
//{

//	}
//}



}