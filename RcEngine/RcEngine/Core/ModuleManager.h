#ifndef ModuleManager_h__
#define ModuleManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine{

class DynLib;
class IModule;
class ModuleManager;

enum ModuleType
{
	MT_Render_OpengGL = 0,
	MT_Render_D3D11,
	MT_Count
};

//! Module names for plugin import
const String ModuleNames[MT_Count] =
{
#ifdef _DEBUG
	"OpenGLRenderSystem_d",
	"Direct3D11RenderSystem_d",
#else
	"OpenGLRenderSystem",
	"Direct3D11RenderSystem",
#endif
};

class _ApiExport ModuleInfo
{
public:
	ModuleInfo() : DynLib(nullptr) {}

	String		ModuleName;
	ModuleType	ModuleType;
	IModule*	ModuleSystem;

private:
	DynLib*		DynLib;

	// Can't copy
	ModuleInfo &operator = (const ModuleInfo &other);
	ModuleInfo(const ModuleInfo &other);

	friend class ModuleManager;
};

class _ApiExport ModuleManager : public Singleton<ModuleManager>
{
public:
	ModuleManager(void);
	~ModuleManager(void);

	bool HasMoudle(const String& name);
	bool HasModule(ModuleType modType);

	bool Load(const String& name, ModuleType modType);
	bool Load(ModuleType modType);

	bool Unload(const String& name);
	bool Unload(ModuleType modType);

	bool LoadAll();
	bool UnloadAll();

	IModule* GetModuleByName(const String &strModule);
	IModule* GetMoudleByType(ModuleType modType);

private:
	ModuleInfo* GetMoudleInfoByName(const String &strModule);
	ModuleInfo* GetMoudleInfoByType(ModuleType modType);
	void Unload(ModuleInfo* module);

private:
	std::vector<ModuleInfo*> mMoudles;
};



} // Namespace RcEngine

#endif // ModuleManager_h__