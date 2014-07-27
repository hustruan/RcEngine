#include "OISInputSystem.h"
#include "OISInputMouse.h"
#include "OISInputKeyboard.h"
#include "Core/Prerequisites.h"
#include "Core/Context.h"
#include "MainApp/Application.h"
#include "MainApp/Window.h"
#include "Core/Exception.h"

namespace RcEngine {

static String Name = String("OIS InputSystem");

OISInputSystem::OISInputSystem()
{
	// init ois input system
	OIS::ParamList pl;
	std::ostringstream windowHndStr;
	Window* mainWin = Context::GetSingleton().GetApplication().GetMainWindow();

	if (!mainWin)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Couldn't init input system before main window created",
			"OISInputSystem::OISInputSystem");
	}

	HWND hwnd = mainWin ->GetHwnd();
	size_t* p=(unsigned int*)&hwnd;
	windowHndStr <<  *p;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	mInputManager = OIS::InputManager::createInputSystem(pl);

	// Here we just create mouse and keyboard
	mKeyboard =  new OISInputKeyboard(this);
	mMouse =  new OISInputMouse(this, mainWin->GetWidth(), mainWin->GetHeight());
}

OISInputSystem::~OISInputSystem()
{

}

const String& OISInputSystem::Name() const
{
	return Name();
}

}