template<typename Real>
void BoundingSphere<Real>::Merge( const Vector<Real, 3>& pt )
{
	if (IsValid())
	{
		Vector<Real, 3> offset = pt - Center;
		Real dist = offset.Length();

		if (dist > Radius)
		{
			Real half = (dist - Radius) * Real(0.5);
			Radius += half;
			Center += offset * (half / dist);
		}
	}
	else
	{
		Center = pt;
		Radius = Real(0);
	}
}

template<typename Real>
void BoundingSphere<Real>::Merge( const BoundingSphere<Real>& sphere )
{
	// do nothing if the rhs sphere id undefined.
	if (!sphere.IsValid())
	{
		return;
	}
	// Otherwise if current null, just take rhs sphere
	else if (!IsValid())
	{
		Center = sphere.Center;
		Radius = sphere.Radius;
		return;
	}
	// Otherwise merge
	else
	{
		Vector<Real, 3> difference  = sphere.Center - Center;
		Real distance = difference .Length();

		// New sphere is already inside this one
		if (distance + sphere.Radius <= Radius)
			return;

		//  New sphere completely contains this one 
		if (distance + Radius <= sphere.Radius)
		{
			Center = sphere.Center;
			Radius = sphere.Radius;
			return;
		}

		// Build a new sphere that completely contains the other two:
		Real newRadius = (distance + Radius + sphere.Radius) * Real(0.5);
		Real ratio = ( newRadius - Radius ) / distance ;

		Center += difference * ratio;
		Radius = newRadius;
	}
}

template<typename Real>
void BoundingSphere<Real>::Merge( const BoundingBox<Real>& box )
{
	if (box.IsValid())
	{
		if (Valid())
		{
			const Vector<Real,3>& min = box.Min;
			const Vector<Real,3>& max = box.Max;

			Merge(min);
			Merge(Vector<Real,3>(max.X(), min.Y(), min.Z()));
			Merge(Vector<Real,3>(min.X(), max.Y(), min.Z()));
			Merge(Vector<Real,3>(max.X(), max.Y(), min.Z()));
			Merge(Vector<Real,3>(min.X(), min.Y(), max.Z()));
			Merge(Vector<Real,3>(max.X(), min.Y(), max.Z()));
			Merge(Vector<Real,3>(min.X(), max.Y(), max.Z()));
			Merge(max);
		}
		else
		{
			Vector<Real,3> difference = box.Max - box.Min;
			Center = (box.Min + box.Max) * Real(0.5);
			Radius = difference.Length() * Real(0.5);
		}
	}
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const BoundingSphere<Real>& sphere ) const
{
	Real distance = Length(Center - sphere.Center);

	if( Radius + sphere.Radius < distance )
		return CT_Disjoint;

	if( Radius - sphere.Radius < distance )
		return CT_Intersects;

	return CT_Contains;
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const BoundingBox<Real>& box ) const
{
	if( !box.Intersects(*this) )
		return CT_Disjoint;

	Vector<Real, 3> vector;

	Real radius = Radius * Radius;
	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( LengthSquared(vector) > radius )
		return CT_Intersects;

	return CT_Contains;
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const Vector<Real,3>& vector ) const
{
	Vector<Real, 3> offset = vector - Center;
	Real distanceSquared = LengthSquared(offset);

	if( distanceSquared >= (Radius * Radius) )
		return CT_Disjoint;

	return CT_Contains;
}

template<typename Real>
bool BoundingSphere<Real>::Intersects( const BoundingBox<Real>& box ) const
{
	Vector<Real, 3> clamped = Clamp(Center, box.Min, box.Max);
	Real distanceSquared = LengthSquared(clamped, Center);
	return distanceSquared <= Radius*Radius;
}

template<typename Real>
bool BoundingSphere<Real>::Intersects( const BoundingSphere<Real>& sphere ) const
{
	Real distance2 = LengthSquared(Center - sphere.Center);
	if ( (Radius + sphere.Radius)*(Radius + sphere.Radius) <= distance2)
		return false;

	return true;
}


//template<typename Real>
//BoundingSphere<Real> Merge(const BoundingSphere<Real>& sphere1,const BoundingSphere<Real>& sphere2 )
//{
//	BoundingSphere sphere;
//	Vector<Real,3> difference = sphere2.Center - sphere1.Center;
//
//	Real length = difference.Length();
//	Real radius = sphere1.Radius;
//	Real radius2 = sphere2.Radius;
//
//	if( radius + radius2 >= length)
//	{
//		if( radius - radius2 >= length )
//			return sphere1;
//
//		if( radius2 - radius >= length )
//			return sphere2;
//	}
//
//	Vector<Real,3> vector = difference * ( Real(1) / length );
//	Real min = (std::min)( -radius, length - radius2 );
//	Real max = ( (std::max)( radius, length + radius2 ) - min ) * Real(0.5);
//
//	sphere.Center = sphere1.Center + vector * ( max + min );
//	sphere.Radius = max;
//
//	return sphere;
//}


