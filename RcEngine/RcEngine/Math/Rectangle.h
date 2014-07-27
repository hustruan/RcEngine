#ifndef Rectangle_h__
#define Rectangle_h__

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine {

template<typename Real>
class Rectangle
{
public:
	typedef Real value_type;

public:
	Rectangle();
	Rectangle(Real x, Real y, Real width, Real height);
	Rectangle(const Rectangle& other);

	inline Real Left() const;
	inline Real Top() const;
	inline Real Right() const;
	inline Real Bottom() const;
	
	inline void SetLeft(Real left);
	inline void SetTop(Real top);
	inline void SetRight(Real right);
	inline void SetBottom(Real bpttom);

	inline void Offset(Real offsetLeft, Real offsetTop);

	inline Vector<Real, 2> TopLeft() const;
	inline Vector<Real, 2> TopRight() const;
	inline Vector<Real, 2> BottomLeft() const;
	inline Vector<Real, 2> BottomRight() const;

	inline bool Contains(Real x, Real y) const;
	inline ContainmentType Contains(Real x, Real y, Real width, Real height) const;
	inline ContainmentType Contains(const Rectangle& other) const;

	inline bool Intersects(Real x, Real y, Real width, Real height) const;
	inline bool Intersects(const Rectangle& other) const;

	inline bool operator == (const Rectangle& other) const;
	inline bool operator != (const Rectangle& other) const;

public:
	Real X, Y;
	Real Width, Height;
};


/**
 * Creates a Rectangle defining the area where one rectangle overlaps with another rectangle.
 */
//template<typename Real>
//Rectangle<Real> Intersect(const Rectangle<Real>& lhs, const Rectangle<Real>& rhs);

/**
 * Creates a Rectangle defining the area which cover both rectangle.
 */
template<typename Real>
Rectangle<Real> Union(const Rectangle<Real>& lhs, const Rectangle<Real>& rhs);

#include <Math/Rectangle.inl>

typedef Rectangle<float> Rectanglef;
typedef Rectangle<int32_t> IntRect;

}




#endif // Rectangle_h__
