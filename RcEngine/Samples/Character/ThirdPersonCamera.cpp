#include "ThirdPersonCamera.h"
#include "CharacterController.h"
#include <Math/MathUtil.h>

inline float AngleDistance(float a, float b)
{
	//a = Mathf.Repeat(a, 360);
	//b = Mathf.Repeat(b, 360);

	//return Mathf.Abs(b - a);
	
	return fabsf(b-1);
}

ThirdPersonCamera::ThirdPersonCamera( CharacterController* characterController )
	: mController(characterController)
{
	float3 characterPosition = mController->GetCharacterPosition();
	const BoundingBoxf& characterBound = mController->GetCharacterBound();

	mCenterOffset = characterBound.Center() - characterPosition;	
	mHeadOffset = mCenterOffset;
	mHeadOffset.Y() = characterBound.Max.Y() - characterPosition.Y();
}

ThirdPersonCamera::~ThirdPersonCamera(void)
{

}

void ThirdPersonCamera::Apply()
{
	const float4x4& targetTransform = mController->GetCharacterTransform();

	Quaternionf targetOrient;
	float3 targetPosition, targetScale;
	MatrixDecompose(targetScale, targetOrient, targetPosition, targetTransform);

	float3 targetCenter = targetPosition + mCenterOffset;
	float3 targetHead = targetPosition + mHeadOffset;

	// Calculate the current & target rotation angles
	float3 targetEulerAngles;
	QuaternionToYawPitchRoll(targetEulerAngles.Y(), targetEulerAngles.X(), targetEulerAngles.Z(), targetOrient);
	float originalTargetAngle = targetEulerAngles.Y();

	float4x4 cameraTransform = MatrixInverse(mCamera->GetViewMatrix());
	float3 currentEulerAngles;
	RotationMatrixToYawPitchRoll(currentEulerAngles.Y(), currentEulerAngles.X(), currentEulerAngles.Z(), cameraTransform);
	float currentAngle = targetEulerAngles.Y();

	// Adjust real target angle when camera is locked
	float targetAngle = originalTargetAngle; 

	if (mSnap)
	{

	}
	else
	{
		if (mController->GetLockCameraTimer() < mLockCameraTimeout)
			targetAngle = currentAngle;
		
		// Lock the camera when moving backwards!
		// * It is really confusing to do 180 degree spins when turning around.
		if (AngleDistance (currentAngle, targetAngle) > 160 && mController->IsMovingBackwards())
			targetAngle += Mathf::PI;

		//currentAngle = Mathf.SmoothDampAngle(currentAngle, targetAngle, angleVelocity, angularSmoothLag, angularMaxSpeed);
	}


	// When jumping don't move camera upwards but only down!
	if (mController->IsJumping())
	{
		// We'd be moving the camera upwards, do that only if it's really high
		float newTargetHeight = targetCenter.Y() + mHeight;
		if (newTargetHeight < mTargetHeight || newTargetHeight - mTargetHeight > 5)
			mTargetHeight = targetCenter.Y() + mHeight;
	}
	// When walking always update the target height
	else
	{
		mTargetHeight = targetCenter.Y() + mHeight;
	}

	// Damp the height
	float currentHeight = cameraTransform.M42;
//	currentHeight = Mathf.SmoothDamp (currentHeight, mTargetHeight, heightVelocity, heightSmoothLag);

	// Convert the angle into a rotation, by which we then reposition the camera
	Quaternionf currentRotation = QuaternionFromYawPitchRoll(currentAngle, 0.0f, 0.0f);

	// Set the position of the camera on the x-z plane to: distance meters behind the target
	float3 newCameraPosition = targetCenter + Transform(float3(0, 0, -1), currentRotation) * mDistance;

	// Set the height of the camera
	newCameraPosition.Y() = currentHeight;

	// Always look at the target	
	SetUpRotation(targetCenter, targetHead);
}

void ThirdPersonCamera::SetUpRotation( const float3& centerPos, const float3& headPos )
{

}

