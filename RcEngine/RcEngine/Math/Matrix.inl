//----------------------------------------------------------------------------
#define MatrixItem(iRow, iCol) ( iRow*4 + iCol )

template<typename Real>
Matrix4<Real>::Matrix4()
{
	 MakeZero();
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4(const Real* rhs)
{
	memcpy(Elements, rhs, sizeof(Elements));
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4(const Matrix4<Real>& rhs)
{
	memcpy(Elements, rhs.Elements, sizeof(Elements));
}

//----------------------------------------------------------------------------
template<typename Real> 
template<typename T>
Matrix4<Real>::Matrix4(const Matrix4<T>& rhs)
{
	for(int32_t i = 0; i < 4; i++)
		for (int32_t j = 0; j < 4; j++)
		{
			Elements[MatrixItem(i, j)] = static_cast<Real>(rhs.Elements[MatrixItem(i, j)]);
		}

}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4( Real f11, Real f12, Real f13, Real f14,
					 Real f21, Real f22, Real f23, Real f24,
					 Real f31, Real f32, Real f33, Real f34,
					 Real f41, Real f42, Real f43, Real f44 )
{
	M11	= f11; M12 = f12; M13 = f13; M14 = f14; 
	M21	= f21; M22 = f22; M23 = f23; M24 = f24; 
	M31	= f31; M32 = f32; M33 = f33; M34 = f34; 
	M41	= f41; M42 = f42; M43 = f43; M44 = f44; 
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::MakeIdentity()
{
	Elements[ 0] = (Real)1.0;
	Elements[ 1] = (Real)0.0;
	Elements[ 2] = (Real)0.0;
	Elements[ 3] = (Real)0.0;
	Elements[ 4] = (Real)0.0;
	Elements[ 5] = (Real)1.0;
	Elements[ 6] = (Real)0.0;
	Elements[ 7] = (Real)0.0;
	Elements[ 8] = (Real)0.0;
	Elements[ 9] = (Real)0.0;
	Elements[10] = (Real)1.0;
	Elements[11] = (Real)0.0;
	Elements[12] = (Real)0.0;
	Elements[13] = (Real)0.0;
	Elements[14] = (Real)0.0;
	Elements[15] = (Real)1.0;
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::MakeZero()
{
	Elements[ 0] = (Real)0.0;
	Elements[ 1] = (Real)0.0;
	Elements[ 2] = (Real)0.0;
	Elements[ 3] = (Real)0.0;
	Elements[ 4] = (Real)0.0;
	Elements[ 5] = (Real)0.0;
	Elements[ 6] = (Real)0.0;
	Elements[ 7] = (Real)0.0;
	Elements[ 8] = (Real)0.0;
	Elements[ 9] = (Real)0.0;
	Elements[10] = (Real)0.0;
	Elements[11] = (Real)0.0;
	Elements[12] = (Real)0.0;
	Elements[13] = (Real)0.0;
	Elements[14] = (Real)0.0;
	Elements[15] = (Real)0.0;
}
//----------------------------------------------------------------------------
template<typename Real>
bool Matrix4<Real>::operator== (const Matrix4<Real>& rhs) const
{
	 return memcmp(Elements,rhs.Elements,16*sizeof(Real)) == 0;
}

//----------------------------------------------------------------------------
template<typename Real>
bool Matrix4<Real>::operator!= (const Matrix4<Real>& rhs) const
{
	 return memcmp(Elements,rhs.Elements,16*sizeof(Real)) != 0;
}
//----------------------------------------------------------------------------
template<typename Real>
const Real* Matrix4<Real>::operator()() const
{
	return Elements;
}	

//----------------------------------------------------------------------------
template<typename Real>
Real* Matrix4<Real>::operator()()
{
	return Elements;
}

//----------------------------------------------------------------------------
template<typename Real>
Real Matrix4<Real>::operator[]( int32_t index ) const
{
	return Elements[index];
}

//----------------------------------------------------------------------------
template<typename Real>
Real& Matrix4<Real>::operator[]( int32_t index )
{
	return Elements[index];
}

//----------------------------------------------------------------------------
template<typename Real>
Real Matrix4<Real>::operator()( int32_t iRow, int32_t iCol ) const
{
	return Elements[MatrixItem(iRow, iCol)];
}

//----------------------------------------------------------------------------
template<typename Real>
Real& Matrix4<Real>::operator()( int32_t iRow, int32_t iCol )
{
	return Elements[MatrixItem(iRow, iCol)];
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::SetRow( int32_t iRow, const Vector<Real, 4>& rhs )
{
	Elements[MatrixItem(iRow, 0)] = rhs[0];
	Elements[MatrixItem(iRow, 1)] = rhs[1];
	Elements[MatrixItem(iRow, 2)] = rhs[2];
	Elements[MatrixItem(iRow, 3)] = rhs[3];
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::GetRow( int32_t iRow ) const
{
	return Vector<Real, 4>(Elements[MatrixItem(iRow, 0)], Elements[MatrixItem(iRow, 1)],
				Elements[MatrixItem(iRow, 2)], Elements[MatrixItem(iRow, 3)]);
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::SetColumn( int32_t iCol, const Vector<Real, 4>& rhs )
{
	Elements[MatrixItem(0, iCol)] = rhs[0];
	Elements[MatrixItem(1, iCol)]= rhs[1];
	Elements[MatrixItem(2, iCol)] = rhs[2];
	Elements[MatrixItem(3, iCol)] = rhs[3];
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::GetColumn( int32_t iCol ) const
{
	return Vector<Real, 4>(Elements[MatrixItem(0, iCol)], Elements[MatrixItem(1, iCol)],
		Elements[MatrixItem(2, iCol)], Elements[MatrixItem(3, iCol)]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator=( const Matrix4<Real>& rhs )
{
	Elements[ 0] = rhs.Elements[ 0];
	Elements[ 1] = rhs.Elements[ 1];
	Elements[ 2] = rhs.Elements[ 2];
	Elements[ 3] = rhs.Elements[ 3];
	Elements[ 4] = rhs.Elements[ 4];
	Elements[ 5] = rhs.Elements[ 5];
	Elements[ 6] = rhs.Elements[ 6];
	Elements[ 7] = rhs.Elements[ 7];
	Elements[ 8] = rhs.Elements[ 8];
	Elements[ 9] = rhs.Elements[ 9];
	Elements[10] = rhs.Elements[10];
	Elements[11] = rhs.Elements[11];
	Elements[12] = rhs.Elements[12];
	Elements[13] = rhs.Elements[13];
	Elements[14] = rhs.Elements[14];
	Elements[15] = rhs.Elements[15];
	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator+( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0] + rhs.Elements[ 0],
		Elements[ 1] + rhs.Elements[ 1],
		Elements[ 2] + rhs.Elements[ 2],
		Elements[ 3] + rhs.Elements[ 3],
		Elements[ 4] + rhs.Elements[ 4],
		Elements[ 5] + rhs.Elements[ 5],
		Elements[ 6] + rhs.Elements[ 6],
		Elements[ 7] + rhs.Elements[ 7],
		Elements[ 8] + rhs.Elements[ 8],
		Elements[ 9] + rhs.Elements[ 9],
		Elements[10] + rhs.Elements[10],
		Elements[11] + rhs.Elements[11],
		Elements[12] + rhs.Elements[12],
		Elements[13] + rhs.Elements[13],
		Elements[14] + rhs.Elements[14],
		Elements[15] + rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator-( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0] - rhs.Elements[ 0],
		Elements[ 1] - rhs.Elements[ 1],
		Elements[ 2] - rhs.Elements[ 2],
		Elements[ 3] - rhs.Elements[ 3],
		Elements[ 4] - rhs.Elements[ 4],
		Elements[ 5] - rhs.Elements[ 5],
		Elements[ 6] - rhs.Elements[ 6],
		Elements[ 7] - rhs.Elements[ 7],
		Elements[ 8] - rhs.Elements[ 8],
		Elements[ 9] - rhs.Elements[ 9],
		Elements[10] - rhs.Elements[10],
		Elements[11] - rhs.Elements[11],
		Elements[12] - rhs.Elements[12],
		Elements[13] - rhs.Elements[13],
		Elements[14] - rhs.Elements[14],
		Elements[15] - rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator*( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0]*rhs.Elements[ 0] +
		Elements[ 1]*rhs.Elements[ 4] +
		Elements[ 2]*rhs.Elements[ 8] +
		Elements[ 3]*rhs.Elements[12],

		Elements[ 0]*rhs.Elements[ 1] +
		Elements[ 1]*rhs.Elements[ 5] +
		Elements[ 2]*rhs.Elements[ 9] +
		Elements[ 3]*rhs.Elements[13],

		Elements[ 0]*rhs.Elements[ 2] +
		Elements[ 1]*rhs.Elements[ 6] +
		Elements[ 2]*rhs.Elements[10] +
		Elements[ 3]*rhs.Elements[14],

		Elements[ 0]*rhs.Elements[ 3] +
		Elements[ 1]*rhs.Elements[ 7] +
		Elements[ 2]*rhs.Elements[11] +
		Elements[ 3]*rhs.Elements[15],

		Elements[ 4]*rhs.Elements[ 0] +
		Elements[ 5]*rhs.Elements[ 4] +
		Elements[ 6]*rhs.Elements[ 8] +
		Elements[ 7]*rhs.Elements[12],

		Elements[ 4]*rhs.Elements[ 1] +
		Elements[ 5]*rhs.Elements[ 5] +
		Elements[ 6]*rhs.Elements[ 9] +
		Elements[ 7]*rhs.Elements[13],

		Elements[ 4]*rhs.Elements[ 2] +
		Elements[ 5]*rhs.Elements[ 6] +
		Elements[ 6]*rhs.Elements[10] +
		Elements[ 7]*rhs.Elements[14],

		Elements[ 4]*rhs.Elements[ 3] +
		Elements[ 5]*rhs.Elements[ 7] +
		Elements[ 6]*rhs.Elements[11] +
		Elements[ 7]*rhs.Elements[15],

		Elements[ 8]*rhs.Elements[ 0] +
		Elements[ 9]*rhs.Elements[ 4] +
		Elements[10]*rhs.Elements[ 8] +
		Elements[11]*rhs.Elements[12],

		Elements[ 8]*rhs.Elements[ 1] +
		Elements[ 9]*rhs.Elements[ 5] +
		Elements[10]*rhs.Elements[ 9] +
		Elements[11]*rhs.Elements[13],

		Elements[ 8]*rhs.Elements[ 2] +
		Elements[ 9]*rhs.Elements[ 6] +
		Elements[10]*rhs.Elements[10] +
		Elements[11]*rhs.Elements[14],

		Elements[ 8]*rhs.Elements[ 3] +
		Elements[ 9]*rhs.Elements[ 7] +
		Elements[10]*rhs.Elements[11] +
		Elements[11]*rhs.Elements[15],

		Elements[12]*rhs.Elements[ 0] +
		Elements[13]*rhs.Elements[ 4] +
		Elements[14]*rhs.Elements[ 8] +
		Elements[15]*rhs.Elements[12],

		Elements[12]*rhs.Elements[ 1] +
		Elements[13]*rhs.Elements[ 5] +
		Elements[14]*rhs.Elements[ 9] +
		Elements[15]*rhs.Elements[13],

		Elements[12]*rhs.Elements[ 2] +
		Elements[13]*rhs.Elements[ 6] +
		Elements[14]*rhs.Elements[10] +
		Elements[15]*rhs.Elements[14],

		Elements[12]*rhs.Elements[ 3] +
		Elements[13]*rhs.Elements[ 7] +
		Elements[14]*rhs.Elements[11] +
		Elements[15]*rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator*( Real fScalar ) const
{
	return Matrix4<Real>(
		fScalar*Elements[ 0],
		fScalar*Elements[ 1],
		fScalar*Elements[ 2],
		fScalar*Elements[ 3],
		fScalar*Elements[ 4],
		fScalar*Elements[ 5],
		fScalar*Elements[ 6],
		fScalar*Elements[ 7],
		fScalar*Elements[ 8],
		fScalar*Elements[ 9],
		fScalar*Elements[10],
		fScalar*Elements[11],
		fScalar*Elements[12],
		fScalar*Elements[13],
		fScalar*Elements[14],
		fScalar*Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator/( Real fScalar ) const
{
	Real fInvScalar = ((Real)1.0)/fScalar;
	return Matrix4<Real>(
		fInvScalar*Elements[ 0],
		fInvScalar*Elements[ 1],
		fInvScalar*Elements[ 2],
		fInvScalar*Elements[ 3],
		fInvScalar*Elements[ 4],
		fInvScalar*Elements[ 5],
		fInvScalar*Elements[ 6],
		fInvScalar*Elements[ 7],
		fInvScalar*Elements[ 8],
		fInvScalar*Elements[ 9],
		fInvScalar*Elements[10],
		fInvScalar*Elements[11],
		fInvScalar*Elements[12],
		fInvScalar*Elements[13],
		fInvScalar*Elements[14],
		fInvScalar*Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator-() const
{
	return Matrix4<Real>(
		-Elements[ 0],
		-Elements[ 1],
		-Elements[ 2],
		-Elements[ 3],
		-Elements[ 4],
		-Elements[ 5],
		-Elements[ 6],
		-Elements[ 7],
		-Elements[ 8],
		-Elements[ 9],
		-Elements[10],
		-Elements[11],
		-Elements[12],
		-Elements[13],
		-Elements[14],
		-Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator+=( const Matrix4<Real>& rhs )
{
	Elements[ 0] += rhs.Elements[ 0];
	Elements[ 1] += rhs.Elements[ 1];
	Elements[ 2] += rhs.Elements[ 2];
	Elements[ 3] += rhs.Elements[ 3];
	Elements[ 4] += rhs.Elements[ 4];
	Elements[ 5] += rhs.Elements[ 5];
	Elements[ 6] += rhs.Elements[ 6];
	Elements[ 7] += rhs.Elements[ 7];
	Elements[ 8] += rhs.Elements[ 8];
	Elements[ 9] += rhs.Elements[ 9];
	Elements[10] += rhs.Elements[10];
	Elements[11] += rhs.Elements[11];
	Elements[12] += rhs.Elements[12];
	Elements[13] += rhs.Elements[13];
	Elements[14] += rhs.Elements[14];
	Elements[15] += rhs.Elements[15];
	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator-=( const Matrix4<Real>& rhs )
{
	Elements[ 0] -= rhs.Elements[ 0];
	Elements[ 1] -= rhs.Elements[ 1];
	Elements[ 2] -= rhs.Elements[ 2];
	Elements[ 3] -= rhs.Elements[ 3];
	Elements[ 4] -= rhs.Elements[ 4];
	Elements[ 5] -= rhs.Elements[ 5];
	Elements[ 6] -= rhs.Elements[ 6];
	Elements[ 7] -= rhs.Elements[ 7];
	Elements[ 8] -= rhs.Elements[ 8];
	Elements[ 9] -= rhs.Elements[ 9];
	Elements[10] -= rhs.Elements[10];
	Elements[11] -= rhs.Elements[11];
	Elements[12] -= rhs.Elements[12];
	Elements[13] -= rhs.Elements[13];
	Elements[14] -= rhs.Elements[14];
	Elements[15] -= rhs.Elements[15];
	return *this;
}


//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator*=(Real fScalar)
{
	Elements[ 0] *= fScalar;
	Elements[ 1] *= fScalar;
	Elements[ 2] *= fScalar;
	Elements[ 3] *= fScalar;
	Elements[ 4] *= fScalar;
	Elements[ 5] *= fScalar;
	Elements[ 6] *= fScalar;
	Elements[ 7] *= fScalar;
	Elements[ 8] *= fScalar;
	Elements[ 9] *= fScalar;
	Elements[10] *= fScalar;
	Elements[11] *= fScalar;
	Elements[12] *= fScalar;
	Elements[13] *= fScalar;
	Elements[14] *= fScalar;
	Elements[15] *= fScalar;
	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator/=(Real fScalar)
{
	Real fInvScalar = ((Real)1.0)/fScalar;
	Elements[ 0] *= fInvScalar;
	Elements[ 1] *= fInvScalar;
	Elements[ 2] *= fInvScalar;
	Elements[ 3] *= fInvScalar;
	Elements[ 4] *= fInvScalar;
	Elements[ 5] *= fInvScalar;
	Elements[ 6] *= fInvScalar;
	Elements[ 7] *= fInvScalar;
	Elements[ 8] *= fInvScalar;
	Elements[ 9] *= fInvScalar;
	Elements[10] *= fInvScalar;
	Elements[11] *= fInvScalar;
	Elements[12] *= fInvScalar;
	Elements[13] *= fInvScalar;
	Elements[14] *= fInvScalar;
	Elements[15] *= fInvScalar;

	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::operator*( const Vector<Real, 4>& rhs ) const
{
	return Vector4<Real>(
		Elements[ 0]*rhs[0] +
		Elements[ 1]*rhs[1] +
		Elements[ 2]*rhs[2] +
		Elements[ 3]*rhs[3],

		Elements[ 4]*rhs[0] +
		Elements[ 5]*rhs[1] +
		Elements[ 6]*rhs[2] +
		Elements[ 7]*rhs[3],

		Elements[ 8]*rhs[0] +
		Elements[ 9]*rhs[1] +
		Elements[10]*rhs[2] +
		Elements[11]*rhs[3],

		Elements[12]*rhs[0] +
		Elements[13]*rhs[1] +
		Elements[14]*rhs[2] +
		Elements[15]*rhs[3]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::Transpose() const
{
	return Matrix4<Real>(
		Elements[ 0],
		Elements[ 4],
		Elements[ 8],
		Elements[12],
		Elements[ 1],
		Elements[ 5],
		Elements[ 9],
		Elements[13],
		Elements[ 2],
		Elements[ 6],
		Elements[10],
		Elements[14],
		Elements[ 3],
		Elements[ 7],
		Elements[11],
		Elements[15]);

}

//----------------------------------------------------------------------------
template<typename Real>
Real Matrix4<Real>::Determinant() const
{
	Real fA0 = Elements[ 0]*Elements[ 5] - Elements[ 1]*Elements[ 4];
	Real fA1 = Elements[ 0]*Elements[ 6] - Elements[ 2]*Elements[ 4];
	Real fA2 = Elements[ 0]*Elements[ 7] - Elements[ 3]*Elements[ 4];
	Real fA3 = Elements[ 1]*Elements[ 6] - Elements[ 2]*Elements[ 5];
	Real fA4 = Elements[ 1]*Elements[ 7] - Elements[ 3]*Elements[ 5];
	Real fA5 = Elements[ 2]*Elements[ 7] - Elements[ 3]*Elements[ 6];
	Real fB0 = Elements[ 8]*Elements[13] - Elements[ 9]*Elements[12];
	Real fB1 = Elements[ 8]*Elements[14] - Elements[10]*Elements[12];
	Real fB2 = Elements[ 8]*Elements[15] - Elements[11]*Elements[12];
	Real fB3 = Elements[ 9]*Elements[14] - Elements[10]*Elements[13];
	Real fB4 = Elements[ 9]*Elements[15] - Elements[11]*Elements[13];
	Real fB5 = Elements[10]*Elements[15] - Elements[11]*Elements[14];
	Real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
	return fDet;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::Inverse() const
{
	Real v0 = M31 * M42 - M32 * M41;
	Real v1 = M31 * M43 - M33 * M41;
	Real v2 = M31 * M44 - M34 * M41;
	Real v3 = M32 * M43 - M33 * M42;
	Real v4 = M32 * M44 - M34 * M42;
	Real v5 = M33 * M44 - M34 * M43;

	Real i11 = (v5 * M22 - v4 * M23 + v3 * M24);
	Real i21 = -(v5 * M21 - v2 * M23 + v1 * M24);
	Real i31 = (v4 * M21 - v2 * M22 + v0 * M24);
	Real i41 = -(v3 * M21 - v1 * M22 + v0 * M23);

	Real invDet = 1.0f / (i11 * M11 + i21 * M12 + i31 * M13 + i41 * M14);

	i11 *= invDet;
	i21 *= invDet;
	i31 *= invDet;
	i41 *= invDet;

	Real i12 = -(v5 * M12 - v4 * M13 + v3 * M14) * invDet;
	Real i22 = (v5 * M11 - v2 * M13 + v1 * M14) * invDet;
	Real i32 = -(v4 * M11 - v2 * M12 + v0 * M14) * invDet;
	Real i42 = (v3 * M11 - v1 * M12 + v0 * M13) * invDet;

	v0 = M21 * M42 - M22 * M41;
	v1 = M21 * M43 - M23 * M41;
	v2 = M21 * M44 - M24 * M41;
	v3 = M22 * M43 - M23 * M42;
	v4 = M22 * M44 - M24 * M42;
	v5 = M23 * M44 - M24 * M43;

	Real i13 = (v5 * M12 - v4 * M13 + v3 * M14) * invDet;
	Real i23 = -(v5 * M11 - v2 * M13 + v1 * M14) * invDet;
	Real i33 = (v4 * M11 - v2 * M12 + v0 * M14) * invDet;
	Real i43 = -(v3 * M11 - v1 * M12 + v0 * M13) * invDet;

	v0 = M32 * M21 - M31 * M22;
	v1 = M33 * M21 - M31 * M23;
	v2 = M34 * M21 - M31 * M24;
	v3 = M33 * M22 - M32 * M23;
	v4 = M34 * M22 - M32 * M24;
	v5 = M34 * M23 - M33 * M24;

	Real i14 = -(v5 * M12 - v4 * M13 + v3 * M14) * invDet;
	Real i24 = (v5 * M11 - v2 * M13 + v1 * M14) * invDet;
	Real i34 = -(v4 * M11 - v2 * M12 + v0 * M14) * invDet;
	Real i44 = (v3 * M11 - v1 * M12 + v0 * M13) * invDet;

	return Matrix4<Real>(
		i11, i12, i13, i14,
		i21, i22, i23, i24,
		i31, i32, i33, i34,
		i41, i42, i43, i44);
}

//----------------------------------------------------------------------------
template<typename Real>
const Matrix4<Real>& Matrix4<Real>::Identity()
{
	static Matrix4<Real> out( Real(1), Real(0), Real(0), Real(0),
							  Real(0), Real(1), Real(0), Real(0),
							  Real(0), Real(0), Real(1), Real(0),
							  Real(0), Real(0), Real(0), Real(1));
	return out;
}

//----------------------------------------------------------------------------
template<typename Real>
inline Vector<Real, 4> operator* (const Vector<Real, 4>& lhs, const Matrix4<Real>& rhs)
{
	return Vector<Real, 4>(
		lhs[0]*rhs.M11+lhs[1]*rhs.M21+lhs[2]*rhs.M31+lhs[3]*rhs.M41,
		lhs[0]*rhs.M12+lhs[1]*rhs.M22+lhs[2]*rhs.M32+lhs[3]*rhs.M42,
		lhs[0]*rhs.M13+lhs[1]*rhs.M23+lhs[2]*rhs.M33+lhs[3]*rhs.M43,
		lhs[0]*rhs.M14+lhs[1]*rhs.M24+lhs[2]*rhs.M34+lhs[3]*rhs.M44);
}

//----------------------------------------------------------------------------
template <typename Real>
inline Matrix4<Real> operator* (Real scalar, const Matrix4<Real>& mat)
{
	return mat * scalar;
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
	MatrixInverse(const Matrix4<Real>& mat)
{
	Real v0 = mat.M31 * mat.M42 - mat.M32 * mat.M41;
	Real v1 = mat.M31 * mat.M43 - mat.M33 * mat.M41;
	Real v2 = mat.M31 * mat.M44 - mat.M34 * mat.M41;
	Real v3 = mat.M32 * mat.M43 - mat.M33 * mat.M42;
	Real v4 = mat.M32 * mat.M44 - mat.M34 * mat.M42;
	Real v5 = mat.M33 * mat.M44 - mat.M34 * mat.M43;

	Real i11 = (v5 * mat.M22 - v4 * mat.M23 + v3 * mat.M24);
	Real i21 = -(v5 * mat.M21 - v2 * mat.M23 + v1 * mat.M24);
	Real i31 = (v4 * mat.M21 - v2 * mat.M22 + v0 * mat.M24);
	Real i41 = -(v3 * mat.M21 - v1 * mat.M22 + v0 * mat.M23);

	Real invDet = 1.0f / (i11 * mat.M11 + i21 * mat.M12 + i31 * mat.M13 + i41 * mat.M14);

	i11 *= invDet;
	i21 *= invDet;
	i31 *= invDet;
	i41 *= invDet;

	Real i12 = -(v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i22 = (v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i32 = -(v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i42 = (v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	v0 = mat.M21 * mat.M42 - mat.M22 * mat.M41;
	v1 = mat.M21 * mat.M43 - mat.M23 * mat.M41;
	v2 = mat.M21 * mat.M44 - mat.M24 * mat.M41;
	v3 = mat.M22 * mat.M43 - mat.M23 * mat.M42;
	v4 = mat.M22 * mat.M44 - mat.M24 * mat.M42;
	v5 = mat.M23 * mat.M44 - mat.M24 * mat.M43;

	Real i13 = (v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i23 = -(v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i33 = (v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i43 = -(v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	v0 = mat.M32 * mat.M21 - mat.M31 * mat.M22;
	v1 = mat.M33 * mat.M21 - mat.M31 * mat.M23;
	v2 = mat.M34 * mat.M21 - mat.M31 * mat.M24;
	v3 = mat.M33 * mat.M22 - mat.M32 * mat.M23;
	v4 = mat.M34 * mat.M22 - mat.M32 * mat.M24;
	v5 = mat.M34 * mat.M23 - mat.M33 * mat.M24;

	Real i14 = -(v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i24 = (v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i34 = -(v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i44 = (v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	return Matrix4<Real>(
		i11, i12, i13, i14,
		i21, i22, i23, i24,
		i31, i32, i33, i34,
		i41, i42, i43, i44);
}