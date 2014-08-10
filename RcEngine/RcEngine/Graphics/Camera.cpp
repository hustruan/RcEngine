#include <Graphics/Camera.h>
#include <Core/Environment.h>
#include <Graphics/RenderDevice.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Camera::Camera(void)
	: mFrustumDirty(true)
{
	CreateLookAt(float3(0, 0, 0), float3(0, 0, 1), float3(0, 1, 0));
	CreatePerspectiveFov(Mathf::HALF_PI, 1.0f, 1.0f, 1000.0f);
}


Camera::~Camera(void)
{
}

void Camera::CreateLookAt( const float3& eyePos, const float3& lookat, const float3& upVec /*= float3(0, 1, 0)*/ )
{
	mPosition = eyePos;
	mLookAt	= lookat;
	mUp = upVec;

	mForward = Normalize(mLookAt - mPosition);
	mViewMatrix = CreateLookAtMatrixLH(mPosition, mLookAt, mUp);
	mEngineViewProjMatrix = mViewMatrix * mEngineProjMatrix;

	mUp = float3(mViewMatrix.M12, mViewMatrix.M22, mViewMatrix.M32);
	mFrustumDirty = true;
}

void Camera::CreatePerspectiveFov( float fov, float aspect, float nearPlane, float farPlane )
{
	mFieldOfView = fov;
	mAspect = aspect;
	mNearPlane = nearPlane;
	mFarPlane = farPlane;

	mProjMatrix = CreatePerspectiveFovLH(mFieldOfView, mAspect, mNearPlane, mFarPlane);

	mEngineProjMatrix = mProjMatrix;
	Environment::GetSingleton().GetRenderDevice()->AdjustProjectionMatrix(mEngineProjMatrix);
	mEngineViewProjMatrix = mViewMatrix * mEngineProjMatrix;

	mFrustumDirty = true;
}

void Camera::CreateOrthoOffCenter( float left, float right, float bottom, float top, float nearPlane, float farPlane )
{
	mNearPlane = nearPlane;
	mFarPlane = farPlane;

	mProjMatrix = CreateOrthoOffCenterLH(left, right, bottom, top, nearPlane, farPlane);

	mEngineProjMatrix = mProjMatrix;
	Environment::GetSingleton().GetRenderDevice()->AdjustProjectionMatrix(mEngineProjMatrix);
	mEngineViewProjMatrix = mViewMatrix * mEngineProjMatrix;
	
	mFrustumDirty = true;
}

const Frustumf& Camera::GetFrustum() const
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}
	return mFrustum;
}

bool Camera::Visible( const BoundingSpheref& sphere ) const
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}

	if (mFrustum.Contain(sphere) == CT_Disjoint)
		return false;

	return true;
}

bool Camera::Visible( const BoundingBoxf& box ) const
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}

	if (mFrustum.Contain(box) == CT_Disjoint)
		return false;

	return true;
}



} // Namespace RcEngine