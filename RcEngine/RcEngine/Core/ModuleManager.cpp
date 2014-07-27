#include <Core/ModuleManager.h>
#include <Core/Utility.h>
#include <Core/IModule.h>
#include <Core/Exception.h>
#include <windows.h>

namespace RcEngine{

typedef void (*DLL_START_PLUGIN)(IModule** pModule);

class DynLib
{
public:
	DynLib(const String& name) : mName(name) {}
	~DynLib() { Unload(); }

	const String& GetName() const { return mName; }

	void Load()
	{
		String name = mName;
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";

		WString wname = StringToWString(name);
		m_hInst = (HINSTANCE)LoadLibraryEx(wname.c_str(), 0,  LOAD_WITH_ALTERED_SEARCH_PATH );
		if (!m_hInst)
		{
			LPVOID lpMsgBuf; 
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, 
				GetLastError(), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &lpMsgBuf, 
				0, 
				NULL 
				); 
			
			fprintf(stderr, "%s\n", (char*)lpMsgBuf);

			// Free the buffer.
			LocalFree( lpMsgBuf );


			String error = "Load " + name + " failed!";
			ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,  error, "DynLib::Load");
		}
	}

	void Unload()
	{
		if (m_hInst)
		{
			FreeLibrary(m_hInst);
			m_hInst = 0;
		}
	}

	void* GetSymbol( const String& strName ) const throw()
	{
		return (void*)GetProcAddress(m_hInst, strName.c_str());
	}

	std::string DynLibError()
	{

	}

private:
	String mName;
	HINSTANCE m_hInst;
};

//////////////////////////////////////////////////////////////////////////
ModuleManager::ModuleManager(void)
{
}

ModuleManager::~ModuleManager(void)
{
	UnloadAll();
}

bool ModuleManager::Load(const String& name, ModuleType modType)
{
	ModuleInfo* info = GetMoudleInfoByName(name);
	if(!info)
	{
		info = new ModuleInfo();
		info->ModuleName = name;
		info->ModuleType = modType;
		info->DynLib = new DynLib(name);
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)info->DynLib->GetSymbol("dllStartPlugin");
		pFunc(&info->ModuleSystem);

		info->ModuleSystem->Initialise();
		mMoudles.push_back(info);
	}
	return true;
}

bool ModuleManager::Load( ModuleType modType )
{
	ModuleInfo* info = GetMoudleInfoByType(modType);
	if(!info)
	{
		info = new ModuleInfo();
		info->ModuleName = ModuleNames[modType];
		info->ModuleType = modType;
		info->DynLib = new DynLib(ModuleNames[modType]);
		info->DynLib->Load();
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)info->DynLib->GetSymbol("dllStartPlugin");
		pFunc(&info->ModuleSystem);

		info->ModuleSystem->Initialise();
		mMoudles.push_back(info);
	}
	return true;
}

bool ModuleManager::Unload( const String& name )
{
	return true;
}

bool ModuleManager::Unload( ModuleType modType )
{
	return true;
}

void ModuleManager::Unload( ModuleInfo* module )
{
	assert(module);
	module->ModuleSystem->Shutdown();

	DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)module->DynLib->GetSymbol("dllStopPlugin");
	pFunc(&module->ModuleSystem);
	module->DynLib->Unload();
	delete module->DynLib;
}

bool ModuleManager::LoadAll()
{
	return true;
}

bool ModuleManager::UnloadAll()
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		Unload(moudleInfo);
		delete moudleInfo;
	}

	mMoudles.clear();
	return true;
}

bool ModuleManager::HasMoudle( const String& name )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleName == name)
			return true;
	}

	return false;
}

bool ModuleManager::HasModule( ModuleType modType )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleType == modType)
			return true;
	}

	return false;
}

IModule* ModuleManager::GetModuleByName( const String& moudleName )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleName == moudleName)
			return moudleInfo->ModuleSystem;
	}

	return nullptr;
}

IModule* ModuleManager::GetMoudleByType( ModuleType moudleTpye )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleType == moudleTpye)
			return moudleInfo->ModuleSystem;
	}

	return nullptr;
}

ModuleInfo* ModuleManager::GetMoudleInfoByName( const String &moudleName )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleName == moudleName)
			return moudleInfo;
	}

	return nullptr;
}

ModuleInfo* ModuleManager::GetMoudleInfoByType( ModuleType moudleTpye )
{
	for (ModuleInfo* moudleInfo : mMoudles)
	{
		if(moudleInfo->ModuleType == moudleTpye)
			return moudleInfo;
	}

	return nullptr;
}


} // Namespace RcEngine