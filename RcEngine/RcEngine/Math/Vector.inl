//  [8/22/2011 hustruan]

//----------------------------------------------------------------------------
template <class Real, int32_t Size>
Vector<Real, Size>::Vector()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real, int32_t Size>
Vector<Real, Size>::Vector(Real fX, Real fY)
{
	static_assert(Size == 2, "Vector dimension error.");

	mTuple[0] = fX;
	mTuple[1] = fY;
}

//----------------------------------------------------------------------------
template <class Real, int32_t Size>
Vector<Real, Size>::Vector(Real fX, Real fY, Real fZ)
{
	static_assert(Size == 3, "Vector dimension error.");

	mTuple[0] = fX;
	mTuple[1] = fY;
	mTuple[2] = fZ;
}

//----------------------------------------------------------------------------
template <class Real, int32_t Size>
Vector<Real, Size>::Vector(Real fX, Real fY, Real fZ, Real fW)
{
	static_assert(Size == 4, "Vector dimension error.");

	mTuple[0] = fX;
	mTuple[1] = fY;
	mTuple[2] = fZ;
	mTuple[3] = fW;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>::Vector( const Real* fTuple )
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i] = fTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>::Vector( Real value )
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i] = value;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size > template<typename T>
Vector<Real, Size>::Vector( const Vector<T, Size>& rhs )
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i] = rhs[i];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>& Vector<Real, Size>::operator=( const Vector& rhs )
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i] = rhs[i];
	return *this;
}


//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
const Real* Vector<Real, Size>::operator()() const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real* Vector<Real, Size>::operator()()
{
	return mTuple;
}


