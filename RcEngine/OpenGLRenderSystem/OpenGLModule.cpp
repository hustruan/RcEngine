#include "OpenGLModule.h"
#include "OpenGLDevice.h"

namespace RcEngine {

const static std::string ModuleName = "GL RenderSystem";

OpenGLModule::OpenGLModule(void)
	: mRenderDevice(0)
{
}


OpenGLModule::~OpenGLModule(void)
{
}

const std::string& OpenGLModule::GetName() const
{
	return ModuleName;
}

void OpenGLModule::Initialise()
{
	mRenderDevice = new OpenGLDevice();
	mRenderDevice->CreateRenderWindow();
}

void OpenGLModule::Shutdown()
{
	delete mRenderDevice;
	mRenderDevice = 0;
}

}

