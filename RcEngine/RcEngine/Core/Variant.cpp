#include <Core/Variant.h>

namespace RcEngine {

Variant::Variant() 
	: mVariantType(Var_None)
{

}

Variant::Variant( const Variant& val )
	: mVariantType(val.mVariantType), mValue(val.mValue)
{

}

Variant::Variant( const bool& val )
{
	mVariantType = Var_Bool;
	mValue.boolValue = val;
}

Variant::Variant( const int8_t& val )
{
	mVariantType = Var_Int8;
	mValue.int8Value = val;
}

Variant::Variant( const int16_t& val )
{
	mVariantType = Var_Int16;
	mValue.int16Value = val;
}

Variant::Variant( const int32_t& val )
{
	mVariantType = Var_Int32;
	mValue.int32Value = val;
}

Variant::Variant( const int64_t& val )
{
	mVariantType = Var_Int64;
	mValue.int64Value = val;
}

Variant::Variant( const uint8_t& val )
{
	mVariantType = Var_UInt8;
	mValue.uint8Value = val;
}

Variant::Variant( const uint16_t& val )
{
	mVariantType = Var_UInt16;
	mValue.uint16Value = val;
}

Variant::Variant( const uint32_t& val )
{
	mVariantType = Var_UInt32;
	mValue.uint32Value = val;
}

Variant::Variant( const uint64_t& val )
{
	mVariantType = Var_UInt64;
	mValue.uint64Value = val;
}

Variant::Variant( const float& val )
{
	mVariantType = Var_Float;
	mValue.floatValue = val;
}

Variant::Variant( const double& val )
{
	mVariantType = Var_Double;
	mValue.doubleValue = val;
}

Variant::Variant(void* val)
{
	mVariantType = Var_Ptr;
	mValue.ptrValue = val;
}

Variant& Variant::operator=( const Variant& rhs )
{
	mVariantType = rhs.mVariantType;
	mValue = rhs.mValue;
	return *this;
}

Variant& Variant::operator=( const bool& var )
{
	mVariantType = Var_Bool;
	mValue.boolValue = var;
	return *this;
}

Variant& Variant::operator=( const int8_t& var )
{
	mVariantType = Var_Int8;
	mValue.int8Value = var;
	return *this;
}

Variant& Variant::operator=( const int16_t& var )
{
	mVariantType = Var_Int16;
	mValue.int16Value = var;
	return *this;
}

Variant& Variant::operator=( const int32_t& var )
{
	mVariantType = Var_Int32;
	mValue.int32Value = var;
	return *this;
}

Variant& Variant::operator=( const int64_t& var )
{
	mVariantType = Var_Int64;
	mValue.int64Value = var;
	return *this;
}

Variant& Variant::operator=( const uint8_t& var )
{
	mVariantType = Var_UInt8;
	mValue.uint8Value = var;
	return *this;
}

Variant& Variant::operator=( const uint16_t& var )
{
	mVariantType = Var_UInt16;
	mValue.uint16Value = var;
	return *this;
}

Variant& Variant::operator=( const uint32_t& var )
{
	mVariantType = Var_UInt32;
	mValue.uint32Value = var;
	return *this;
}

Variant& Variant::operator=( const uint64_t& var )
{
	mVariantType = Var_UInt8;
	mValue.uint64Value = var;
	return *this;
}

Variant& Variant::operator=( const float& var )
{
	mVariantType = Var_Float;
	mValue.floatValue = var;
	return *this;
}

Variant& Variant::operator=( const double& var )
{
	mVariantType = Var_Double;
	mValue.doubleValue = var;
	return *this;
}

Variant& Variant::operator=( void* var )
{
	mVariantType = Var_Ptr;
	mValue.ptrValue= var;
	return *this;
}

bool Variant::operator ==( const Variant& rhs )
{
	if (mVariantType != rhs.mVariantType)
		return false;

	switch (mVariantType)
	{
	case Var_Bool:
		return mValue.boolValue == rhs.mValue.boolValue;
	case Var_Int8:
		return mValue.int8Value == rhs.mValue.int8Value;
	case Var_Int16:
		return mValue.int16Value == rhs.mValue.int16Value;
	case Var_Int32:
		return mValue.int32Value == rhs.mValue.int32Value;
	case Var_Int64:
		return mValue.int64Value == rhs.mValue.int64Value;
	case Var_UInt8:
		return mValue.uint8Value == rhs.mValue.uint8Value;
	case Var_UInt16:
		return mValue.uint16Value == rhs.mValue.uint16Value;
	case Var_UInt32:
		return mValue.uint32Value == rhs.mValue.uint32Value;
	case Var_UInt64:
		return mValue.uint64Value == rhs.mValue.uint64Value;
	case Var_Double:
		return mValue.doubleValue == rhs.mValue.doubleValue;
	case Var_Float:
		return mValue.floatValue == rhs.mValue.floatValue;
	case Var_Ptr:
		return mValue.ptrValue == rhs.mValue.ptrValue;
	default:
		return false;
	}

	return false;
}

}