#include <Core/StringHash.h>

namespace RcEngine {

StringHash::StringHash() 
	: mHash(0)
{

}

StringHash::StringHash( const StringHash& rhs ) 
	: mHash(rhs.mHash)
{

}

StringHash::StringHash( const char* str ) 
	: mHash(CalculateHash(str))
{

}

StringHash::StringHash( const String& str ) 
	: mHash(CalculateHash(str.c_str()))
{

}

uint32_t StringHash::CalculateHash( const char* str )
{
	uint32_t hash = 0;

	if (!str)
		return hash;

	int c;
	while( c = *str++ )
		hash = c + (hash << 6) + (hash << 16) - hash;  // SDBM Hash

	return hash;
}

String StringHash::ToString() const
{
	char tempBuffer[128];
	sprintf(tempBuffer, "%08X", mHash);
	return String(tempBuffer);
}

}