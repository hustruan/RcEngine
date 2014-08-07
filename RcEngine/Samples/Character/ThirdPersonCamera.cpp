#include "ThirdPersonCamera.h"
#include "SinbadCharacterController.h"
#include <Math/MathUtil.h>
#include <Graphics/Camera.h>
#include <Input/InputSystem.h>

ThirdPersonCamera::ThirdPersonCamera( SinbadCharacterController* characterController, shared_ptr<Camera> camera )
	: mController(characterController),
	  mCamera(camera),
	  mDistance(25),
	  mCameraHeight(2)
{
	float3 characterPosition = mController->GetCharacterPosition();
	const BoundingBoxf& characterBound = mController->GetCharacterBound();

	mCenterOffset = characterBound.Center() - characterPosition;	
	mHeadOffset = mCenterOffset;
	mHeadOffset.Y() = characterBound.Max.Y() - characterPosition.Y();

	mCameraYawAngle = 0;
	mCameraPitchAngle = Mathf::ToRadian(45.0f);

	mMouseDelta = float2::Zero();
}

ThirdPersonCamera::~ThirdPersonCamera(void)
{

}


void ThirdPersonCamera::UpdateInput( float deltaTime )
{
	InputSystem& inputSys = InputSystem::GetSingleton();

	float2 curMouseDelta(0.0f, 0.0f);

	const float RotateScaler = 0.001f;
	curMouseDelta.X() = inputSys.GetMouseMoveX() * RotateScaler;
	curMouseDelta.Y() = inputSys.GetMouseMoveY() * RotateScaler;
	inputSys.ForceCursorToCenter();

	// Smooth the relative mouse data over a few frames so it isn't 
	// jerky when moving slowly at low frame rates.
	float fPercentOfNew = 1.0f / 2.0f;
	float fPercentOfOld = 1.0f - fPercentOfNew;
	mMouseDelta = mMouseDelta * fPercentOfOld + curMouseDelta * fPercentOfNew;

	
	// Zoom 
	//printf("%d\n", inputSys.GetMouseMoveWheel());
	//mDistance += mDistance * inputSys.GetMouseMoveWheel() * 0.0005f;
	//mDistance = Clamp(mDistance, 8.0f, 25.0f);
}

void ThirdPersonCamera::Update( float deltaTime )
{
	UpdateInput(deltaTime);

	float3 targetPosition = mController->GetCharacterPosition();

	float3 targetCenter = targetPosition + mCenterOffset;
	float3 targetHead = targetPosition + mHeadOffset;
	

	// Update the pitch & yaw angle based on mouse movement
	mCameraYawAngle += mMouseDelta.X();
	mCameraPitchAngle += mMouseDelta.Y();

	mCameraPitchAngle = Clamp(mCameraPitchAngle, Mathf::ToRadian(-5.0f), Mathf::ToRadian(60.0f));

	Quaternionf cameraRot = QuaternionFromYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, 0.0f);
	float3 cameraOrigin = Transform( float3(0, 0, -mDistance), cameraRot );

	cameraOrigin += targetCenter;
	
	float3 cameraTarget = targetCenter + float3(0, mCameraHeight, 0);

	mCamera->CreateLookAt(cameraOrigin, cameraTarget);
}

