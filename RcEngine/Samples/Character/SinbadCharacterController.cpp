#include "SinbadCharacterController.h"
#include <Graphics/Camera.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationController.h>
#include <Scene/SceneManager.h>
#include <Input/InputSystem.h>
#include <Core/Environment.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

SinbadCharacterController::SinbadCharacterController( shared_ptr<Camera> viewCamera )
	: mMovingBack(false),
	mBaseAnimID(ANIM_NONE),
	mTopAnimID(ANIM_NONE),
	mRotateSpeed(Mathf::ToRadian(500.0f)),
	mRunSpeed(17.0f),
	mGravity(90.0f),
	mJumpAccel(30.0f)
{
	mCamera = viewCamera;

	SetupBody();
	SetupAnimation();
}

SinbadCharacterController::~SinbadCharacterController(void)
{
}

void SinbadCharacterController::SetupBody()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	mSinbadNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sinbad", float3(0, CHAR_HEIGHT, 0));
	mSinbady = sceneMan->CreateEntity("Sinbad", "./Sinbad/Sinbad.mesh",  "Custom");
	mSinbadNode->AttachObject(mSinbady);
	mSinbadNode->Translate(float3(0, 18.5, 0));

	// Sinbad Orientation
	const float3 forward(0, 0, -1);
	mMoveDirection = Transform(forward, mSinbadNode->GetRotation());

	// Sword BoneSceneNode
	mSheathLNode = mSinbady->CreateBoneSceneNode("SheathL", "Sheath.L");
	mSheathRNode = mSinbady->CreateBoneSceneNode("SheathR", "Sheath.R");
	mHandleLNode = mSinbady->CreateBoneSceneNode("HandL", "Handle.L");
	mHandleRNode = mSinbady->CreateBoneSceneNode("HandR", "Handle.R");

	mSwordL = sceneMan->CreateEntity("Swoard", "./Sinbad/Sword.mesh",  "Custom");
	mSwordR = sceneMan->CreateEntity("Swoard", "./Sinbad/Sword.mesh",  "Custom");

	mSheathLNode->AttachObject(mSwordL);
	mSheathRNode->AttachObject(mSwordR);
}

void SinbadCharacterController::SetupAnimation()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();

	const String AnimClips[] = { 
		"./Sinbad/IdleBase.anim",
		"./Sinbad/IdleTop.anim",
		"./Sinbad/RunBase.anim",
		"./Sinbad/RunTop.anim",
		"./Sinbad/HandsClosed.anim",
		"./Sinbad/HandsRelaxed.anim",
		"./Sinbad/DrawSwords.anim",
		"./Sinbad/SliceHorizontal.anim",
		"./Sinbad/SliceVertical.anim",
		"./Sinbad/Dance.anim", 
		"./Sinbad/JumpStart.anim",
		"./Sinbad/JumpLoop.anim",
		"./Sinbad/JumpEnd.anim",
	};
	static_assert( ARRAY_SIZE(AnimClips) == ANIM_COUNT, "AnimtionClip Not Match!" );

	assert(mSinbady);
	AnimationPlayer* skinAnimPlayer = mSinbady->GetAnimationPlayer();
	for (size_t i = 0; i < ARRAY_SIZE(AnimClips); ++i)
	{
		mAnimStates[i] = skinAnimPlayer->AddClip(
			resMan.GetResourceByName<AnimationClip>(RT_Animation, AnimClips[i], "Custom") );

		mAnimStates[i]->WrapMode = AnimationState::Wrap_Loop;
		mAnimStates[i]->SetClipStateBit(AnimationState::Clip_Is_Playing_Bit);
		mAnimStates[i]->SetEnable(false);

		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// Start in idle state
	SetTopAnimation(ANIM_IDLE_TOP);
	SetBaseAnimation(ANIM_IDLE_BASE);

	mAnimStates[ANIM_HANDS_RELAXED]->SetEnable(true);
	mSwordsDrawn = false;
}

