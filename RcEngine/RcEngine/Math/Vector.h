#pragma once
#ifndef _Vector__H
#define _Vector__H

#include <cstdint>
#include <math.h>
#include <iostream>

//  [8/22/2011 hustruan]
namespace RcEngine {


template< typename Real, int32_t Size >
class Vector
{
public:
	typedef Real value_type;

public:
	Vector();

	Vector(Real fX, Real fY);
	Vector(Real fX, Real fY, Real fZ);
	Vector(Real fX, Real fY, Real fZ, Real fW);

	explicit Vector(const Real* fTuple);
	explicit Vector(Real value);
		
	template<typename T>
	Vector(const Vector<T, Size>& rhs);

	// assignment
	inline Vector& operator= (const Vector& rhs);

	// coordinate access
	inline const Real* operator() (void)const;
	inline Real* operator() (void) ;
	inline Real operator[] (size_t i) const;
	inline Real& operator[] (size_t i);
	inline Real X () const;
	inline Real& X ();
	inline Real Y () const;
	inline Real& Y ();
	inline Real Z () const;
	inline Real& Z ();
	inline Real W () const;
	inline Real& W ();

	// arithmetic operations
	inline Vector operator+ (const Vector<Real, Size>& rhs) const;
	inline Vector operator- (const Vector<Real, Size>& rhs) const;
	inline Real   operator* (const Vector<Real, Size>& rhs) const;
	inline Vector operator* (Real fScalar) const;
	inline Vector operator/ (Real fScalar) const;
	inline Vector operator- () const;

	// arithmetic updates
	inline Vector& operator+= (const Vector& rhs);
	inline Vector& operator-= (const Vector& rhs);
	inline Vector& operator*= (Real fScalar);
	inline Vector& operator/= (Real fScalar);

	// comparison
	bool operator== (const Vector& rhs) const;
	bool operator!= (const Vector& rhs) const;

	// vector operations, prefer to use non menber funtion version
	inline Real Length () const;
	inline Real LengthSquared () const;

	// Normalize
	inline void Normalize ();

	// Zero Vector
	inline static const Vector& Zero();

private:
	Real mTuple[Size];
};

// debugging output
template <class Real, int32_t Size>
std::ostream& operator<< (std::ostream& rkOStr, const Vector<Real, Size>& rhs);

template <class Real, int32_t Size>
inline Vector<Real, Size> operator* (Real fScalar, const Vector<Real, Size>& rhs);

template< typename Real, int32_t Size >
inline Real Length( const Vector<Real, Size>& vec );

template< typename Real, int32_t Size >
inline Real LengthSquared( const Vector<Real, Size>& vec );

template< typename Real, int32_t Size >
inline Real Dot( const Vector<Real, Size>& lfs, const Vector<Real, Size>& rhs );

template< typename Real, int32_t Size >
inline Vector<Real, Size> Normalize(const Vector<Real, Size>& vec);

template< typename Real, int32_t Size >
inline Vector<Real, Size> Clamp( const Vector<Real, Size>& value, const Vector<Real, Size>& min,  const Vector<Real, Size>& max );

template<typename Real>
inline Vector<Real, 3> Cross( const Vector<Real, 3>& vec1, const Vector<Real, 3>& vec2 );

#include <Math/Vector.inl>

typedef Vector<float,2>  float2;
typedef Vector<float,3>  float3;
typedef Vector<float,4>  float4;

typedef Vector<int32_t,2> int2;
typedef Vector<int32_t,3> int3;
typedef Vector<int32_t,4> int4;

typedef Vector<uint32_t,2> uint2;
typedef Vector<uint32_t,3> uint3;
typedef Vector<uint32_t,4> uint4;

} // Namespace RcEngine
#endif