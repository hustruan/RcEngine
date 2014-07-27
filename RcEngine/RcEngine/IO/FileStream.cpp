#include <IO/FileStream.h>
#include <Core/Exception.h>

namespace RcEngine {

static const char* OpenMode[] =
{
	"rb",
	"wb",
	"w+b"
};

FileStream::FileStream()
	: mFileHandle(nullptr), mFileMode(FILE_READ)
{

}

FileStream::FileStream( const String& fileName, FileMode mode )
	: mFileHandle(nullptr), mFileMode(FILE_READ)
{
	Open(fileName, mode);
}

FileStream::~FileStream()
{
	Close();
}

uint32_t FileStream::Read( void* dest, uint32_t size )
{
	if (mFileMode == FILE_WRITE)
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
			"File not opened for reading", "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	if (!mFileHandle)
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
			"File not open", "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	if (size + mPosition > mSize)
		size = mSize - mPosition;
		
	if (!size)
		return 0;

	size_t ret = fread(dest, size, 1, (FILE*)mFileHandle);
	if (ret != 1)
	{
		// Return to the position where the read began
		fseek((FILE*)mFileHandle, mPosition, SEEK_SET);
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
			"Error while reading from file " + GetName(),"FileStream::Read( void*, uint32_t)");
		return 0;
	}

	mPosition += size;
	return size;
}

uint32_t FileStream::Write( const void* data, uint32_t size )
{
	if (mFileMode == FILE_READ)
	{
		ENGINE_EXCEPT(Exception::ERR_CANNOT_WRITE_TO_FILE, 
			"File not opened for writing", "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	if (!mFileHandle)
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
			"File not open", "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	if(!size)
		return 0;

	if( 1 != fwrite(data, size, 1, (FILE*)mFileHandle) )
	{
		// Return to the position where the write began
		fseek((FILE*)mFileHandle, mPosition, SEEK_SET);
		ENGINE_EXCEPT(Exception::ERR_CANNOT_WRITE_TO_FILE, 
			"Error while writing to file"+ GetName(), "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	mPosition += size;
	if (mPosition > mSize)
		mSize = mPosition;

	return size;
}

uint32_t FileStream::Seek( uint32_t position )
{
	// Allow sparse seeks if writing
	if (mFileMode == FILE_READ && mPosition > mSize)
		mPosition = mSize;

	if (!mFileHandle)
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, 
			"File not open", "FileStream::Read( void*, uint32_t)");
		return 0;
	}

	fseek((FILE*)mFileHandle, position, SEEK_SET);
	mPosition = position;
	return mPosition;
}

void FileStream::Close()
{
	if (mFileHandle)
	{
		fclose((FILE*)mFileHandle);
		mFileHandle = 0;
		mPosition = 0;
		mSize = 0;
	}
}

void FileStream::Flush()
{
	if (mFileHandle)
		fflush((FILE*)mFileHandle);
}

bool FileStream::Open( const String& fileName, FileMode mode /*= FILE_READ*/ )
{
	Close();

	mFileName = fileName;
	mFileMode = mode;
	mSize = 0;
	mPosition = 0;

	mFileHandle = fopen(fileName.c_str(), OpenMode[mFileMode]);

	if (!mFileHandle)
		return false;

	fseek((FILE*)mFileHandle, 0, SEEK_END);
	mSize = ftell((FILE*)mFileHandle);
	fseek((FILE*)mFileHandle, 0, SEEK_SET);

	return true;
}

} //Namespace RcEngine