void SinbadCharacterController::UpdateSmoothedMovementDirection(float deltaTime)
{
	InputSystem& inputSys = InputSystem::GetSingleton();

	// Forward vector relative to the camera along the x-z plane	
	float3 forward = mCamera->GetView();
	forward.Y() = 0;
	forward = Normalize(forward);

	// Right vector relative to the camera, always orthogonal to the forward vector
	float3 right = float3(forward.Z(), 0, -forward.X());
	
	// Are we moving backwards or looking backwards
	mMovingBack = mKeybordDirection.Z() < 0.1f;

	bool wasMovding = mIsMoving;
	mIsMoving = LengthSquared(mKeybordDirection) > 0.1f;

	// Target direction relative to the camera
	float3 targetDirection = mKeybordDirection.X() * right + mKeybordDirection.Z() * forward;

	// Grounded controls
	if (IsGrounded())
	{
		if (mKeybordDirection != float3::Zero() && mBaseAnimID != ANIM_DANCE)
		{
			Quaternionf toTarget = RotateTowards(mMoveDirection, targetDirection, float3(0, 1, 0));

			float yawToTarget, dummy;
			QuaternionToYawPitchRoll(yawToTarget, dummy, dummy, toTarget);
			
			// this is how much the character CAN turn this frame
			float yawAtSpeed = yawToTarget / fabs(yawToTarget) * deltaTime * mRotateSpeed;
			
			// turn as much as we can, but not more than we need to
			if (yawAtSpeed < 0) 
				yawToTarget = std::min<float>(0, std::max<float>(yawToTarget, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);


			else if (yawAtSpeed > 0)
				yawToTarget = std::max<float>(0, std::min<float>(yawToTarget, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);

			mSinbadNode->Rotate(QuaternionFromYawPitchRoll(yawToTarget, 0.0f, 0.0f));
	
			// reduce "turnability" if we're in midair
			if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

			mMoveDirection = Transform(float3(0, 0, -1), mSinbadNode->GetRotation());

			// move in current body direction (not the goal direction)
			mSinbadNode->Translate(float3(0, 0, -deltaTime * mRunSpeed * mAnimStates[mBaseAnimID]->GetWeight()), Node::TS_Local);
		}
	}

	if (mBaseAnimID == ANIM_JUMP_LOOP)
	{
		// if we're jumping, add a vertical offset too, and apply gravity
		mSinbadNode->Translate(float3(0, mVerticalVelocity * deltaTime, 0), Node::TS_Local);
		mVerticalVelocity -= mGravity * deltaTime;

		float3 pos = mSinbadNode->GetPosition();
		if (pos.Y() <= CHAR_HEIGHT + 18.5)
		{
			// if we've hit the ground, change to landing state
			pos.Y() = CHAR_HEIGHT + 18.5;
			mSinbadNode->SetPosition(pos);
			SetBaseAnimation(ANIM_JUMP_END, true);
			mTimer = 0;
		}
	}
}

bool SinbadCharacterController::IsGrounded() const
{
	return true;
}

void SinbadCharacterController::UpdateInput( float deltaTime )
{
	InputSystem& inputSys = InputSystem::GetSingleton();
	
	// Reset
	mKeybordDirection = float3(0, 0, 0);
	if (inputSys.KeyDown(KC_W)) mKeybordDirection.Z() = 1.0f;
	if (inputSys.KeyDown(KC_S)) mKeybordDirection.Z() = -1.0f;
	if (inputSys.KeyDown(KC_A)) mKeybordDirection.X() = -1.0f;
	if (inputSys.KeyDown(KC_D)) mKeybordDirection.X() = 1.0f;


	// Keyboards
	if (inputSys.KeyPress(KC_Q) && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// take swords out (or put them back, since it's the same animation but reversed)
		SetTopAnimation(ANIM_DRAW_SWORDS, true);
		mTimer = 0;
	}
	else if (!mSwordsDrawn && inputSys.KeyPress(KC_E))
	{
		if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
		{
			// start dancing
			SetBaseAnimation(ANIM_DANCE, true);
			SetTopAnimation(ANIM_NONE);
			// disable hand animation because the dance controls hands
			mAnimStates[ANIM_HANDS_RELAXED]->SetEnable(false);
		}
		else if (mBaseAnimID == ANIM_DANCE)
		{
			// stop dancing
			SetBaseAnimation(ANIM_IDLE_BASE);
			SetTopAnimation(ANIM_IDLE_TOP);
			// re-enable hand animation
			mAnimStates[ANIM_HANDS_RELAXED]->SetEnable(true);
		}
	}
	else if (inputSys.KeyPress(KC_Space) && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// jump if on ground
		SetBaseAnimation(ANIM_JUMP_START, true);
		SetTopAnimation(ANIM_NONE);
		mTimer = 0;
	}

	
	if (mKeybordDirection != float3::Zero() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		// start running if not already moving and the player wants to move
		SetBaseAnimation(ANIM_RUN_BASE, true);
		if (mTopAnimID == ANIM_IDLE_TOP) SetTopAnimation(ANIM_RUN_TOP, true);
	}
	else if (mKeybordDirection == float3::Zero() && mBaseAnimID == ANIM_RUN_BASE)
	{
		// stop running if already moving and the player doesn't want to move
		SetBaseAnimation(ANIM_IDLE_BASE);
		if (mTopAnimID == ANIM_RUN_TOP) SetTopAnimation(ANIM_IDLE_TOP);
	}

	// Mouse
	if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		if (inputSys.MouseButtonDown(MS_LeftButton))
		{
			SetTopAnimation(ANIM_SLICE_VERTICAL, true);
		}
		else if (inputSys.MouseButtonDown(MS_RightButton))
		{
			SetTopAnimation(ANIM_SLICE_HORIZONTAL, true);
		}

		mTimer = 0.0f;
	}
}

void SinbadCharacterController::SetBaseAnimation( AnimID id, bool reset )
{
	AnimID oldBaseAnimID = mBaseAnimID;

	if (mBaseAnimID >= 0 && mBaseAnimID < ANIM_COUNT)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnimStates[id]->SetEnable(true);
		mAnimStates[id]->BlendWeight = (oldBaseAnimID == ANIM_NONE ? 1.0f : 0.0f);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnimStates[id]->SetTime(0.0f);
	}
}

