namespace Internal 
{

template<typename Real>
Vector<Real,3> IntersectionOfPlanes(const Plane<Real>& a, const Plane<Real>& b, const Plane<Real>& c) 
{
	Real invDenom = -1.0f / Dot(a.Normal, Cross(b.Normal, c.Normal));
	
	Vector<Real,3> A = a.Distance * Cross(b.Normal, c.Normal);
	Vector<Real,3> B = b.Distance * Cross(c.Normal, a.Normal);
	Vector<Real,3> C = c.Distance * Cross(a.Normal, b.Normal);

	return (A + B + C) * invDenom;

	//Real A1 = a.Normal.X(), A2 = a.Normal.Y(), A3 = a.Normal.Z();
	//Real B1 = b.Normal.X(), B2 = b.Normal.Y(), B3 = b.Normal.Z();
	//Real C1 = c.Normal.X(), C2 = c.Normal.Y(), C3 = c.Normal.Z();
	//Real D1 = a.Distance,   D2 = b.Distance,   D3 = c.Distance;  

	//Real x =  D1*(B2*C3-C2*B3) - D2*(A2*C3-C2*A3) + D3*(A2*B3-B2*A3);
	//Real y = -D1*(B1*C3-C1*B3) + D2*(A1*C3-C1*A3) - D3*(A1*B3-B1*A3);
	//Real z =  D1*(B1*C2-C1*B2) - D2*(A1*C2-C1*A2) + D3*(A1*B2-B1*A2);
	
	//return Vector<Real,3>(x * invDenom, y * invDenom, z * invDenom);
}

}


//////////////////////////////////////////////////////////////////////////
template<typename Real>
Frustum<Real>::Frustum()
{

}


//////////////////////////////////////////////////////////////////////////
template<typename Real>
void Frustum<Real>::Update( const Matrix4<Real>& viewProjection )
{
	// Left plane
	Planes[FPS_Left].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M11, 
											  viewProjection.M24 + viewProjection.M21, 
											  viewProjection.M34 + viewProjection.M31);	
	Planes[FPS_Left].Distance =	viewProjection.M44 + viewProjection.M41;
	Planes[FPS_Left].Normalize();

	// Right plane
	Planes[FPS_Right].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M11, 
											   viewProjection.M24 - viewProjection.M21, 
											   viewProjection.M34 - viewProjection.M31);	
	Planes[FPS_Right].Distance= viewProjection.M44 - viewProjection.M41;
	Planes[FPS_Right].Normalize();

	// Top plane
	Planes[FPS_Top].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M12, 
											 viewProjection.M24 - viewProjection.M22, 
											 viewProjection.M34 - viewProjection.M32);
	Planes[FPS_Top].Distance = viewProjection.M44 - viewProjection.M42;
	Planes[FPS_Top].Normalize();

	// Bottom plane
	Planes[FPS_Bottom].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M12, 
											    viewProjection.M24 + viewProjection.M22, 
												viewProjection.M34 + viewProjection.M32);
	Planes[FPS_Bottom].Distance = viewProjection.M44 + viewProjection.M42;
	Planes[FPS_Bottom].Normalize();

	// Near plane
	Planes[FPS_Near].Normal = Vector<Real, 3>(viewProjection.M13, viewProjection.M23, viewProjection.M33);
	Planes[FPS_Near].Distance = viewProjection.M43;
	Planes[FPS_Near].Normalize();

	// Far plane
	Planes[FPS_Far].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M13,
											 viewProjection.M24 - viewProjection.M23,
											 viewProjection.M34 - viewProjection.M33);
	Planes[FPS_Far].Distance = viewProjection.M44 - viewProjection.M43;
	Planes[FPS_Far].Normalize();
}

template<typename Real>
void Frustum<Real>::UpdateCorner()
{
	Corner[FC_Near_Top_Left] = Internal::IntersectionOfPlanes(Planes[FPS_Near], Planes[FPS_Top], Planes[FPS_Left]);
	Corner[FC_Near_Top_Right] = Internal::IntersectionOfPlanes(Planes[FPS_Near], Planes[FPS_Top], Planes[FPS_Right]);
	Corner[FC_Near_Bottom_Left] = Internal::IntersectionOfPlanes(Planes[FPS_Near], Planes[FPS_Bottom], Planes[FPS_Left]);
	Corner[FC_Near_Bottom_Right] = Internal::IntersectionOfPlanes(Planes[FPS_Near], Planes[FPS_Bottom], Planes[FPS_Right]);
	Corner[FC_Far_Top_Left] = Internal::IntersectionOfPlanes(Planes[FPS_Far], Planes[FPS_Top], Planes[FPS_Left]);
	Corner[FC_Far_Top_Right] = Internal::IntersectionOfPlanes(Planes[FPS_Far], Planes[FPS_Top], Planes[FPS_Right]);
	Corner[FC_Far_Bottom_Left] = Internal::IntersectionOfPlanes(Planes[FPS_Far], Planes[FPS_Bottom], Planes[FPS_Left]);
	Corner[FC_Far_Bottom_Right] = Internal::IntersectionOfPlanes(Planes[FPS_Far], Planes[FPS_Bottom], Planes[FPS_Right]);
}


template<typename Real>
ContainmentType Frustum<Real>::Contain( const BoundingSphere<Real>& sphere ) const
{
	bool allInside = true;
	int num = 0;
	for (const Plane<Real>& plane : Planes)
	{
		Real dot = Dot(plane.Normal, sphere.Center) + plane.Distance;
		if (dot < -sphere.Radius)
			return CT_Disjoint;
		
		if( dot > sphere.Radius )
			num++;		
	}

	return (num == 6) ? CT_Contains : CT_Intersects;	
}

template<typename Real>
ContainmentType Frustum<Real>::Contain( const BoundingBox<Real>& aabb ) const
{
	bool allInside = true;
	for (const Plane<Real>& plane : Planes)
	{
		switch (plane.Intersects(aabb))
		{
		case PIT_Back:
			return CT_Disjoint;
		case PIT_Intersecting:
			allInside = false;
			break;
		}
	}

	return allInside ? CT_Contains : CT_Intersects;
}

//template<typename Real>
//ContainmentType Frustum<Real>::Contain( const Frustum<Real>& frustum ) const
//{
//
//}

