#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine {

template<typename Real>
class BoundingSphere;

template <typename Real>
class BoundingBox
{
public:
	typedef Real value_type;

public:

	BoundingBox() : Min(FLT_MAX, FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX, -FLT_MAX) { }

	BoundingBox(const Vector<Real,3>& min, const Vector<Real,3>& max)
		: Min(min), Max(max) { }

	BoundingBox(const BoundingBox<Real>& rhs)
		: Min(rhs.Min), Max(rhs.Max) { }

	/**
	 * Reset the bounding box to undefined.
	 */
	inline void SetNull()	
	{ 
		Min = Vector<Real,3>(FLT_MAX, FLT_MAX, FLT_MAX); 
		Max = Vector<Real,3>(-FLT_MAX, -FLT_MAX, -FLT_MAX);  
	}

	inline bool IsValid() const {  return Max.X()>=Min.X() && Max.Y()>=Min.Y() && Max.Z()>=Min.Z(); }

	inline Vector<Real,3> Center() const { return (Min + Max) * Real(0.5); }
	

	/** 
	 * Returns a specific corner of the bounding box.
     * pos specifies the corner as a number between 0 and 7.
     * Each bit selects an axis, X, Y, or Z from least- to
     * most-significant. Unset bits select the minimum value
     * for that axis, and set bits select the maximum.
	 */
	Vector<Real, 3> GetCorner(uint8_t pos) const;
	void GetCorners(Vector<Real, 3> corners[8]) const; 

	void Merge(const Vector<Real,3>& point);
	void Merge(const BoundingBox<Real>& box);
	void Merge(const BoundingSphere<Real>& sphere);

	ContainmentType Contains( const BoundingBox<Real>& box ) const;
	ContainmentType Contains( const BoundingSphere<Real>& sphere ) const;
	ContainmentType Contains( const Vector<Real,3>& point ) const;

	bool Intersects(const BoundingSphere<Real>& sphere) const;
	bool Intersects(const BoundingBox<Real>& box) const;



public:
	Vector<Real,3> Max;
	Vector<Real,3> Min;
};

typedef BoundingBox<float> BoundingBoxf;
typedef BoundingBox<double> BoundingBoxd;

/**
 * Return a new bounding box merged  from two given bounding box 
 */
//template<typename Real>
//BoundingBox<Real> Merge( const BoundingBox<Real>& box1, const BoundingBox<Real>& box2 );

#include "Math/BoundingBox.inl"


} // Namespace RcEngine


#endif // BoundingBox_h__
