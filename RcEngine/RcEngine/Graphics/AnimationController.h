#ifndef AnimationController_h__
#define AnimationController_h__

#include <Core/Prerequisites.h>

namespace RcEngine {


class _ApiExport AnimationController
{
	friend class AnimationPlayer;

public:
	enum State
	{
		Running,
		Idle,
		Paused,
		Stopped
	};

public:
	AnimationController();
	~AnimationController();
	
	State GetState() const	{ return mState; }

	void Update(float elapsedTime);

	void Unschedule(AnimationState* clipState);
	void Schedule(AnimationState* clipState);

	void Pause();
	void Resume();

private:
	State mState;

	// A list of running AnimationClips.
	std::list<AnimationState*> mRunningClips;     
};

}


#endif // AnimationController_h__
