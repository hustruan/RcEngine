#ifndef PathUtil_h__
#define PathUtil_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport PathUtil
{
public:
	
	static void SplitPath(const String& fullPath, String& pathName, String& fileName, String& extension);

	static String GetParentPath(const String& pathName);
	static String GetPath(const String& fullPath);
	static String GetFileExtension(const String& fullPath);
	static String GetFileName(const String& fullPath);
	static String GetFileNameAndExtension(const String& fullPath);

	/**
	 * Add a slash at the end of the path if missing and convert to internal format (use slashes.)
	*/
	static String AddTrailingSlash(const String& pathName);

	/**
	 * Remove the slash from the end of a path if exists and convert to internal format (use slashes.)
	*/ 
	static String RemoveTrailingSlash(const String& pathName);

	static String GetInternalPath(const String& fullPath);
};

}


#endif // PathUtil_h__
