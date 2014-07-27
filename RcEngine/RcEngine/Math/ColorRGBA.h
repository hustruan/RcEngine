#ifndef _ColorValue__H
#define _ColorValue__H

#include <Core/Prerequisites.h>

namespace RcEngine{


class _ApiExport ColorRGBA
{
public:
	ColorRGBA ();  // initial values (0,0,0,0)
	ColorRGBA (float fR, float fG, float fB, float fA);
	ColorRGBA (const float* tuple);
	ColorRGBA (const ColorRGBA& rhs);
		
	// assignment
	ColorRGBA& operator= (const ColorRGBA& rhs);

	// member access
	const float* operator() (void) const;
	float* operator() (void);
	float operator[] (size_t i) const;
	float& operator[] (size_t i);
	float R () const;
	float& R ();
	float G () const;
	float& G ();
	float B () const;
	float& B ();
	float A () const;
	float& A ();
		
	// comparison
	bool operator==(const ColorRGBA& rhs) const;
	bool operator!=(const ColorRGBA& rhs) const;

	// arithmetic operations
	ColorRGBA operator+ (const ColorRGBA& rkC) const;
	ColorRGBA operator- (const ColorRGBA& rkC) const;
	ColorRGBA operator* (const ColorRGBA& rkC) const;
	ColorRGBA operator* (float fScalar) const;

	// arithmetic updates
	ColorRGBA& operator+= (const ColorRGBA& rkC);
	ColorRGBA& operator-= (const ColorRGBA& rkC);
	ColorRGBA& operator*= (const ColorRGBA& rkC);
	ColorRGBA& operator*= (float fScalar);

	// Clamps colour value to the range [0, 1].
	void Saturate(void);

	static const ColorRGBA Black;
	static const ColorRGBA White;
	static const ColorRGBA Red;
	static const ColorRGBA Green;
	static const ColorRGBA Blue;

private:

	float mTuple[4];
};

ColorRGBA operator* (float fScalar, const ColorRGBA& rhs);

#include <Math/ColorRGBA.inl>


} // Namespace RcEngine

#endif