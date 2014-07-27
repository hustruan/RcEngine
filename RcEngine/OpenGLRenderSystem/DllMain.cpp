#include "OpenGLModule.h"

extern "C" void _OpenGLExport dllStartPlugin(RcEngine::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::OpenGLModule();
}

extern "C" void _OpenGLExport dllStopPlugin(RcEngine::IModule** pMoudle) 
{
	delete (*pMoudle);
}
