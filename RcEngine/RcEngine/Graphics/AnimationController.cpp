#include <Graphics/AnimationController.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>

namespace RcEngine {

AnimationController::AnimationController()
	: mState(Idle)
{

}

AnimationController::~AnimationController()
{

}

void AnimationController::Update( float elapsedTime )
{
	if (mState != Running)
		return;

	// Loop through running clips and call update() on them.
	std::list<AnimationState*>::iterator clipIter = mRunningClips.begin();
	while (clipIter != mRunningClips.end())
	{
		AnimationState* clipState = *clipIter;

		if( !clipState->Update(elapsedTime) )
		{
			clipIter = mRunningClips.erase(clipIter);
		}
		else
			++clipIter;
	}

	if (mRunningClips.empty())
		mState = Idle;
}

void AnimationController::Schedule(AnimationState* clipState)
{
	if (mRunningClips.empty())
		mState = Running;

	assert(clipState);
	mRunningClips.push_back(clipState);
}

void AnimationController::Unschedule(AnimationState* clipState)
{
	std::list<AnimationState*>::iterator found;

	found = std::find(mRunningClips.begin(), mRunningClips.end(), clipState);
	if ( found != mRunningClips.end() )
	{
		mRunningClips.erase(found);

		if (mRunningClips.empty())
			mState = Idle;
	}
}

}