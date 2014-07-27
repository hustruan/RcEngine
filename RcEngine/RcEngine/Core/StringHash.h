#ifndef StringHash_h__
#define StringHash_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport StringHash
{
public:
	StringHash();
	StringHash(const char* str);
	StringHash(const String& str);

	StringHash(const StringHash& rhs);

	inline StringHash& operator = (const StringHash& rhs)
	{
		mHash = rhs.mHash;
		return *this;
	}

	inline StringHash operator + (const StringHash& rhs) const
	{
		StringHash ret;
		ret.mHash = mHash + rhs.mHash;
		return ret;
	}

	inline StringHash& operator += (const StringHash& rhs)
	{
		mHash += rhs.mHash;
		return *this;
	}

	inline bool operator == (const StringHash& rhs) const { return mHash == rhs.mHash; }
	inline bool operator != (const StringHash& rhs) const { return mHash != rhs.mHash; }

	// Used for std::map
	inline bool operator< (const StringHash& rhs) const { return mHash < rhs.mHash; }
	inline bool operator> (const StringHash& rhs) const { return mHash > rhs.mHash; }

	inline uint32_t ToHash() const { return mHash; }

	String ToString() const;

private:
	uint32_t CalculateHash(const char* str);

private:
	uint32_t mHash;
};

}

// For std::unordered_map hash
namespace std
{
	template<>
	struct hash<RcEngine::StringHash>
	{
	public:
		std::size_t operator()(const RcEngine::StringHash& k) const 
		{
			return k.ToHash();
		}
	};
}


#endif // StringHash_h__
