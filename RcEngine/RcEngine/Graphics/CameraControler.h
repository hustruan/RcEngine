#ifndef CameraControler_h__
#define CameraControler_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>


namespace RcEngine {
		
/**
* Camera controller. Controller provides camera movement and rotation control using keyboard and mouse.
*/
class _ApiExport CameraControler 
{
protected:
	CameraControler();

public:
	virtual ~CameraControler();

	//virtual void Update(float deltaTime);


	virtual void AttachCamera(Camera* camera);
	void DetachCamera();



protected:
	Camera* mAttachedCamera;

};

class _ApiExport FPSCameraControler : public CameraControler
{
public:
	enum Action
	{
		TurnLeftRight,
		TurnUpDown,
		RollLeft,
		RollRight,

		Forward,
		Backward,
		MoveLeft,
		MoveRight
	};

public:
	FPSCameraControler();
	~FPSCameraControler();

	void AttachCamera(Camera* camera);

protected:
	void Move(float x, float y, float z);
	void Rotate(float yaw, float pitch, float roll);
	void HandleMove(uint32_t action, bool value, float delta);
	void HandleRoatate(uint32_t action, int32_t value, float delta);

protected:
	Quaternionf mCameraRot;
	float mCameraYawAngle;
	float mCameraPitchAngle;
	float mCameraRollAngle;


	float3 mMoveVelocity; 
	float3 mRotVelocity;	
};


class _ApiExport ArcBall
{
public:
	ArcBall();
	ArcBall( int32_t windowWidth, int32_t windowHeight );
	~ArcBall();

	void Reset();
	void SetWindowSize( int32_t windowWidth, int32_t windowHeight );
	void SetCenterAndRadius( const float2& center, float radius );

	float3 GetConstraintAxis() const			    		{ return mConstraintAxis; }
	void SetConstraintAxis( const float3& axis )    		{ mConstraintAxis = axis; mUseConstraint = true; }
	void SetNoConstraintAxis( )								{ mUseConstraint = false; }
	bool IsUsingConstraint() const							{ return mUseConstraint; }

	void OnMove( int32_t mouseX, int32_t mouseY );
	void OnBegin( int32_t mouseX, int32_t mouseY );
	void OnEnd();

	bool IsDragging() const { return mDrag; }

	Quaternionf GetRotation() const { return mQuatNow; }

private:
	float3 ScreenToSphere( float screenX, float screenY );

private:
			
	int32_t mWindowWidth;		// arc ball's window width
	int32_t mWindowHeight;		// arc ball's window height	

	float2 mCenter;		 // center of arc ball in screen coordinates
	float mRadius;              // arc ball's radius in screen coordinates
	
	float mRadiusTranslation;   // arc ball's radius for translating the target

	bool mUseConstraint;
	float3 mConstraintAxis;

	bool mDrag;

	Quaternionf mQuatNow;
	Quaternionf mQuatDown;

	float2 mLastMousePoint;

	float3 mDownPt;			// starting point of rotation arc on sphere
	float3 mCurrentPt;		// current point of rotation arc on sphere
};

class _ApiExport ModelViewerCameraControler : public CameraControler
{
public:
	enum Action 
	{
		ModelView,
		CameraView,
		Turn
	};

public:
	ModelViewerCameraControler();
	~ModelViewerCameraControler();

	void AttachCamera( Camera* camera );

	/**
	 * Set window size, the arcball need window size. by default, arcball 
	 * center is window center, radius is half window size.
	 */
	void SetWindowSize( int32_t width, int32_t height );
	
	/**
	 * Set arcball center and radius
	 */
	void SetCenterAndRadius( const float2& center, float radis );


	/**
	 * Get the world matrix of model
	 */
	float4x4 GetWorldMatrix() const	{ return mWorld; }

protected:
	void HandleModelView(uint32_t action, bool value, float delta);
	void HadnleCameraView(uint32_t action, bool value, float delta);

protected:
	ArcBall mCameraArcBall;
	ArcBall mModelArcBall;
	Quaternionf mCameraRot;
	float4x4 mWorld; // world matrix of model
};


} // Namespace RcEngine

#endif // CameraControler_h__