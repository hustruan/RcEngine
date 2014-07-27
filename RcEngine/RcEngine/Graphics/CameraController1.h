#ifndef CameraController_h__
#define CameraController_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>
#include <Input/InputSystem.h>
#include <Input/InputEvent.h>

namespace RcEngine {
namespace Test { 

class _ApiExport CameraController
{
public:	
	CameraController();
	virtual ~CameraController() {}

	virtual void Update(float deltaTime) = 0;

	virtual void AttachCamera(Camera& camera);
	virtual void DetachCamera();

protected:
	Camera* mAttachedCamera;
};

class _ApiExport FPSCameraControler : public CameraController
{
public:
	FPSCameraControler();
	virtual ~FPSCameraControler();

	virtual void Update(float deltaTime);
	virtual void AttachCamera(Camera& camera);

	inline void  SetMoveSpeed(float speed)							{ mMoveSpeed = speed; }
	inline float GetMoveSpeed() const								{ return mMoveSpeed; }

	inline void  SetRotateSpeed(float speed)						{ mRotateSpeed = speed; }
	inline float GetRotateSpeed() const								{ return mRotateSpeed; }

	inline void SetMoveInertia(bool enable, float totalTime = 0.25f);

protected:
	void UpdateInput(bool keyboardInput, bool mouseInput, bool resetCursorAfterMove);

protected:
	float3 mKeybordDirection;
	float2 mMouseDelta;
	
	float3 mMoveVelocity;
	float2 mRotationVelocity;

	float mMoveSpeed;
	float mRotateSpeed;

	float mFramesToSmoothMouseData;

	bool mMoveInertia;
	float mTotalInertiaTime;
	float mInertiaTime;
	float3 mMoveVelocityInertia;

	float mCameraYawAngle;
	float mCameraPitchAngle;
	float mCameraRollAngle;

	Quaternionf mCameraRot;
};

}
}

#endif // CameraController_h__
