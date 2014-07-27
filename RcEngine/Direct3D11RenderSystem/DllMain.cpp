#include "D3D11Module.h"

extern "C" void _D3D11Export dllStartPlugin(RcEngine::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::D3D11Module();
}

extern "C" void _D3D11Export dllStopPlugin(RcEngine::IModule** pMoudle) 
{
	delete (*pMoudle);
}
