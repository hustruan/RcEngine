#ifndef CharacterController_h__
#define CharacterController_h__

#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Math/Vector.h>

using namespace RcEngine;


class CharacterController
{
public:

	enum CharacterState
	{
		Idle,
		Walking,
		Trotting,
		Running,
		Jumping,
	};


public:
	CharacterController(void);
	~CharacterController(void);

	inline bool IsJumping() const				{ return mJumping; }
	inline bool HasJumpReachedApex() const		{ return mJumpingReachedApex; }

	void SetupAnimation();

	

protected:
	bool IsGrounded() const;

	void UpdateSmoothedMovementDirection();

private:

	CharacterState mCharacterState;

	// The speed when walking
	float mWaldSpeed;

	// after trotAfterSeconds of walking we trot with trotSpeed
	float mTrotSpeed;

	// when pressing "Fire3" button (cmd) we start running
	float mRunSpeed;

	// How high do we jump when pressing jump and letting go immediately
	float jumpHeight;

	// The gravity for the character
	float mGravity;
	// The gravity in controlled descent mode
	float mSpeedSmoothing;
	float mRotateSpeed;
	float mTrotAfterSeconds;

	bool mCanJump;

	// Are we moving backwards (This locks the camera to not do a 180 degree spin)
	bool mMovingBack;
	
	// Is the user pressing any keys?
	bool mIsMoving;

	// The current move direction in x-z
	float3 mMoveDirection;

	// Are we jumping? (Initiated with jump button and not grounded yet)
	bool mJumping;
	bool mJumpingReachedApex;
};


#endif // CharacterController_h__