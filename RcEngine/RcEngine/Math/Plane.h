#pragma once
#ifndef _Plane__H
#define _Plane__H

//  [8/24/2011 hustruan]
#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine{

enum PlaneIntersectionType
{
	PIT_Front,
	PIT_Back,
	PIT_Intersecting
};

template<typename Real>
class BoundingSphere;

template <typename Real>
class BoundingBox;

/**
 * Plane: N*P+D = 0
 */
template< typename Real >
class Plane
{
public:
	typedef Real value_type;

public:
	Plane();
	Plane(const Vector<Real, 3>& nroaml, Real distance);
	Plane(const Vector<Real, 3>& nroaml, const Vector<Real, 3>& point);	
	Plane(const Vector<Real, 3>& point1, const Vector<Real, 3>& point2, const Vector<Real, 3>& point3);
	Plane(const Plane& rhs);

	inline void Flip() { Normal = -Normal; Distance = -Distance; }
	void Normalize();
	Real DotCoordinate(const Vector<Real, 3>& value) const;
	Real DotNormal(const Vector<Real, 3>& value) const;

	PlaneIntersectionType Intersects(const BoundingSphere<Real>& sphere) const;
	PlaneIntersectionType Intersects(const BoundingBox<Real>& box) const;

public:
	Vector<Real, 3> Normal;
	Real Distance;
};



#include <Math/Plane.inl>

typedef Plane<float> Planef;


} // Namespace RcEngine



#endif