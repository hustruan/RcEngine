template<typename Real>
void BoundingBox<Real>::Merge( const Vector<Real,3>& point )
{
	if (!IsValid())
	{
		Max = Min = point;
		return;
	}

	if (point.X() < Min.X())	Min.X() = point.X();
	if (point.Y() < Min.Y())	Min.Y() = point.Y();
	if (point.Z() < Min.Z())	Min.Z() = point.Z();

	if (point.X() > Max.X())	Max.X() = point.X();
	if (point.Y() > Max.Y())	Max.Y() = point.Y();
	if (point.Z() > Max.Z())	Max.Z() = point.Z();
}

template<typename Real>
void BoundingBox<Real>::Merge( const BoundingBox<Real>& box )
{
	// do nothing if the rhs box id undefined.
	if (!box.IsValid()) return;
	
	if (box.Min.X() < Min.X())	Min.X() = box.Min.X();
	if (box.Max.X() > Max.X())	Max.X() = box.Max.X();

	if (box.Min.Y() < Min.Y())	Min.Y() = box.Min.Y();
	if (box.Max.Y() > Max.Y())	Max.Y() = box.Max.Y();

	if (box.Min.Z() < Min.Z())	Min.Z() = box.Min.Z();	
	if (box.Max.Z() > Max.Z())	Max.Z() = box.Max.Z();
}

template <typename Real>
void BoundingBox<Real>::Merge( const BoundingSphere<Real>& sphere )
{
	if (!sphere.IsValid())
		return;

	if(sphere.Center.X()-sphere.Radius < Min.X()) Min.X() = sphere.Center.X()-sphere.Radius;
	if(sphere.Center.X()+sphere.Radius > Max.X()) Max.X() = sphere.Center.X()+sphere.Radius;

	if(sphere.Center.Y()-sphere.Radius < Min.Y()) Min.Y() = sphere.Center.Y()-sphere.Radius;
	if(sphere.Center.Y()+sphere.Radius > Max.Y()) Max.Y() = sphere.Center.Y()+sphere.Radius;

	if(sphere.Center.Z()-sphere.Radius < Min.Z()) Min.Z() = sphere.Center.Z()-sphere.Radius;
	if(sphere.Center.Z()+sphere.Radius > Max.Z()) Max.Z() = sphere.Center.Z()+sphere.Radius;
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const BoundingBox<Real>& box ) const
{
	if( Max.X() < box.Min.X() || Min.X() > box.Max.X() )
		return CT_Disjoint;

	if( Max.Y() < box.Min.Y() || Min.Y() > box.Max.Y() )
		return CT_Disjoint;

	if( Max.Z() < box.Min.Z() || Min.Z() > box.Max.Z() )
		return CT_Disjoint;

	if( Min.X() <= box.Min.X() && box.Max.X() <= Max.X() && 
		Min.Y() <= box.Min.Y() && box.Max.Y() <= Max.Y() &&
		Min.Z() <= box.Min.Z() && box.Max.Z() <= Max.Z() )
		return CT_Contains;

	return CT_Intersects;
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const Vector<Real,3>& vector ) const
{
	if( Min.X() <= vector.X() && vector.X() <= Max.X() && Min.Y() <= vector.Y() && 
		vector.Y() <= Max.Y() && Min.Z() <= vector.Z() && vector.Z() <= Max.Z() )
		return CT_Contains;

	return CT_Disjoint;
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const BoundingSphere<Real>& sphere ) const
{
	Real dist;
	Vector<Real, 3> clamped;

	clamped = Clamp(sphere.Center, Min, Max);
	dist = LengthSquared(sphere.Center - clamped);
	Real radius = sphere.Radius;
	
	if( dist > (radius * radius) )
		return CT_Disjoint;

	if( Min.X() + radius <= sphere.Center.X() && sphere.Center.X() <= Max.X() - radius && 
		Max.X() - Min.X() > radius && Min.Y() + radius <= sphere.Center.Y() && 
		sphere.Center.Y() <= Max.Y() - radius && Max.Y() - Min.Y() > radius && 
		Min.Z() + radius <= sphere.Center.Z() && sphere.Center.Z() <= Max.Z() - radius &&
		Max.X() - Min.X() > radius )
		return CT_Contains;

	return CT_Intersects;
}

template<typename Real>
bool BoundingBox<Real>::Intersects( const BoundingBox<Real>& box ) const
{
	if ( Max.X() < box.Min.X() || Min.X() > box.Max.X() )
		return false;

	if ( Max.Y() < box.Min.Y() || Min.Y() > box.Max.Y() )
		return false;

	return ( Max.Z() >= box.Min.Z() && Min.Z() <= box.Max.Z() );
}

template <typename Real>
bool BoundingBox<Real>::Intersects( const BoundingSphere<Real>& sphere ) const
{
	Vector<Real, 3> clamped = Clamp(sphere.Center, Min, Max);
	Real distanceSquared = LengthSquared(clamped, sphere.Center);
	return distanceSquared <= sphere.Radius*sphere.Radius;
}

template <typename Real>
Vector<Real, 3> BoundingBox<Real>::GetCorner( uint8_t pos ) const
{
	return Vector<Real, 3>(pos&1?Max.X():Min.X(), pos&2?Max.Y():Min.Y(), pos&4?Max.Z():Min.Z());
}


template <typename Real>
void BoundingBox<Real>::GetCorners( Vector<Real, 3> corners[8] ) const
{
	corners[0] = Vector<Real, 3>(Min.X(), Min.Y(), Min.Z());
	corners[1] = Vector<Real, 3>(Max.X(), Min.Y(), Min.Z());
	corners[2] = Vector<Real, 3>(Min.X(), Max.Y(), Min.Z());
	corners[3] = Vector<Real, 3>(Max.X(), Max.Y(), Min.Z());

	corners[4] = Vector<Real, 3>(Min.X(), Min.Y(), Max.Z());
	corners[5] = Vector<Real, 3>(Max.X(), Min.Y(), Max.Z());
	corners[6] = Vector<Real, 3>(Min.X(), Max.Y(), Max.Z());
	corners[7] = Vector<Real, 3>(Max.X(), Max.Y(), Max.Z());
}


//template<typename Real>
//BoundingBox<Real> Merge( const BoundingBox<Real>& box1, const BoundingBox<Real>& box2 )
//{
//	BoundingBox<Real> retVal = box2;
//
//	if (box1.Min.X() < box2.Min.X())	retVal.Min.X() = box1.Min.X();
//	if (box1.Min.Y() < box2.Min.Y())	retVal.Min.Y() = box1.Min.Y();
//	if (box1.Min.Z() < box2.Min.Z())	retVal.Min.Z() = box1.Min.Z();
//
//	if (box1.Max.X() > box2.Max.X())	retVal.Max.X() = box1.Max.X();
//	if (box1.Max.Y() > box2.Max.Y())	retVal.Max.Y() = box1.Max.Y();
//	if (box1.Max.Z() > box2.Max.Z())	retVal.Max.Z() = box1.Max.Z();
//
//	return retVal;
//}