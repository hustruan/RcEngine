#ifndef Environment_h__
#define Environment_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine{

class Application;
class RenderDevice;
class RenderFactory;

class _ApiExport Environment : public Singleton<Environment>
{
public:
	Environment();
	~Environment();
	
	inline Application*  GetApplication() const			{ assert(mApplication); return mApplication; }
	inline RenderDevice*     GetRenderDevice() const			{ assert(mRenderDevice); return mRenderDevice; }
	inline RenderFactory*    GetRenderFactory() const			{ assert(mRenderFactory); return mRenderFactory; }
	inline SceneManager* GetSceneManager() const		{ assert(mRenderFactory); return mSceneManager; }

private:

	friend class Application;
	friend class RenderDevice;
	friend class RenderFactory;
	friend class SceneManager;

	Application* mApplication;
	RenderDevice* mRenderDevice;
	RenderFactory* mRenderFactory;
	SceneManager* mSceneManager;
};

} // Namespace RcEngine

#endif // Environment_h__
