#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine {

template <typename Real>
class BoundingBox;

template<typename Real>
class BoundingSphere
{
public:
	typedef Real value_type;

public:
	/**
	 * Construct a default bounding sphere with radius to -1.0, representing an invalid/unset bounding sphere.
	 */ 
	BoundingSphere(): Center(0.0, 0.0, 0.0), Radius(-1.0) { }

	BoundingSphere(const Vector<Real, 3>& center, Real radius)
		: Center(center), Radius(radius) {}

	BoundingSphere(const BoundingSphere<Real>& rhs)
		: Center(rhs.Center), Radius(rhs.Radius) {}

	//  Reset to undefined.
	inline void SetNull()			{ Center = Vector<Real, 3>(0, 0, 0); Radius = -1.0; }
	inline bool IsValid() const		{ return Radius >= 0.0; }

	void Merge(const Vector<Real, 3>& point);
	void Merge(const BoundingSphere<Real>& sphere);
	void Merge(const BoundingBox<Real>& box);

	ContainmentType Contains(const BoundingSphere<Real>& sphere) const;
	ContainmentType Contains(const BoundingBox<Real>& box) const;
	ContainmentType Contains(const Vector<Real,3>& point) const;

	bool Intersects(const BoundingSphere<Real>& sphere) const;
	bool Intersects(const BoundingBox<Real>& box) const;

public:
	Vector<Real, 3> Center;
	Real Radius;
};

typedef BoundingSphere<float> BoundingSpheref;
typedef BoundingSphere<double> BoundingSphered;

/**
 * Return a new bounding sphere merged  from two given bounding sphere 
 */
//template<typename Real>
//BoundingSphere<Real> Merge(const BoundingSphere<Real>& sphere1,const BoundingSphere<Real>& sphere2);

#include "Math/BoundingSphere.inl"


} // Namespace RcEngine

#endif // BoundingSphere_h__
