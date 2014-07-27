#include "D3D11Module.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11Module::D3D11Module(void)
	: mRenderDevice(0)
{

}

D3D11Module::~D3D11Module(void)
{
}

const String& D3D11Module::GetName() const
{
	static String DeviceName("Direct3D 11");
	return DeviceName;
}


void D3D11Module::Initialise()
{
	mRenderDevice = new D3D11Device();
	mRenderDevice->CreateRenderWindow();
}

void D3D11Module::Shutdown()
{
	delete mRenderDevice;
	mRenderDevice = 0;
}

}