#include "Graphics/CameraController1.h"
#include "Graphics/Camera.h"
#include "MainApp/Application.h"
#include "Math/MathUtil.h"

namespace RcEngine {
namespace Test {

CameraController::CameraController()
	: mAttachedCamera(nullptr)
{

}

void CameraController::AttachCamera( Camera& camera )
{
	mAttachedCamera = &camera;
}

void CameraController::DetachCamera()
{
	mAttachedCamera = nullptr;
}

//---------------------------------------------------------------------------------------------
FPSCameraControler::FPSCameraControler()
	: mMouseDelta(0.0f, 0.0f),
	  mMoveVelocity(0.0f, 0.0f, 0.0f),
	  mRotationVelocity(0.0f, 0.0f),
	  mMoveSpeed(5.0f),
	  mRotateSpeed(0.001f),
	  mMoveInertia(false),
	  mTotalInertiaTime(0.25f),
	  mFramesToSmoothMouseData(2.0f)
{

}

FPSCameraControler::~FPSCameraControler()
{

}

void FPSCameraControler::SetMoveInertia( bool enable, float totalTime /*= 0.25f*/ )
{
	mMoveInertia = enable;
	mTotalInertiaTime = totalTime;
}

void FPSCameraControler::UpdateInput( bool keyboardInput, bool mouseInput, bool resetCursorAfterMove )
{
	InputSystem& inputSys = InputSystem::GetSingleton();
	Window* mainWindow = Application::msApp->GetMainWindow();

	mKeybordDirection = float3::Zero();
	if (keyboardInput)
	{
		if (inputSys.KeyDown(KC_W))	mKeybordDirection.Z() += 1.0f;
		if (inputSys.KeyDown(KC_S))	mKeybordDirection.Z() -= 1.0f;
		if (inputSys.KeyDown(KC_A))	mKeybordDirection.X() -= 1.0f;
		if (inputSys.KeyDown(KC_D))	mKeybordDirection.X() += 1.0f;
	}
	
	float2 curMouseDelta(0.0f, 0.0f);
	if (mouseInput)
	{
		curMouseDelta.X() = static_cast<float>( inputSys.GetMouseMoveX() );
		curMouseDelta.Y() = static_cast<float>( inputSys.GetMouseMoveY() );
	}

	if (resetCursorAfterMove)
		inputSys.ForceCursorToCenter();

	// Smooth the relative mouse data over a few frames so it isn't 
	// jerky when moving slowly at low frame rates.
	float fPercentOfNew = 1.0f / mFramesToSmoothMouseData;
	float fPercentOfOld = 1.0f - fPercentOfNew;
	mMouseDelta = mMouseDelta * fPercentOfOld + curMouseDelta * fPercentOfNew;
}

void FPSCameraControler::Update( float deltaTime )
{
	InputSystem& inputSys = InputSystem::GetSingleton();

	//bool updateRotate = inputSys.MouseButtonDown(MS_LeftButton);
	bool updateRotate = inputSys.MouseButtonDown(MS_RightButton);

	UpdateInput(true, updateRotate, false);
	
	float3 vAccel = mKeybordDirection;

	//Make sure movements are normalized so that moving forward at the same time as strafing doesn't move your character faster
	vAccel.Normalize();
	vAccel = vAccel * mMoveSpeed;

	// Update Velocity
	if (mMoveInertia)
	{
		// Is there any acceleration this frame
		if (LengthSquared(vAccel) > 0.005f)
		{
			// If so, then this means the user has pressed a movement key 
			// so change the velocity immediately to acceleration 
			// upon keyboard input.  This isn't normal physics
			// but it will give a quick response to keyboard input
			mMoveVelocity = vAccel;
			mInertiaTime = mTotalInertiaTime;
			mMoveVelocityInertia = vAccel / mInertiaTime;
		}
		else
		{
			// If no key being pressed, then slowly decrease velocity to 0
			if( mInertiaTime > 0.0f )
			{
				// Drag until timer is <= 0
				mMoveVelocity -= mMoveVelocityInertia * deltaTime;
				mInertiaTime -= deltaTime;
			}
			else
			{
				// Zero velocity
				mMoveVelocity = float3(0, 0, 0);
			}
		}
	}
	else
	{
		mMoveVelocity = vAccel;
	}

	float3 deltaMove = mMoveVelocity * deltaTime;

	float3 vWorldUp, vWorldAhead;
	if (updateRotate)
	{
		// Mouse Smoothing
		mRotationVelocity = mMouseDelta * mRotateSpeed;

		// Update the pitch & yaw angle based on mouse movement
		mCameraYawAngle += mRotationVelocity.X();
		mCameraPitchAngle += mRotationVelocity.Y();

		// Limit pitch to straight up or straight down
		mCameraPitchAngle = Clamp(mCameraPitchAngle, -Mathf::HALF_PI, +Mathf::HALF_PI );

		// Build camera rotation
		mCameraRot = QuaternionFromYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, 0.0f);

		// Transform vectors based on camera's rotation matrix
		
		float3 vLocalUp = float3( 0, 1, 0 );
		float3 vLocalAhead = float3( 0, 0, 1 );
		vWorldUp = Transform(vLocalUp, mCameraRot );
		vWorldAhead = Transform( vLocalAhead, mCameraRot );
	}
	else
	{
		 vWorldUp = mAttachedCamera->GetUp();
		 vWorldAhead = mAttachedCamera->GetView();
	}
	
	float3 newCameraPos = mAttachedCamera->GetPosition() + Transform(deltaMove, mCameraRot);
	
	// Update View Matrix
	mAttachedCamera->CreateLookAt(newCameraPos, newCameraPos + vWorldAhead, vWorldUp);
}

void FPSCameraControler::AttachCamera( Camera& camera )
{
	CameraController::AttachCamera(camera);		

	Quaternionf quat;
	float3 scale, pos;
	MatrixDecompose(scale, quat, pos, camera.GetViewMatrix());

	mCameraRot = QuaternionInverse(quat);
	QuaternionToYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, mCameraRollAngle, mCameraRot);


	//float3 X = mAttachedCamera->GetRight();
	//float3 Y = mAttachedCamera->GetUp();
	//float3 Z = mAttachedCamera->GetView();

	//float4x4 rot(X[0], X[1], X[2], 0,
	//	Y[0], Y[1], Y[2], 0,
	//	Z[0], Z[1], Z[2], 0,
	//	0, 0, 0, 1);

	//float y, p, r;
	//RotationMatrixToYawPitchRoll(y, p, r, rot);
}



}
}