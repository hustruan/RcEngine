#pragma once

#include <Graphics/Camera.h>

using namespace RcEngine;

class CharacterController;

class ThirdPersonCamera
{
public:
	ThirdPersonCamera(CharacterController* characterController);
	~ThirdPersonCamera(void);

protected:
	void Apply();
	void SetUpRotation(const float3& centerPos, const float3& headPos);


private:
	Camera* mCamera;

	CharacterController* mController;
	
	float3 mCenterOffset;
	float3 mHeadOffset;

	// the height we want the camera to be above the target
	float mHeight;

	float mTargetHeight;


	// The distance in the x-z plane to the target
	float mDistance;


	float mHeightVelocity;
	float mAngleVelocity;

	float mLockCameraTimeout;

	bool mSnap;
};

