#ifndef Animation_h__
#define Animation_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class Bone;
class Skeleton;

class _ApiExport AnimationPlayer
{
	friend class AnimationState;

public:
	AnimationPlayer();
	~AnimationPlayer();

	bool IsPlaying(const String& clipName) const;

	void PlayClip(const String& clipName);
	void PauseClip(const String& clipName);
	void ResumeClip(const String& clipName);
	void StopClip(const String& clipName);
	void StopAll();

	AnimationState* GetClip( const String& clip ) const;
	const unordered_map<String, AnimationState*>& GetAllClip() const { return mAnimationStates; }

	AnimationState* AddClip( const shared_ptr<AnimationClip>& clip );

	AnimationController* GetAnimationController() const { return mController; }

public:
	static shared_ptr<AnimationPlayer> LoadFrom(Mesh& parentMesh, Stream& source);

	// Events
	EventHandler EventAnimationCompleted;

protected:

	AnimationController* mController; 

	AnimationState* mCurrentClipState;

	unordered_map<String, AnimationState*> mAnimationStates;

	unordered_map<String, Bone*> mAnimateTargets;
};


class _ApiExport SkinnedAnimationPlayer : public AnimationPlayer
{
public:
	SkinnedAnimationPlayer(const shared_ptr<Skeleton>& skeleton);
	~SkinnedAnimationPlayer();

	void CrossFade( const String& fadeClip, float fadeLength );

private:
	
};

}





#endif // Animation_h__
