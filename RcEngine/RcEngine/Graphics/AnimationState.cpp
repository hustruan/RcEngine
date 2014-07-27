#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>
#include <Input/InputSystem.h>
#include <Core/Environment.h>

namespace RcEngine {

AnimationState::AnimationState( AnimationPlayer& animation, const shared_ptr<AnimationClip> clip )
	: mAnimation(animation),
	  mClip(clip),
	  mTime(0), 
	  BlendWeight(1.0f), 
	  PlayBackSpeed(1.0f), 
	  WrapMode(Wrap_Once),
	  mStateBits(0x00),
	  mEnable(true),
	  mFadeToClipState(nullptr)
{

}

AnimationState::~AnimationState()
{

}

void AnimationState::SetAnimationWrapMode( AnimationWrapMode wrapMode )
{
	WrapMode = wrapMode;
}

void AnimationState::SetEnable( bool enabled )
{
	mEnable = enabled;
}

const String& AnimationState::GetClipName() const
{
	return mClip->GetClipName();
}

float AnimationState::GetDuration() const
{
	return mClip->GetDuration();
}

void AnimationState::Apply()
{
	if (!IsEnabled() || !IsClipStateBitSet(Clip_Is_Playing_Bit))
		return;

	for (const AnimationClip::AnimationTrack& animTrack : mClip->mAnimationTracks)
	{
		// if no key frames, pass
		if (animTrack.KeyFrames.empty())
			continue;

		unordered_map<String, Bone*>::const_iterator found = mAnimation.mAnimateTargets.find(animTrack.Name);

		assert( found != mAnimation.mAnimateTargets.end() );
		Bone* bone = found->second;

		int32_t frame = animTrack.GetKeyFrameIndex(mTime);
		int32_t nextFrame = frame + 1;
		//printf("nextFrame = %d\n", nextFrame);

		bool interpolate = true;
		if (nextFrame >= (int32_t)animTrack.KeyFrames.size())
		{
			if (WrapMode != Wrap_Loop)
			{
				nextFrame = frame;
				interpolate = false;
			}
			else
				nextFrame = 0;
		}

		const AnimationClip::KeyFrame& keyframe = animTrack.KeyFrames[frame];

		if (!interpolate)
		{
			// No interpolation, blend between old transform & animation
			 bone->SetPosition( Lerp(bone->GetPosition(), keyframe.Translation, BlendWeight) );
			 bone->SetRotation( QuaternionSlerp(bone->GetRotation(), keyframe.Rotation, BlendWeight) );
			 bone->SetScale( Lerp(bone->GetScale(), keyframe.Scale, BlendWeight) );
		}
		else
		{		
			const AnimationClip::KeyFrame& nextKeyframe = animTrack.KeyFrames[nextFrame];

			//std::cout << keyframe.Time << "-->" << nextKeyframe.Time << std::endl;
			float timeInterval = nextKeyframe.Time - keyframe.Time;
			if (timeInterval < 0.0f)
				timeInterval += mClip->GetDuration();
			float t = timeInterval > 0.0f ? (mTime - keyframe.Time) / timeInterval : 1.0f;
		
			bone->SetPosition( Lerp(bone->GetPosition(),
			Lerp(keyframe.Translation, nextKeyframe.Translation, t), BlendWeight) );

			bone->SetRotation( QuaternionSlerp(bone->GetRotation(), 
			QuaternionSlerp(keyframe.Rotation, nextKeyframe.Rotation, t), BlendWeight) );

			bone->SetScale( Lerp(bone->GetScale(),
			Lerp(keyframe.Scale, nextKeyframe.Scale, t), BlendWeight) );
		}
	}
}

void AnimationState::AdvanceTime( float delta )
{
	float length = mClip->GetDuration();
	
	if (delta == 0 || length == 0)
		return;

	float timePos = GetTime() + delta;
	if (WrapMode == Wrap_Loop)
	{
		// wrap
		timePos = fmod(timePos, length);
		if(timePos < 0)
			timePos += length;    
	}
	else
	{
		if (timePos > length)
			SetClipStateBit(Clip_Is_End_Bit);
	}

	SetTime(timePos);
}

void AnimationState::SetTime( float time )
{
	// Clamp
	time = Clamp(time, 0.0f, mClip->GetDuration());

	if (mTime != time)
	{
		mTime = time;

		//if (mEnable)
		//	mAnimation.mParentMesh.MarkAnimationDirty();
	}
}

void AnimationState::SetFadeLength( float fadeLength )
{
	CrossFadeLength = fadeLength;
}

bool AnimationState::Update( float delta )
{
	if (IsClipStateBitSet(Clip_Is_Pause_Bit))
	{
		// paused
		return true;
	}

	if (IsClipStateBitSet(Clip_Is_End_Bit))
	{
		OnEnd();
		// Return false so the AnimationClip is removed from the running clips on the AnimationController.
		return false;
	}
	
	if (!IsClipStateBitSet(Clip_Is_Started_Bit))
	{
		// first time update, begin
		OnBegin();
	}
	else
	{
		// advance time
		AdvanceTime(delta * PlayBackSpeed);
	}

	// Notify any listeners of Animation events.
	if (!mAnimNotifies.empty())
	{
		if (PlayBackSpeed >= 0.0f)
		{
			while (mAninNofityIter != mAnimNotifies.end() && mTime >= mAninNofityIter->first)
			{
				(mAninNofityIter->second)(this, mTime);
				++mAninNofityIter;
			}
		}
		else
		{
			while (mAninNofityIter != mAnimNotifies.begin() && mTime <= mAninNofityIter->first)
			{
				(mAninNofityIter->second)(this, mTime);
				--mAninNofityIter;
			}
		}
	}

	float percentComplete = mTime / GetDuration();

	if (IsClipStateBitSet(Clip_Is_Fading_Bit))
	{
		assert(CrossFadeLength > 0);

		mCrossFadeOutElapsed += delta * PlayBackSpeed;

		if (mCrossFadeOutElapsed < CrossFadeLength)
        {
            // Calculate this clip's blend weight.
            float tempBlendWeight = (CrossFadeLength - mCrossFadeOutElapsed) / CrossFadeLength;
            
            // If this clip is fading in, adjust the crossfade clip's weight to be a percentage of your current blend weight
            if (IsClipStateBitSet(Clip_Is_FadeOut_Started_Bit))
            {
                BlendWeight = (std::max)(tempBlendWeight, 0.0f);
            }
            else
            {
                // Just set the blend weight.
                BlendWeight = (std::min)(1 - tempBlendWeight, 1.0f);
            }
        }
        else
        {  
			if (IsClipStateBitSet(Clip_Is_FadeIn_Started_Bit))
			{
				BlendWeight = 1.0f;
				ResetClipStateBit(Clip_Is_FadeIn_Started_Bit);
				ResetClipStateBit(Clip_Is_Fading_Bit);
			}
			else
			{
				BlendWeight = 0.0f;
				ResetClipStateBit(Clip_Is_FadeOut_Started_Bit);
				ResetClipStateBit(Clip_Is_Fading_Bit);

				// Fading out 
				SetClipStateBit(Clip_Is_End_Bit);
			}
        }
	}

	return true;
}


bool AnimationState::IsClipStateBitSet( uint8_t bits ) const
{
	return (mStateBits & bits) == bits ;
}

void AnimationState::SetClipStateBit( uint8_t bits )
{
	mStateBits |= bits;
}

void AnimationState::ResetClipStateBit( uint8_t bits )
{
	mStateBits &= ~bits;
}

void AnimationState::OnBegin()
{
	// Initialize animation to play.
	SetClipStateBit(Clip_Is_Started_Bit);

	if (!BeginNotify.empty())
		BeginNotify(this);
}

void AnimationState::OnEnd()
{
	BlendWeight = 1.0f;
	ResetClipStateBit(Clip_All_Bit);

	// Notify end listeners if any.
	if (!EndNotify.empty())
		EndNotify(this);
}

bool AnimationState::IsPlaying() const
{
	return IsClipStateBitSet(Clip_Is_Playing_Bit) && !IsClipStateBitSet(Clip_Is_Pause_Bit);
}

void AnimationState::Play()
{
	if (IsClipStateBitSet(Clip_Is_Playing_Bit))
	{
		// If paused, reset the bit and return.
		if (IsClipStateBitSet(Clip_Is_Pause_Bit))
		{
			ResetClipStateBit(Clip_Is_Pause_Bit);
			return;
		}

		// If the clip is set to be end, reset the flag.
		if (IsClipStateBitSet(Clip_Is_End_Bit))
			ResetClipStateBit(Clip_Is_End_Bit);

		SetClipStateBit(Clip_Is_Restarted_Bit);
	}
	else
	{
		SetClipStateBit(Clip_Is_Playing_Bit);
		mTime = 0;

		// add to controller
		mAnimation.mController->Schedule(this);
	}
}

void AnimationState::Pause()
{
	if (IsClipStateBitSet(Clip_Is_Playing_Bit) && !IsClipStateBitSet(Clip_Is_End_Bit))
		SetClipStateBit(Clip_Is_Pause_Bit);
}

void AnimationState::Resume()
{
	if (IsClipStateBitSet(Clip_Is_Playing_Bit) && IsClipStateBitSet(Clip_Is_Pause_Bit))
		ResetClipStateBit(Clip_Is_Pause_Bit);
}

void AnimationState::Stop()
{
	if (IsClipStateBitSet(Clip_Is_Playing_Bit))
	{
		// Reset the restarted and paused bits. 
		ResetClipStateBit(Clip_Is_Restarted_Bit);
		ResetClipStateBit(Clip_Is_Pause_Bit);

		// Mark the clip to removed from the AnimationController.
		SetClipStateBit(Clip_Is_End_Bit);
	}
}

void AnimationState::CrossFade( AnimationState* fadeClipState, float fadeLength )
{
	if (fadeClipState == this)
		return;

	if (!fadeClipState->IsClipStateBitSet(Clip_Is_Fading_Bit) &&
		!IsClipStateBitSet(Clip_Is_Fading_Bit))
	{
		// if the given clip is not fading, do fading	
		fadeClipState->BlendWeight = 0.0f;
		fadeClipState->SetClipStateBit( Clip_Is_FadeIn_Started_Bit | Clip_Is_Fading_Bit );
		fadeClipState->CrossFadeLength = fadeLength;
		fadeClipState->ResetCrossFadeTime();

		// Set this clip fade out
		SetClipStateBit( Clip_Is_FadeOut_Started_Bit | Clip_Is_Fading_Bit );
		CrossFadeLength = fadeLength;
		ResetCrossFadeTime();

		// If this clip is currently not playing, we should start playing it.
		if (!IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
			Play();

		// Start playing the cross fade clip.
		fadeClipState->Play();
	}
}

void AnimationState::AddNotify( const AnimatonNotify& notify, float fireTime )
{
	if (mAnimNotifies.empty())
	{
		mAnimNotifies.push_back(std::make_pair(fireTime, notify));

		if (IsClipStateBitSet(Clip_Is_Playing_Bit))
			mAninNofityIter = mAnimNotifies.begin();
	}
	else
	{
		for (auto iter = mAnimNotifies.begin(); iter != mAnimNotifies.end(); ++iter)
		{
			if (iter->first > fireTime)
			{
				iter = mAnimNotifies.insert(iter, std::make_pair(fireTime, notify));

				//// If playing, update the iterator if we need to.
				//// otherwise, it will just be set the next time the clip gets played.
				//if (IsClipStateBitSet(Clip_Is_Playing_Bit))
				//{
				//	mAninNofityIter = mAnimNotifies.begin();
				//}
				break;
			}
		}
	}

}






}