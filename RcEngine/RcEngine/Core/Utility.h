#pragma once
#ifndef _Utility__H
#define _Utility__H

#include <Core/Prerequisites.h>
#include <type_traits>
#include <codecvt>

namespace RcEngine {

// Convert string to other number type
template <typename Target, typename Source>
inline typename Target LexicalCast( const Source& value )
{
	Target re;
	std::stringstream str;
	str << value;
	str >> re;
	return re;
}

template <typename T, unsigned int Size>
inline T SwapBytes(T value)
{
	union {
		T value;
		char bytes[Size];
	} in, out;

	in.value = value;
	
	for (unsigned int i = 0; i < Size/2; ++i)
	{
		out.bytes[i] = in.bytes[Size-1-i];
		out.bytes[Size-1-i] = in.bytes[i];
	}

	return out.value;
}

template <typename EndianFrom, typename EndianTo, typename T>
inline T EndianSwapBytes(T value)
{
	static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
	static_assert(std::is_arithmetic<T>::value);

	if (EndianFrom == EndianTo)
		return value;
	return SwapBytes<T, sizeof(T)>(value);
}

inline WString StringToWString(const String& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	return convert.from_bytes(src);
}

inline String WStringToString(const WString& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	return convert.to_bytes(src);
}

/**
 * Convert float to uint32
 */
inline uint32_t FloatToUint32(float v)
{
	union 
	{
		float f;
		uint32_t u;

	} helper;

	helper.f = v;

	return helper.u;
}

inline float Uint32ToFloat(uint32_t v)
{
	union 
	{
		float f;
		uint32_t u;

	} helper;

	helper.u = v;

	return helper.f;
}

template <class T>
inline void HashCombine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


inline void Split(std::vector<String>& tokens, const String& text, char sep)
{
	size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != String::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
}

}

#endif // _Utility__H