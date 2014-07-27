#include "OISInputModule.h"

namespace RcEngine {

const static std::string ModuleName = "OIS Input System";

OISInputModule::OISInputModule()
	: mInputSystem(nullptr)
{

}

OISInputModule::~OISInputModule()
{

}

const String& OISInputModule::GetName() const
{
	return ModuleName;
}

void OISInputModule::Initialise()
{
	mInputSystem = new OISInputSystem();
}

void OISInputModule::Shutdown()
{
	delete mInputSystem;
	mInputSystem = nullptr;
}

InputSystem* OISInputModule::GetInputSystem()
{
	return mInputSystem;
}

}
