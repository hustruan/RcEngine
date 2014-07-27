#ifndef OpenGLModule_h__
#define OpenGLModule_h__

#include "OpenGLPrerequisites.h"
#include <Core/IModule.h>

namespace RcEngine {

class OpenGLDevice;

class _OpenGLExport OpenGLModule : public IModule
{
public:
	OpenGLModule(void);
	~OpenGLModule(void);

	const String& GetName() const ;
	void Initialise();
	void Shutdown();
	
private:
	OpenGLDevice* mRenderDevice;
};

}


#endif // OpenGLModule_h__