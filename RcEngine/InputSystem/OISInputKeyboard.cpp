#include "OISInputKeyboard.h"
#include "OISInputSystem.h"

namespace RcEngine {

static String Name = String("OIS Keyboard");

OISInputKeyboard::OISInputKeyboard( InputSystem* inputSystem )
	: Keyboard(inputSystem)
{
	assert(inputSystem);
	OIS::InputManager* inputManager = (static_cast<OISInputSystem*>(mInputSystem))->GetInputManager();
	mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
}

OISInputKeyboard::~OISInputKeyboard()
{

}

const String& OISInputKeyboard::GetName() const
{
	return Name;
}

void OISInputKeyboard::Update( )
{
	mKeyboard->capture();
	mIndex = !mIndex;
	mKeyboard->copyKeyStates((char*)mKeyBuffer[mIndex]);
}

}