void SinbadCharacterController::SetTopAnimation( AnimID id, bool reset  )
{
	AnimID oldTopAnimID = mTopAnimID;

	if (mTopAnimID >= 0 && mTopAnimID < ANIM_COUNT)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnimStates[id]->SetEnable(true);
		mAnimStates[id]->BlendWeight = (oldTopAnimID == ANIM_NONE ? 1.0f : 0.0f);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnimStates[id]->SetTime(0.0f);
	}
}

void SinbadCharacterController::Update( float deltaTime )
{
	UpdateInput(deltaTime);
	UpdateSmoothedMovementDirection(deltaTime);
	UpdateAnimation(deltaTime);
}

void SinbadCharacterController::UpdateAnimation( float deltaTime )
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	float baseAnimSpeed = 1;
	float topAnimSpeed = 1;

	mTimer += deltaTime;

	if (mTopAnimID == ANIM_DRAW_SWORDS)
	{
		// flip the draw swords animation if we need to put it back
		topAnimSpeed = mSwordsDrawn ? -1.0f : 1.0f;

		// half-way through the animation is when the hand grasps the handles...
		if (mTimer >= mAnimStates[mTopAnimID]->GetDuration() / 2 &&
			mTimer - deltaTime < mAnimStates[mTopAnimID]->GetDuration() / 2)
		{
			// so transfer the swords from the sheaths to the hands
			if (mSwordsDrawn)
			{
				mHandleLNode->DetachAllObject();
				mHandleRNode->DetachAllObject();

				mSheathLNode->AttachObject(mSwordL);
				mSheathRNode->AttachObject(mSwordR);
			}
			else
			{
				mSheathLNode->DetachAllObject();
				mSheathRNode->DetachAllObject();

				mHandleLNode->AttachObject(mSwordL);
				mHandleRNode->AttachObject(mSwordR);
			}

			// change the hand state to grab or let go
			mAnimStates[ANIM_HANDS_CLOSED]->SetEnable(!mSwordsDrawn);
			mAnimStates[ANIM_HANDS_RELAXED]->SetEnable(mSwordsDrawn);
		}

		if (mTimer >= mAnimStates[mTopAnimID]->GetDuration())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
			else
			{
				SetTopAnimation(ANIM_RUN_TOP);
				mAnimStates[ANIM_RUN_TOP]->SetTime(mAnimStates[ANIM_RUN_BASE]->GetTime());
			}
			mSwordsDrawn = !mSwordsDrawn;
		}
	}
	else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
	{
		if (mTimer >= mAnimStates[mTopAnimID]->GetDuration())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
			else
			{
				SetTopAnimation(ANIM_RUN_TOP);
				mAnimStates[ANIM_RUN_TOP]->SetTime(mAnimStates[ANIM_RUN_BASE]->GetTime());
			}
		}

		// don't sway hips from side to side when slicing. that's just embarrasing.
		if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
	}
	else if (mBaseAnimID == ANIM_JUMP_START)
	{
		if (mTimer >= mAnimStates[mBaseAnimID]->GetDuration())
		{
			// takeoff animation finished, so time to leave the ground!
			SetBaseAnimation(ANIM_JUMP_LOOP, true);
			// apply a jump acceleration to the character
			mVerticalVelocity = mJumpAccel;
		}
	}
	else if (mBaseAnimID == ANIM_JUMP_END)
	{
		if (mTimer >= mAnimStates[mBaseAnimID]->GetDuration())
		{
			// safely landed, so go back to running or idling
			if (mKeybordDirection == float3::Zero())
			{
				SetBaseAnimation(ANIM_IDLE_BASE);
				SetTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				SetBaseAnimation(ANIM_RUN_BASE, true);
				SetTopAnimation(ANIM_RUN_TOP, true);
			}
		}
	}

	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnimStates[mBaseAnimID]->AdvanceTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnimStates[mTopAnimID]->AdvanceTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	FadeAnimations(deltaTime);
}


void SinbadCharacterController::FadeAnimations( float deltaTime )
{
	for (int i = 0; i < ANIM_COUNT; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			float newWeight = mAnimStates[i]->BlendWeight + deltaTime * ANIM_FADE_SPEED;
			mAnimStates[i]->BlendWeight = Clamp(newWeight, 0.0f, 1.0f);
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			float newWeight = mAnimStates[i]->BlendWeight - deltaTime * ANIM_FADE_SPEED;
			mAnimStates[i]->BlendWeight = Clamp(newWeight, 0.0f, 1.0f);
			if (newWeight <= 0)
			{
				mAnimStates[i]->SetEnable(false);
				mFadingOut[i] = false;
			}
		}
	}
}
