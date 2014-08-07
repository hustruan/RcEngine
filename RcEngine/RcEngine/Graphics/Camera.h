#ifndef Camera_h__
#define Camera_h__

#include <Core/Prerequisites.h>
#include <Math/Frustum.h>

namespace RcEngine {

class _ApiExport Camera
{
public:
	Camera(void);
	~Camera(void);

	const float3& GetPosition() const				{ return mPosition; }
	const float3& GetLookAt() const					{ return mLookAt; }
	const float3& GetUp() const						{ return mUp; }
	const float3& GetForward() const				{ return mForward; }
	float3 GetRight() const							{ return Normalize(Cross(mUp, mForward)); }
	float  GetFov() const							{ return mFieldOfView; }
	float  GetAspect() const						{ return mAspect; }
	float  GetNearPlane() const						{ return mNearPlane; }
	float  GetFarPlane() const						{ return mFarPlane; }

	const float4x4& GetViewMatrix() const			{ return mViewMatrix; }
	const float4x4& GetProjMatrix() const			{ return mProjMatrix; }

	const Frustumf& GetFrustum() const;

	void CreateLookAt(const float3& eyePos, const float3& lookat, const float3& upVec = float3(0, 1, 0));
	void CreatePerspectiveFov(float fov, float aspect, float nearPlane, float farPlane);
	void CreateOrthoOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane);

	bool Visible( const BoundingSpheref& sphere ) const;
	bool Visible( const BoundingBoxf& box ) const;

public_internal:
	const float4x4& GetEngineProjMatrix() const			{ return mEngineProjMatrix; }
	const float4x4& GetEngineViewProjMatrix() const     { return mEngineViewProjMatrix; }

private:
	float3 mPosition;
	float3 mLookAt;
	float3 mUp;
	float3 mForward;

	float mFieldOfView;
	float mAspect;
	float mNearPlane;
	float mFarPlane;

	float4x4 mViewMatrix;
	float4x4 mProjMatrix;
	
	/**
	 * Note:
	 * OpenGL viewport transform map z [-1, 1] to [0, 1].
	 * Engine all DirectX coordinate system, so in order to make OpenGL work with DirectX's rule,
	 * We must adapt projection matrix if use OpenGL as renderer. Scale and offset, Z' = 2*Z -1.
	 *
	 * Never use EngineProjMatrix, it's only used in Vertex Shader to transform position.
	 */
	  
	float4x4 mEngineProjMatrix;
	float4x4 mEngineViewProjMatrix;

	mutable Frustumf mFrustum;
	mutable bool mFrustumDirty;
};

} // Namespace RcEngine

#endif // Camera_h__
