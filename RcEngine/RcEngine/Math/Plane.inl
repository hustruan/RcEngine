//  [8/23/2011 hustruan]
//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Plane<Real>& rhs)	
	: Normal(rhs.Normal)
{
	Distance = rhs.Distance;
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane( const Vector<Real, 3>& rkNormal, Real fDistance )
	: Normal(rkNormal)
{
	Distance = fDistance; 
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Vector<Real, 3>& rkNormal, const Vector<Real, 3>& rkP)
	:Normal(rkNormal)
{
	Distance = -Dot(rkNormal, rkP);
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Vector<Real, 3>& rkPoint1, const Vector<Real, 3>& rkPoint2, const Vector<Real, 3>& rkPoint3)
{
	Vector<Real, 3> kEdge1 = rkPoint2 - rkPoint1;
	Vector<Real, 3> kEdge2 = rkPoint3 - rkPoint1;
	Normal = Normalize(Cross(kEdge1, kEdge2));
	Distance = -Dot(Normal, rkPoint1);
}

//----------------------------------------------------------------------------
template< typename Real >
Real Plane<Real>::DotNormal( const Vector<Real, 3>& value ) const
{
	return Dot(Normal, value)
}

//----------------------------------------------------------------------------
template< typename Real >
Real Plane<Real>::DotCoordinate( const Vector<Real, 3>& value ) const
{
	return Dot(Normal, value) + Distance;
}

//----------------------------------------------------------------------------
template< typename Real >
void Plane<Real>::Normalize( )
{
	Real length2 = Dot(Normal, Normal);

	if ( fabs(length2 - 1) > Math<Real>::EPSILON)
	{
		Real invLength = Real(1) / sqrt(length2);
		Normal *= invLength;
		Distance *= invLength;
	}
}

//----------------------------------------------------------------------------
template< typename Real >
PlaneIntersectionType Plane<Real>::Intersects( const BoundingBox<Real>& box ) const
{
	Vector<Real, 3> min, max;
	max.X() = (Normal.X() >= Real(0)) ? box.Min.X() : box.Max.X();
	max.Y() = (Normal.Y() >= Real(0)) ? box.Min.Y() : box.Max.Y();
	max.Z() = (Normal.Z() >= Real(0)) ? box.Min.Z() : box.Max.Z();
	min.X() = (Normal.X() >= Real(0)) ? box.Max.X() : box.Min.X();
	min.Y() = (Normal.Y() >= Real(0)) ? box.Max.Y() : box.Min.Y();
	min.Z() = (Normal.Z() >= Real(0)) ? box.Max.Z() : box.Min.Z();

	Real dot = Dot(Normal, max); 
	if( dot + Distance > Real(0) )
		return PIT_Front;

	dot = Dot(Normal, min); 
	if ( dot + Distance < Real(0))
		return PIT_Back;

	return PIT_Intersecting;
}

//----------------------------------------------------------------------------
template< typename Real >
PlaneIntersectionType Plane<Real>::Intersects( const BoundingSphere<Real>& sphere ) const
{
	Real dot = Dot(sphere.Center,  Normal) + Distance;

	if( dot > sphere.Radius )
		return PIT_Front;

	if( dot < -sphere.Radius )
		return PIT_Back

	return PIT_Intersecting;
}

