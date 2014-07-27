#ifndef Variant_h__
#define Variant_h__

#include <Core/Prerequisites.h>
#include <Core/StringHash.h>

namespace RcEngine {

enum VariantType
{
	Var_None, 
	Var_Bool,
	Var_Int8,
	Var_Int16,
	Var_Int32,
	Var_Int64,
	Var_UInt8,
	Var_UInt16,
	Var_UInt32,
	Var_UInt64,
	Var_Float,
	Var_Double,
	Var_Ptr
};

class _ApiExport Variant
{
public:
	Variant();
	Variant(const Variant& val);

	explicit Variant(const bool& val);
	explicit Variant(const int8_t& val);
	explicit Variant(const int16_t& val);
	explicit Variant(const int32_t& val);
	explicit Variant(const int64_t& val);
	explicit Variant(const uint8_t& val);
	explicit Variant(const uint16_t& val);
	explicit Variant(const uint32_t& val);
	explicit Variant(const uint64_t& val);
	explicit Variant(const float& val);
	explicit Variant(const double& val);
	explicit Variant(void* val);

	VariantType GetType() const { return mVariantType; }

	Variant& operator= (const Variant& val);

	Variant& operator= (const bool& val);
	Variant& operator= (const int8_t& val);
	Variant& operator= (const int16_t& val);
	Variant& operator= (const int32_t& val);
	Variant& operator= (const int64_t& val);
	Variant& operator= (const uint8_t& val);
	Variant& operator= (const uint16_t& val);
	Variant& operator= (const uint32_t& val);
	Variant& operator= (const uint64_t& val);
	Variant& operator= (const float& val);
	Variant& operator= (const double& val);
	Variant& operator= (void* val);
	
	bool boolValue() const { return mVariantType == Var_Bool ? mValue.boolValue : false; }

	int8_t  int8Value() const  { return mVariantType == Var_Int8 ? mValue.int8Value : 0; } 
	int16_t int16Value() const { return mVariantType == Var_Int8 ? mValue.int16Value : 0; } 
	int32_t int32Value() const { return mVariantType == Var_Int8 ? mValue.int32Value : 0; } 
	int64_t int64Value() const { return mVariantType == Var_Int8 ? mValue.int64Value : 0; } 

	uint8_t  uint8Value() const  { return mVariantType == Var_UInt8 ? mValue.uint8Value : 0; } 
	uint16_t uint16Value() const { return mVariantType == Var_UInt8 ? mValue.uint16Value : 0; } 
	uint32_t uint32Value() const { return mVariantType == Var_UInt8 ? mValue.uint32Value : 0; } 
	uint64_t uint64Value() const { return mVariantType == Var_UInt8 ? mValue.uint64Value : 0; } 

	float  floatValue() const  { return mVariantType == Var_Float ? mValue.floatValue : 0.0f; }
	double doubleValue() const { return mVariantType == Var_Bool ? mValue.boolValue : 0.0; }

	bool operator == (const Variant& rhs);

private:

	void SetType(VariantType type) { mVariantType = type; }

private:
	VariantType mVariantType;

	union 
	{
		bool boolValue;

		int8_t  int8Value;
		int16_t int16Value;
		int32_t int32Value;
		int64_t int64Value;

		uint8_t  uint8Value;
		uint16_t uint16Value;
		uint32_t uint32Value;
		uint64_t uint64Value;

		float  floatValue;
		double doubleValue;

		void* ptrValue;

	} mValue;
};

typedef std::unordered_map<StringHash, Variant> VariantMap;

}

#endif // Variant_h__
