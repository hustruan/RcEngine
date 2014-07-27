#include "OISInputMouse.h"
#include "OISInputSystem.h"

namespace RcEngine {

String MouseName = String("OIS Mouse");

OIS::MouseButtonID MapToOIS(MouseCode button) 
{
	switch(button)
	{
	case MS_LeftButton:
		return OIS::MB_Left;
	case MS_RightButton:
		return OIS::MB_Right;
	case MS_MiddleButton:
		return OIS::MB_Middle;
	default:
		return OIS::MB_Left;
	}
}

OISInputMouse::OISInputMouse( InputSystem* inputSystem, uint32_t width, uint32_t height )
	: Mouse(inputSystem)
{
	assert(inputSystem);
	OIS::InputManager* inputManager = (static_cast<OISInputSystem*>(mInputSystem))->GetInputManager();
	mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, false));
			
	// feed window width, height
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

OISInputMouse::~OISInputMouse()
{

}
		
const String& OISInputMouse::GetName() const
{
	return MouseName;
}

void OISInputMouse::Update()
{
	mMouse->capture();
	const OIS::MouseState& mouseState = mMouse->getMouseState();
	mIndex =  !mIndex;
	mButtons[mIndex] = mouseState.buttons;
	mMouseMove.X() = mouseState.X.rel;
	mMouseMove.Y() = mouseState.Y.rel;
	mCurrentPosition.X() = mouseState.X.abs;
	mCurrentPosition.Y() = mouseState.Y.abs;
}

void OISInputMouse::SetWindowSize( uint32_t width, uint32_t height )
{
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

}