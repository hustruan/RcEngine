#ifndef FileStream_h__
#define FileStream_h__

#include <Core/Prerequisites.h>
#include <IO/Stream.h>

namespace RcEngine {

/// File open mode.
enum FileMode
{
	FILE_READ = 0,
	FILE_WRITE,
	FILE_READWRITE
};

class _ApiExport FileStream : public Stream
{
public:
	FileStream();
	FileStream(const String& fileName, FileMode mode);
	virtual ~FileStream();

	virtual const String& GetName() const	{ return mFileName; }
	virtual uint32_t Read(void* dest, uint32_t size);
	virtual uint32_t Write(const void* data, uint32_t size);
	virtual uint32_t Seek(uint32_t position);

	virtual void Close();
	virtual void Flush();

	bool Open(const String& fileName, FileMode mode = FILE_READ);

	FileMode GetMode() const { return mFileMode; }
	bool IsOpen() const { return mFileHandle != 0; }
	void* GetHandle() const { return mFileHandle; }

protected:
	String mFileName;
	FileMode mFileMode;
	void* mFileHandle;
};

} //Namespace RcEngine

#endif // FileStream_h__