#include "ColorRGBA.h"

namespace RcEngine{

const ColorRGBA ColorRGBA::Black(0.0f,0.0f,0.0f,1.0f);
const ColorRGBA ColorRGBA::White(1.0f,1.0f,1.0f,1.0f);
const ColorRGBA ColorRGBA::Red(1.0f,0.0f,0.0f,1.0f);
const ColorRGBA ColorRGBA::Green(0.0f,1.0f,0.0f,1.0f);
const ColorRGBA ColorRGBA::Blue(0.0f,0.0f,1.0f,1.0f);


//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA ()
{
	mTuple[0] = 0.0f;
	mTuple[1] = 0.0f;
	mTuple[2] = 0.0f;
	mTuple[3] = 0.0f;
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (float fR, float fG, float fB, float fA)
{
	mTuple[0] = fR;
	mTuple[1] = fG;
	mTuple[2] = fB;
	mTuple[3] = fA;
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (const float* afTuple)
{
	mTuple[0] = afTuple[0];
	mTuple[1] = afTuple[1];
	mTuple[2] = afTuple[2];
	mTuple[3] = afTuple[3];
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (const ColorRGBA& rkC)
{
	mTuple[0] = rkC.mTuple[0];
	mTuple[1] = rkC.mTuple[1];
	mTuple[2] = rkC.mTuple[2];
	mTuple[3] = rkC.mTuple[3];
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator= (const ColorRGBA& rkC)
{
	mTuple[0] = rkC.mTuple[0];
	mTuple[1] = rkC.mTuple[1];
	mTuple[2] = rkC.mTuple[2];
	mTuple[3] = rkC.mTuple[3];
	return *this;
}

//----------------------------------------------------------------------------
bool ColorRGBA::operator== (const ColorRGBA& rkC) const
{
	return
		mTuple[0] == rkC.mTuple[0] &&
		mTuple[1] == rkC.mTuple[1] &&
		mTuple[2] == rkC.mTuple[2] &&
		mTuple[3] == rkC.mTuple[3];
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator!= (const ColorRGBA& rkC) const
{
	return
		mTuple[0] != rkC.mTuple[0] ||
		mTuple[1] != rkC.mTuple[1] ||
		mTuple[2] != rkC.mTuple[2] ||
		mTuple[3] != rkC.mTuple[3];
}

//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator+ (const ColorRGBA& rkC) const
{
	return ColorRGBA(
		mTuple[0] + rkC.mTuple[0],
		mTuple[1] + rkC.mTuple[1],
		mTuple[2] + rkC.mTuple[2],
		mTuple[3] + rkC.mTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator- (const ColorRGBA& rkC) const
{
	return ColorRGBA(
		mTuple[0] - rkC.mTuple[0],
		mTuple[1] - rkC.mTuple[1],
		mTuple[2] - rkC.mTuple[2],
		mTuple[3] - rkC.mTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator* (const ColorRGBA& rkC) const
{
	return ColorRGBA(
		mTuple[0]*rkC.mTuple[0],
		mTuple[1]*rkC.mTuple[1],
		mTuple[2]*rkC.mTuple[2],
		mTuple[3]*rkC.mTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator* (float fScalar) const
{
	return ColorRGBA(
		fScalar*mTuple[0],
		fScalar*mTuple[1],
		fScalar*mTuple[2],
		fScalar*mTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA operator* (float fScalar, const ColorRGBA& rkC)
{
	return ColorRGBA(
		fScalar*rkC[0],
		fScalar*rkC[1],
		fScalar*rkC[2],
		fScalar*rkC[3]);
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator+= (const ColorRGBA& rkC)
{
	mTuple[0] += rkC.mTuple[0];
	mTuple[1] += rkC.mTuple[1];
	mTuple[2] += rkC.mTuple[2];
	mTuple[3] += rkC.mTuple[3];
	return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator-= (const ColorRGBA& rkC)
{
	mTuple[0] -= rkC.mTuple[0];
	mTuple[1] -= rkC.mTuple[1];
	mTuple[2] -= rkC.mTuple[2];
	mTuple[3] -= rkC.mTuple[3];
	return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator*= (const ColorRGBA& rkC)
{
	mTuple[0] *= rkC.mTuple[0];
	mTuple[1] *= rkC.mTuple[1];
	mTuple[2] *= rkC.mTuple[2];
	mTuple[3] *= rkC.mTuple[3];
	return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator*= (float fScalar)
{
	mTuple[0] *= fScalar;
	mTuple[1] *= fScalar;
	mTuple[2] *= fScalar;
	mTuple[3] *= fScalar;
	return *this;
}

void ColorRGBA::Saturate( void )
{
	for (int32_t i = 0; i < 4; i++)
	{
		if (mTuple[i] > 1.0f)
		{
			mTuple[i] = 1.0f;
		}
		else if (mTuple[i] < 0.0f)
		{
			mTuple[i] = 0.0f;
		}
	}
}


} // Namespace RcEngine