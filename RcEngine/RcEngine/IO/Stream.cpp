#include "IO/Stream.h"

namespace RcEngine {

static const String NoName;

Stream::Stream()
	: mPosition(0), mSize(0)
{

}

Stream::~Stream()
{

}

const String& Stream::GetName() const
{
	return NoName;
}


int8_t Stream::ReadByte()
{
	int8_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

uint8_t Stream::ReadUByte()
{
	uint8_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

int16_t Stream::ReadShort()
{
	int16_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

uint16_t Stream::ReadUShort()
{
	uint16_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

int32_t Stream::ReadInt()
{
	int32_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

uint32_t Stream::ReadUInt()
{
	uint32_t ret;
	Read(&ret, sizeof ret);
	return ret;
}

bool Stream::ReadBool()
{
	if(ReadByte())
		return true;
	else 
		return false;
}

float Stream::ReadFloat()
{
	float ret;
	Read(&ret, sizeof ret);
	return ret;
}

String Stream::ReadString()
{
	String ret;
	for (;;)
	{
		char c = ReadByte();
		if (!c)
			break;
		else
			ret += c;
	}
	return ret;
}

bool Stream::WriteInt(int32_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteShort(int16_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteByte(int8_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteUInt(uint32_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteUShort(uint16_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteUByte(uint8_t value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteBool(bool value)
{
		return WriteUByte(value ? 1 : 0) == 1;
}

bool Stream::WriteFloat(float value)
{
	return Write(&value, sizeof value) == sizeof value;
}

bool Stream::WriteString(const String& value)
{
	const char* str = value.c_str();
	size_t length = strlen(str);
	return Write(str, length + 1) == length + 1;
}

} //Namespace RcEngine
