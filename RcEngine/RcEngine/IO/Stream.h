#ifndef Stream_h__
#define Stream_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport Stream
{
public:
	Stream();
	virtual ~Stream();

	/// Return name of the stream.
	virtual const String& GetName() const;
	/// Read bytes from the stream. Return number of bytes actually read.
	virtual uint32_t Read(void* dest, uint32_t size) = 0;
	/// Set position from the beginning of the stream.
	virtual uint32_t Seek(uint32_t position) = 0;
	/// Return current position.
	inline uint32_t GetPosition() const { return mPosition; }
	/// Return size.
	inline uint32_t GetSize() const { return mSize; }
	/// Return whether the end of stream has been reached.
	inline bool IsEof() const { return mPosition >= mSize; }

	/// Closes the current stream and releases any resources associated with the current stream.
	virtual void Close() = 0;
	/// Clears all buffers for this stream and flush any buffered output to the underlying device.
	virtual void Flush() = 0;

	/// Read a 32-bit integer.
	int32_t ReadInt();
	/// Read a 16-bit integer.
	int16_t ReadShort();
	/// Read an 8-bit integer.
	int8_t ReadByte();
	/// Read a 32-bit unsigned integer.
	uint32_t ReadUInt();
	/// Read a 16-bit unsigned integer.
	uint16_t ReadUShort();
	/// Read an 8-bit unsigned integer.
	uint8_t ReadUByte();
	/// Read a bool.
	bool ReadBool();
	/// Read a float.
	float ReadFloat();
	/// Read a null-terminated string.
	String ReadString();

	/// Write bytes to the stream. Return number of bytes actually written.
	virtual uint32_t Write(const void* data, uint32_t size) = 0;
	/// Write a 32-bit integer.
	bool WriteInt(int32_t value);
	/// Write a 16-bit integer.
	bool WriteShort(int16_t value);
	/// Write an 8-bit integer.
	bool WriteByte(int8_t value);
	/// Write a 32-bit unsigned integer.
	bool WriteUInt(uint32_t value);
	/// Write a 16-bit unsigned integer.
	bool WriteUShort(uint16_t value);
	/// Write an 8-bit unsigned integer.
	bool WriteUByte(uint8_t value);
	/// Write a bool.
	bool WriteBool(bool value);
	/// Write a float.
	bool WriteFloat(float value);
	/// Write a null-terminated string.
	bool WriteString(const String& value);

protected:
	uint32_t mPosition;
	uint32_t mSize;
};

} //Namespace RcEngine

#endif // Stream_h__