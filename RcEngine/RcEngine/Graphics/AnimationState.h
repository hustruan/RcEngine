#ifndef AnimationState_h__
#define AnimationState_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class AnimationClip;

class _ApiExport AnimationState
{
public:
	enum AnimationWrapMode
	{
		Wrap_Once,
		Wrap_Loop,
	};      

	enum AnimationStateBits
	{
		Clip_Is_Playing_Bit			= 0x01,   // Bit representing whether AnimationClip is a running clip in AnimationController
		Clip_Is_Started_Bit			= 0x02,   // Bit representing whether the AnimationClip has actually been started (ie: received first call to update())
		Clip_Is_Pause_Bit			= 0x04,   // Bit representing if the clip is currently paused.
		Clip_Is_End_Bit				= 0x08,   // Bit representing whether the clip has ended and should be removed from the AnimationController.
		Clip_Is_FadeOut_Started_Bit = 0x10,
		Clip_Is_FadeIn_Started_Bit  = 0x20,
		Clip_Is_Fading_Bit			= 0x40,
		Clip_Is_Restarted_Bit		= 0x80,
		Clip_All_Bit				= 0xFF
	};

	// Animation event callback at specified time pos between [0, length]. 
	typedef fastdelegate::FastDelegate<void(AnimationState*, float)> AnimatonNotify;
	typedef fastdelegate::FastDelegate<void(AnimationState*)> AnimatonBeginNotify;
	typedef fastdelegate::FastDelegate<void(AnimationState*)> AnimatonEndNotify;

public:
	AnimationState(AnimationPlayer& animation, const shared_ptr<AnimationClip> clip);
	~AnimationState();

	const String& GetClipName() const; 

	/**
	 * Add animation event callback.
	 */
	void AddNotify(const AnimatonNotify& notify, float fireTime);
	
	/**
	 * Update animation state, return false if finished, so will be removed
	 * from running clips in animation controller.
	 */
	bool Update(float delta);

	void SetAnimationWrapMode( AnimationWrapMode wrapMode );
	AnimationWrapMode GetAnimationWrapMode() const { return WrapMode; }

	void SetEnable( bool enabled );
	bool IsEnabled() const { return mEnable; } 

	/**
	 * Current animation time position.
	 */
	void SetTime( float time );
	float GetTime() const { return mTime; }

	/**
	 * Get animation duration in seconds.
	 */
	float GetDuration() const;

	float GetWeight() const { return BlendWeight; }

	uint8_t GetLayer() const { return mLayer; }
	
	void SetFadeLength(float fadeLength);

	/**
	 * Applies an animation track to the designated target. 
	 */
	void Apply();

	void ResetCrossFadeTime()	{ mCrossFadeOutElapsed = 0.0f; }

	void SetClipStateBit(uint8_t bits);
	void ResetClipStateBit(uint8_t bits);
	bool IsClipStateBitSet(uint8_t bits) const;

	void Play();
	void Pause();
	void Resume();
	void Stop();
	void CrossFade(AnimationState* clipState, float fadeLength);

	bool IsPlaying() const;

private:
	void OnBegin();
	void OnEnd();
	
	/**
	 * Advance the animation by delta time.
	 */
	void AdvanceTime( float delta );

	
private:

	AnimationPlayer& mAnimation;

	// The clip that is being played by this animation state.
	shared_ptr<AnimationClip> mClip;
	
	uint8_t mStateBits;

	bool mEnable;

	// The current time of the animation.
	float mTime;
	
	// Cross fade time, and fade to clip state
	float mCrossFadeOutElapsed;
	AnimationState* mFadeToClipState;
	
	// Ordered collection of listeners on the clip.
	std::list<std::pair<float, AnimatonNotify>> mAnimNotifies;
	
	// Iterator that points to the next listener event to be triggered.
	std::list<std::pair<float, AnimatonNotify>>::iterator mAninNofityIter;
		 
public:

	// The weight of animation
	float BlendWeight;

	// The playback speed of the animation. 1 is normal playback speed.
	float PlayBackSpeed;

	// Wrapping mode of the animation.
	AnimationWrapMode WrapMode;

	// The layer of the animation. When calculating the final blend weights, animations in higher layers will get their weights
	uint8_t mLayer;

	float CrossFadeLength;

	AnimatonBeginNotify BeginNotify;
	AnimatonEndNotify EndNotify;
};

}


#endif // AnimationState_h__