//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real Vector<Real, Size>::operator[]( size_t i ) const
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real& Vector<Real, Size>::operator[]( size_t i )
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real Vector<Real, Size>::X() const 
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real& Vector<Real, Size>::X()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real Vector<Real, Size>::Y() const 
{
	static_assert(Size >= 2, "Vector dimension error.");
	return mTuple[1];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real& Vector<Real, Size>::Y()
{
	static_assert(Size >= 2, "Vector dimension error.");
	return mTuple[1];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real Vector<Real, Size>::Z() const 
{
	static_assert(Size >= 3, "Vector dimension error.");
	return mTuple[2];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real& Vector<Real, Size>::Z()
{
	static_assert(Size >= 3, "Vector dimension error.");
	return mTuple[2];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real Vector<Real, Size>::W() const 
{
	static_assert(Size >= 4, "Vector dimension error.");
	return mTuple[3];
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Real& Vector<Real, Size>::W()
{
	static_assert(Size >= 4, "Vector dimension error.");
	return mTuple[3];
}

//----------------------------------------------------------------------------
template<typename Real, int32_t Size>
Vector<Real, Size> Vector<Real, Size>::operator+( const Vector<Real, Size>& rhs ) const
{
	Vector<Real, Size> result;
	for(int32_t i = 0; i < Size; i++)
		result[i] = rhs[i] + mTuple[i];

	return result;
}

//----------------------------------------------------------------------------

template<typename Real, int32_t Size>
Vector<Real, Size> Vector<Real, Size>::operator-( const Vector<Real, Size>& rhs ) const
{
	Vector<Real, Size> result;
	for(int32_t i = 0; i < Size; i++)
		result[i] = mTuple[i]- rhs[i];

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int32_t Size>
Real Vector<Real, Size>::operator*( const Vector<Real, Size>& rhs) const
{
	Real result = (Real)0;
	for(int32_t i = 0; i < Size; i++)
		result +=  mTuple[i] * rhs[i];

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int32_t Size>
Vector<Real, Size> Vector<Real, Size>::operator*( Real fScalar ) const
{
	Vector<Real, Size> result;
	for(int32_t i = 0; i < Size; i++)
		result[i] =  mTuple[i] * fScalar;

	return result;
}

//----------------------------------------------------------------------------

template<typename Real, int32_t Size>
Vector<Real, Size> Vector<Real, Size>::operator/( Real fScalar ) const
{
	Vector<Real, Size> result;

	for(int32_t i = 0; i < Size; i++)
		result[i] = mTuple[i] / fScalar;

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int32_t Size>
Vector<Real, Size> Vector<Real, Size>::operator-() const
{
	Vector<Real, Size> result;
	for(int32_t i = 0; i < Size; i++)
		result[i] =  -mTuple[i];

	return result;
}

//----------------------------------------------------------------------------

template< typename Real, int32_t Size >
Vector<Real, Size>& Vector<Real, Size>::operator+=( const Vector<Real, Size>& rhs )
{
	for(int32_t i = 0; i < Size; i++)
		 mTuple[i]  += rhs[i];
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>& Vector<Real, Size>::operator-=( const Vector<Real, Size>& rhs )
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i]  -= rhs[i];
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>& Vector<Real, Size>::operator*=( Real fScalar)
{
	for(int32_t i = 0; i < Size; i++)
		mTuple[i]  *= fScalar;
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
Vector<Real, Size>& Vector<Real, Size>::operator/=( Real fScalar)
{
	Real fInvScalar = ((Real)1.0)/fScalar;

	for(int32_t i = 0; i < Size; i++)
		mTuple[i]  *= fInvScalar;

	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
bool Vector<Real, Size>::operator==( const Vector<Real, Size>& rhs ) const
{
	 return memcmp(mTuple,rhs.mTuple,Size*sizeof(Real)) == 0;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
bool Vector<Real, Size>::operator!=( const Vector<Real, Size>& rhs ) const
{
	 return memcmp(mTuple,rhs.mTuple,Size*sizeof(Real)) != 0;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline Real Vector<Real, Size>::Length( ) const
{
	Real result = Real(0);
	for(int32_t i = 0; i < Size; i++)
		result +=  mTuple[i] * mTuple[i] ;
	return sqrt(result);
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline Real Vector<Real, Size>::LengthSquared( ) const
{
	Real result = Real(0);
	for(int32_t i = 0; i < Size; i++)
		result +=  mTuple[i] * mTuple[i] ;
	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
void Vector<Real, Size>::Normalize()
{
	Real fLength = LengthSquared();
	if(fLength > (Real)0.0)
	{
		Real fInvScalar = ((Real)1.0)/ (sqrt(fLength));

		for(int32_t i = 0; i < Size; i++)
			mTuple[i] *= fInvScalar;
	}
}



//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
const Vector<Real, Size>& Vector<Real, Size>::Zero() 
{
	static const Vector<Real, Size> zero((Real)0);
	return zero;
}

//----------------------------------------------------------------------------
// debugging output
static const char* VectorMsg[] = { "X = ", ",Y = ", ",Z = ", ",W = " };

template <class Real, int32_t Size>
std::ostream& operator<< (std::ostream& rkOStr, const Vector<Real, Size>& rhs)
{
	int32_t i = 0;
	while( i < Size )
	{
		rkOStr << VectorMsg[i] << rhs[i] << " ";
		i++;
	}
	return rkOStr << std::endl;
}

template <class Real, int32_t Size>
inline Vector<Real, Size> operator* (Real fScalar, const Vector<Real, Size>& rhs)
{
	Vector<Real, Size> result;
	for(int32_t i = 0; i < Size; i++)
		result[i] =  fScalar * rhs[i];

	return result;
}

template< typename Real, int32_t Size >
inline Real Length( const Vector<Real, Size>& vec )
{
	Real result = Real(0);
	for(int32_t i = 0; i < Size; i++)
		result +=  vec[i] * vec[i] ;
	return sqrt(result);
}

template< typename Real, int32_t Size >
inline Real LengthSquared( const Vector<Real, Size>& vec )
{
	Real result = Real(0);
	for(int32_t i = 0; i < Size; i++)
		result +=  vec[i] * vec[i] ;
	return result;
}

template< typename Real, int32_t Size >
inline Real Dot( const Vector<Real, Size>& lfs, const Vector<Real, Size>& rhs )
{
	Real result = Real(0);
	for(int32_t i = 0; i < Size; i++)
		result += lfs[i] * rhs[i];
	return result;
}

template<typename Real>
inline Vector<Real, 3>
Cross( const Vector<Real, 3>& vec1, const Vector<Real, 3>& vec2 )
{
	return Vector<Real, 3>(
		vec1.Y() * vec2.Z() - vec1.Z() * vec2.Y(),
		vec1.Z() * vec2.X() - vec1.X() * vec2.Z(), 
		vec1.X() * vec2.Y() - vec1.Y() * vec2.X());
}

template< typename Real, int32_t Size >
inline Vector<Real, Size> Normalize(const Vector<Real, Size>& vec)
{
	Real fLength = LengthSquared(vec);
	Real fInvScalar = ((Real)1.0)/ ((Real)sqrt((double)fLength));
	return vec * fInvScalar;
}

template< typename Real, int32_t Size >
inline Vector<Real, Size> Clamp( const Vector<Real, Size>& value, const Vector<Real, Size>& min,  const Vector<Real, Size>& max )
{
	Vector<Real, Size> retVal = value;

	for (uint32_t i = 0; i < Size; ++i)
	{
		retVal[i] = (retVal[i] > max[i]) ? max[i] : retVal[i];
		retVal[i] = (retVal[i] < min[i]) ? min[i] : retVal[i];
	}

	return retVal;
}