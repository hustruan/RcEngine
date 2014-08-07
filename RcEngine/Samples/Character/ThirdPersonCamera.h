#pragma once

#include <Graphics/Camera.h>

using namespace RcEngine;

class SinbadCharacterController;

class ThirdPersonCamera
{
public:
	ThirdPersonCamera(SinbadCharacterController* characterController, shared_ptr<Camera> camera);
	~ThirdPersonCamera(void);
	
	void Update(float deltaTime);

protected:

	void UpdateInput(float deltaTime);

	void HandleRoatate( uint32_t action, int32_t value, float dt);

private:
	shared_ptr<Camera> mCamera;

	SinbadCharacterController* mController;
	
	float3 mCenterOffset;
	float3 mHeadOffset;

	// the height we want the camera to be above the target
	float mCameraHeight;

	float mTargetHeight;

	// The distance in the x-z plane to the target
	float mDistance;

	float mHeightVelocity;
	float mAngleVelocity;

	float2 mMouseDelta;

	float mCameraYawAngle;
	float mCameraPitchAngle;
};

