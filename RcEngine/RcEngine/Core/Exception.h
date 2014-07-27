#ifndef Exception_h__
#define Exception_h__

#include <Core/Prerequisites.h>

namespace RcEngine{


#ifndef ENGINE_EXCEPT
#define ENGINE_EXCEPT(num, desc, src) throw RcEngine::ExceptionFactory::Create( \
	RcEngine::ExceptionCodeType<num>(), desc, src, __FILE__, __LINE__ );
#endif


class _ApiExport Exception : public std::exception
{
public:
	enum ExceptionCodes
	{
		ERR_CANNOT_WRITE_TO_FILE, 	
		ERR_INVALID_STATE,	
		ERR_INVALID_PARAMS, 	
		ERR_RENDERINGAPI_ERROR,
		ERR_DUPLICATE_ITEM, 	
		ERR_ITEM_NOT_FOUND, 	
		ERR_FILE_NOT_FOUND, 	
		ERR_INTERNAL_ERROR, 	
		ERR_RT_ASSERTION_FAILED, 	
		ERR_NOT_IMPLEMENTED 	
	};

protected:

	long line;							// Exception line number.
	int32_t  number;						// Exception type
	std::string typeName;				// Exception type name
	std::string description;			// Exception description
	std::string source;				
	std::string file;
	mutable std::string fullDesc;

public:

	// Default Constructor
	Exception( int32_t typeNum, const std::string& description, const std::string& source );

	// Advanced Constructor
	Exception( int32_t typeNum, const std::string& description, const std::string& source, const char* type, const char* file, long line );

	// Copy Constructor
	Exception(const Exception& rhs);

	virtual ~Exception() throw()  {}

	void operator = (const Exception& rhs);

	virtual const std::string& GetFullDescription(void) const;

	virtual int32_t GetNumber(void) const throw();

	virtual const std::string& GetSource() const; 

	virtual const std::string& GetFile() const;

	virtual long GetLineNumber() const; 

	virtual const std::string& GetDescription( void ) const;

	/// Override std::exception::what
	const char* what() const throw() { return GetFullDescription().c_str(); }        

};

/** Template struct which creates a distinct type for each exception code.
@note
This is useful because it allows us to create an overloaded method
for returning different exception types by value without ambiguity. 
From 'Modern C++ Design' (Alexandrescu 2001).
*/
template <int32_t num>
struct ExceptionCodeType
{
	enum { number = num };
};

class _ApiExport UnimplementedException : public Exception 
{
public:
	UnimplementedException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "UnimplementedException", inFile, inLine) {}
};

class _ApiExport FileNotFoundException : public Exception
{
public:
	FileNotFoundException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "FileNotFoundException", inFile, inLine) {}
};

class _ApiExport IOException : public Exception
{
public:
	IOException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "IOException", inFile, inLine) {}
};

class _ApiExport InvalidStateException : public Exception
{
public:
	InvalidStateException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "InvalidStateException", inFile, inLine) {}
};

class _ApiExport InvalidParametersException : public Exception
{
public:
	InvalidParametersException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "InvalidParametersException", inFile, inLine) {}
};

class _ApiExport ItemIdentityException : public Exception
{
public:
	ItemIdentityException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "ItemIdentityException", inFile, inLine) {}
};

class _ApiExport InternalErrorException : public Exception
{
public:
	InternalErrorException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "InternalErrorException", inFile, inLine) {}
};

class _ApiExport RenderingAPIException : public Exception
{
public:
	RenderingAPIException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "RenderingAPIException", inFile, inLine) {}
};

class  _ApiExport RuntimeAssertionException : public Exception
{
public:
	RuntimeAssertionException(int32_t inNumber, const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
		: Exception(inNumber, inDescription, inSource, "RuntimeAssertionException", inFile, inLine) {}
};


class ExceptionFactory
{
private:
	/// Private constructor, no construction
	ExceptionFactory() {}
public:
	static UnimplementedException Create(
		ExceptionCodeType<Exception::ERR_NOT_IMPLEMENTED> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return UnimplementedException(code.number, desc, src, file, line);
	}

	static FileNotFoundException Create(
		ExceptionCodeType<Exception::ERR_FILE_NOT_FOUND> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return FileNotFoundException(code.number, desc, src, file, line);
	}

	static IOException Create(
		ExceptionCodeType<Exception::ERR_CANNOT_WRITE_TO_FILE> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return IOException(code.number, desc, src, file, line);
	}

	static InvalidStateException Create(
		ExceptionCodeType<Exception::ERR_INVALID_STATE> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return InvalidStateException(code.number, desc, src, file, line);
	}

	static InvalidParametersException Create(
		ExceptionCodeType<Exception::ERR_INVALID_PARAMS> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return InvalidParametersException(code.number, desc, src, file, line);
	}

	static ItemIdentityException Create(
		ExceptionCodeType<Exception::ERR_ITEM_NOT_FOUND> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return ItemIdentityException(code.number, desc, src, file, line);
	}

	static ItemIdentityException Create(
		ExceptionCodeType<Exception::ERR_DUPLICATE_ITEM> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return ItemIdentityException(code.number, desc, src, file, line);
	}

	static InternalErrorException Create(
		ExceptionCodeType<Exception::ERR_INTERNAL_ERROR> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return InternalErrorException(code.number, desc, src, file, line);
	}

	static RenderingAPIException Create(
		ExceptionCodeType<Exception::ERR_RENDERINGAPI_ERROR> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return RenderingAPIException(code.number, desc, src, file, line);
	}

	static RuntimeAssertionException Create(
		ExceptionCodeType<Exception::ERR_RT_ASSERTION_FAILED> code, 
		const std::string& desc, 
		const std::string& src, const char* file, long line)
	{
		return RuntimeAssertionException(code.number, desc, src, file, line);
	}

};



} // Namespace RcEngine

#endif // Exception_h__