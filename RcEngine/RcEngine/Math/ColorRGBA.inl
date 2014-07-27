
//----------------------------------------------------------------------------
inline const float* ColorRGBA::operator() (void) const
{
	return mTuple;
}
//----------------------------------------------------------------------------
inline float* ColorRGBA::operator() (void)
{
	return mTuple;
}
//----------------------------------------------------------------------------
inline float ColorRGBA::operator[] (size_t i) const
{
	assert(0 <= i && i <= 3);
	if (i < 0)
	{
		i = 0;
	}
	else if ( i > 3 )
	{
		i = 3;
	}

	return mTuple[i];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::operator[] (size_t i)
{
	assert(0 <= i && i <= 3);
	if (i < 0)
	{
		i = 0;
	}
	else if ( i > 3 )
	{
		i = 3;
	}

	return mTuple[i];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::R () const
{
	return mTuple[0];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::R ()
{
	return mTuple[0];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::G () const
{
	return mTuple[1];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::G ()
{
	return mTuple[1];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::B () const
{
	return mTuple[2];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::B ()
{
	return mTuple[2];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::A () const
{
	return mTuple[3];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::A ()
{
	return mTuple[3];
}
//----------------------------------------------------------------------------
