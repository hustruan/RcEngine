#ifndef Math_h__
#define Math_h__

#include <cfloat>
#include <math.h>

namespace RcEngine {

enum ContainmentType 
{
	CT_Disjoint,
	CT_Contains,
	CT_Intersects
};

template <typename Real>
class Math
{
public:
	// Common constants.
	static const Real EPSILON;
	static const Real ZERO_TOLERANCE;
	static const Real MAX_REAL;
	static const Real PI;
	static const Real TWO_PI;
	static const Real HALF_PI;
	static const Real INV_PI;
	static const Real INV_TWO_PI;
	static const Real DEG_TO_RAD;
	static const Real RAD_TO_DEG;
	static const Real LN_2;
	static const Real LN_10;
	static const Real INV_LN_2;
	static const Real INV_LN_10;
	static const Real SQRT_2;
	static const Real INV_SQRT_2;
	static const Real SQRT_3;
	static const Real INV_SQRT_3;

	static Real ToDegree(Real radian)	{ return radian * RAD_TO_DEG;}
	static Real ToRadian(Real degree)	{ return degree * DEG_TO_RAD; }
};

template <typename Real>
inline Real Clamp( Real val, Real min, Real max )
{
	if (val < min)	return min;
	if (val > max)  return max;
	return val;
}

template <typename T>
inline T Lerp(const T& a, const T& b, float t)
{
	return a + (b - a) * t;
}

typedef Math<float> Mathf;

//------------------------------------------------------------------------
template<> const float Math<float>::EPSILON = FLT_EPSILON;
template<> const float Math<float>::ZERO_TOLERANCE = 1e-06f;
template<> const float Math<float>::MAX_REAL = FLT_MAX;
template<> const float Math<float>::PI = (float)(4.0*atan(1.0));
template<> const float Math<float>::TWO_PI = 2.0f*Math<float>::PI;
template<> const float Math<float>::HALF_PI = 0.5f*Math<float>::PI;
template<> const float Math<float>::INV_PI = 1.0f/Math<float>::PI;
template<> const float Math<float>::INV_TWO_PI = 1.0f/Math<float>::TWO_PI;
template<> const float Math<float>::DEG_TO_RAD = Math<float>::PI/180.0f;
template<> const float Math<float>::RAD_TO_DEG = 180.0f/Math<float>::PI;


template<> const double Math<double>::EPSILON = DBL_EPSILON;
template<> const double Math<double>::ZERO_TOLERANCE = 1e-08;
template<> const double Math<double>::MAX_REAL = DBL_MAX;
template<> const double Math<double>::PI = 4.0*atan(1.0);
template<> const double Math<double>::TWO_PI = 2.0*Math<double>::PI;
template<> const double Math<double>::HALF_PI = 0.5*Math<double>::PI;
template<> const double Math<double>::INV_PI = 1.0/Math<double>::PI;
template<> const double Math<double>::INV_TWO_PI = 1.0/Math<double>::TWO_PI;
template<> const double Math<double>::DEG_TO_RAD = Math<double>::PI/180.0;
template<> const double Math<double>::RAD_TO_DEG = 180.0/Math<double>::PI;

}


#endif // Math_h__