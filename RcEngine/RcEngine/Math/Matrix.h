#ifndef _Matrix__H
#define _Matrix__H

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine{

template<typename Real>
class Matrix4
{
public:
	typedef Real value_type;

public:
	Matrix4();
	Matrix4(const Real* rhs);
	Matrix4(const Matrix4& rhs);
	
	Matrix4(Real f11, Real f12, Real f13, Real f14,
		Real f21, Real f22, Real f23, Real f24,
		Real f31, Real f32, Real f33, Real f34,
		Real f41, Real f42, Real f43, Real f44);

	template<typename T>
	Matrix4(const Matrix4<T>& rhs);

	void MakeIdentity();
	void MakeZero();

	// member access
	inline const Real* operator() () const;
	inline Real* operator() () ;

	inline Real operator[] (int32_t index) const;
	inline Real& operator[] (int32_t index);

	inline Real operator() (int32_t iRow, int32_t iCol) const;
	inline Real& operator() (int32_t iRow, int32_t iCol);

	void SetRow (int32_t iRow, const Vector<Real, 4>& rhs);
	Vector<Real, 4> GetRow (int32_t iRow) const;

	void SetColumn (int32_t iCol, const Vector<Real, 4>& rhs);
	Vector<Real, 4> GetColumn (int32_t iCol) const;
		
	// assignment
	inline Matrix4& operator= (const Matrix4& rhs);

	// comparison
	bool operator== (const Matrix4& rhs) const;
	bool operator!= (const Matrix4& rhs) const;
		
	// arithmetic operations
	inline Matrix4 operator+ (const Matrix4& rhs) const;
	inline Matrix4 operator- (const Matrix4& rhs) const;
	inline Matrix4 operator* (const Matrix4& rhs) const;
	inline Matrix4 operator* (Real fScalar) const;
	inline Matrix4 operator/ (Real fScalar) const;
	inline Matrix4 operator- () const;

	// arithmetic updates
	inline Matrix4& operator+= (const Matrix4& rhs);
	inline Matrix4& operator-= (const Matrix4& rhs);
	inline Matrix4& operator*= (Real fScalar);
	inline Matrix4& operator/= (Real fScalar);

	// matrix times vector
	inline Vector<Real, 4> operator* (const Vector<Real, 4>& rhs) const;   // dst = M * src

	Real Determinant () const;
	Matrix4 Transpose() const;
	Matrix4 Inverse() const;

	// return an identity matrix
	inline static const Matrix4& Identity();
	
public:

	union
	{
		struct{ Real Elements[16]; };
		struct
		{
			Real M11, M12, M13, M14;
			Real M21, M22, M23, M24;
			Real M31, M32, M33, M34;
			Real M41, M42, M43, M44;
		};
	};		
};

template <typename Real>
inline Matrix4<Real> operator* (Real scalar, const Matrix4<Real>& mat);

// M * V^T
template<typename Real>
inline Vector<Real, 4> operator* (const Vector<Real, 4>& lhs, const Matrix4<Real>& rhs);

// Compute inverse matrix
template<typename Real>
Matrix4<Real> MatrixInverse(const Matrix4<Real>& mat);

#include <Math/Matrix.inl>

typedef Matrix4<float> float4x4;


} // Namespace RcEngine

#endif // _Matrix__H