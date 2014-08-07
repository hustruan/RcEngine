#ifndef CharacterController_h__
#define CharacterController_h__

#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Math/Vector.h>

using namespace RcEngine;

class SinbadCharacterController
{
public:
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE,
		ANIM_COUNT = ANIM_NONE
	};

public:
	SinbadCharacterController(shared_ptr<Camera> viewCamera);
	~SinbadCharacterController(void);


	inline const float4x4& GetCharacterTransform() const	{ return mSinbady->GetWorldTransform(); }
	inline float3 GetCharacterPosition() const				{ return mSinbadNode->GetWorldPosition(); }
	inline const BoundingBoxf& GetCharacterBound() const	{ return mSinbady->GetWorldBoundingBox(); }

	void Update(float deltaTime);

protected:
	
	void SetupBody();
	void SetupAnimation();

	void SetTopAnimation(AnimID id, bool reset = false);
	void SetBaseAnimation(AnimID id, bool reset = false);
	
	void UpdateInput(float deltaTime);
	void UpdateAnimation(float deltaTime);
	void FadeAnimations(float deltaTime);

	bool IsGrounded() const;
	void UpdateSmoothedMovementDirection(float deltaTime);

private:

	shared_ptr<Camera> mCamera;

	SceneNode* mSinbadNode;

	// Bone scene node for Sword
	SceneNode* mSheathLNode;
	SceneNode* mSheathRNode;
	SceneNode* mHandleLNode;
	SceneNode* mHandleRNode;

	Entity* mSinbady;
	Entity* mSwordL;
	Entity* mSwordR;

	float mTimer;

	AnimID mTopAnimID, mBaseAnimID;
	AnimationState* mAnimStates[ANIM_COUNT];
	bool mFadingIn[ANIM_COUNT];
	bool mFadingOut[ANIM_COUNT];

	bool mSwordsDrawn;

	// When pressing "Fire3" button (cmd) we start running
	float mRunSpeed;

	// How high do we jump when pressing jump and letting go immediately
	float mVerticalVelocity;

	float mJumpAccel;

	// The gravity for the character
	float mGravity;

	float mRotateSpeed;

	// Are we moving backwards (This locks the camera to not do a 180 degree spin)
	bool mMovingBack;
	
	// Is the user pressing any keys?
	bool mIsMoving;

	// The current move direction in x-z
	float3 mMoveDirection;

	float3 mKeybordDirection;
};


#endif // CharacterController_h__