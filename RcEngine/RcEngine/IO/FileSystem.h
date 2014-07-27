#ifndef StreamWriter_h__
#define StreamWriter_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine {

class _ApiExport FileSystem : public Singleton<FileSystem>  
{
public:
	FileSystem();
	~FileSystem();

	void RegisterPath(const String& pathName, const String& group);

	String GetCurrentDir() const;
	bool SetCurrentDir(const String& pathName);


	/**
	 * Check if a path is allowed to be accessed. If no paths are registered, all are allowed.
	 */
	bool CheckAccess(const String& pathName);

	bool FileExits(const String& fileName);
	bool DirExits(const String& pathName);


	bool Exits(const String& name, const String& group="General");
	String Locate(const String& file, const String& group="General");
	shared_ptr<Stream> OpenStream(const String& file, const String& group="General");

private:
	void ScanDirInternal(vector<String>& result, String path, const String& startPath,
		const String& filter, unsigned flags, bool recursive);

private:
	unordered_set<String> mAllowedPaths;
	unordered_map<String, vector<String> > mResouceGroups;
	
};

} //Namespace RcEngine

#endif // StreamWriter_h__