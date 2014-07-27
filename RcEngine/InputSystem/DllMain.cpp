#include "OISInputModule.h"

extern "C" void _InputExport dllStartPlugin(RcEngine::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::OISInputModule();
}

extern "C" void _InputExport dllStopPlugin(RcEngine::IModule** pMoudle) 
{
	delete (*pMoudle);
}
