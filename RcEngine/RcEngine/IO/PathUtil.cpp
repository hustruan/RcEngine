#include <IO/PathUtil.h>

namespace RcEngine {

void PathUtil::SplitPath( const String& fullPath, String& pathName, String& fileName, String& extension )
{
	String fullPathCopy = GetInternalPath(fullPath);

	size_t extPos = fullPathCopy.find_last_of('.');
	size_t pathPos = fullPathCopy.find_last_of('/');

	if (extPos != String::npos || (pathPos == String::npos || pathPos > extPos))
	{
		String extStr = fullPathCopy.substr(extPos);
		std::transform(extStr.begin(), extStr.end(), extStr.begin(), (int(*)(int))tolower);
		 extension = extStr;
		fullPathCopy = fullPathCopy.substr(0, extPos);
	}
	else
	{
		extension.clear();
	}

	pathPos = fullPathCopy.find_last_of('/');
	if (pathPos != String::npos)
	{
		fileName = fullPathCopy.substr(pathPos + 1);
		pathName = fullPathCopy.substr(0, pathPos + 1);
	}
	else
	{
		fileName = fullPathCopy;
		pathName.clear();
	}
}

String PathUtil::GetFileExtension( const String& fullPath )
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return extension;
}

String PathUtil::GetFileName( const String& fullPath )
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return file;
}

String PathUtil::GetFileNameAndExtension( const String& fullPath )
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return file + extension;
}

String PathUtil::GetPath( const String& fullPath )
{
	String path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return path;
}

String PathUtil::GetParentPath( const String& pathName )
{
	size_t pos = RemoveTrailingSlash(pathName).find_last_of('/');
	if (pos != String::npos)
	{
		return pathName.substr(0, pos);
	}
	else
	{
		return String();
	}
}

String PathUtil::AddTrailingSlash( const String& pathName )
{
	String fullPathCopy = GetInternalPath(pathName);
	if (!fullPathCopy.empty() && fullPathCopy.back() != '/')
	{
		fullPathCopy.push_back('/');
	}
	return fullPathCopy;
}

String PathUtil::RemoveTrailingSlash( const String& pathName )
{
	String fullPathCopy = GetInternalPath(pathName);
	if (!fullPathCopy.empty() && fullPathCopy.back() == '/')
	{
		fullPathCopy.resize(fullPathCopy.length() - 1);
	}
	return fullPathCopy;
}

String PathUtil::GetInternalPath(const String& fullPath)
{
	String fullPathCopy = fullPath;

	// Replace \ with / first
	std::replace( fullPathCopy.begin(), fullPathCopy.end(), '\\', '/' );

	return fullPathCopy;
}

}