#ifndef Frustum_h__
#define Frustum_h__

#include <Math/Plane.h>
#include <Math/Matrix.h>
#include <Math/BoundingSphere.h>
#include <Math/BoundingBox.h>

namespace RcEngine {

enum FrustumPlaneSide
{
	FPS_Left = 0,
	FPS_Right,
	FPS_Top,
	FPS_Bottom,
	FPS_Near,
	FPS_Far,
};

enum FrustumCorner
{
	FC_Near_Top_Left = 0,
	FC_Near_Top_Right,
	FC_Near_Bottom_Left,
	FC_Near_Bottom_Right,
	FC_Far_Top_Left,
	FC_Far_Top_Right,
	FC_Far_Bottom_Left,
	FC_Far_Bottom_Right,
};

template<typename Real>
class Frustum
{
public:
	typedef Real value_type;

public:
	Frustum();

	/**
	 * Update the view frustum planes using the current view/projection matrices.
	 */
	void Update(const Matrix4<Real>& viewProjection);

	ContainmentType Contain(const BoundingSphere<Real>& sphere) const; 
	ContainmentType Contain(const BoundingBox<Real>& box) const; 
	/*ContainmentType Contain(const Frustum<Real>& frustum) const; */

public:
	Plane<Real> Planes[6];
	Vector<Real,3> Corner[8];  // Corner points
};



typedef Frustum<float> Frustumf;

#include <Math/Frustum.inl>

}


#endif // Frustum_h__