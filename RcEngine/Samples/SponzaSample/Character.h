#ifndef Player_h__
#define Player_h__

#include <Core/Prerequisites.h>
#include <Graphics/Animation.h>

using namespace RcEngine;

class Character
{
public:
	enum AnimID
	{
		Anim_Standby = 0,
		Anim_Walk,
		Anim_Run,
		Anim_Random,
		Anim_Casting,
		Anim_Fighting_Standby,
		Anim_Count
	};

public:
	Character();
	~Character();

	void LoadContent();
	void Update(float dt);

private:
	void AnimationEnd(AnimationState* state);

private:
	SkinnedAnimationPlayer* mAnimationPlayer;

	Entity* mActorEntity;
	SceneNode* mActorNode;

	Entity* mSwordEntity;
	SceneNode* mSwordNode;

	AnimID mCurrAnimID;
	AnimationState* mAnimationState[Anim_Count];
};

#endif // Player_h__
