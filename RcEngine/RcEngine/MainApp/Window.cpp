#include <MainApp/Window.h>
#include <Core/Utility.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>

namespace RcEngine {

Window* Window::msWindow = 0;

void Window::OnUserResized()
{
	UpdateWindowSize();

	if(!UserResizedEvent.empty())
		UserResizedEvent();
}

void Window::OnSuspend()
{
	if(!SuspendEvent.empty())
		SuspendEvent();
}

void Window::OnResume()
{
	if(!ResumeEvent.empty())
		ResumeEvent();
}

void Window::OnApplicationActivated()
{
	if(!ApplicationActivatedEvent.empty())
		ApplicationActivatedEvent();
}

void Window::OnApplicationDeactivated()
{
	if(!ApplicationDeactivatedEvent.empty())
		ApplicationDeactivatedEvent();
}

void Window::OnPaint()
{
	if(!PaintEvent.empty())
		PaintEvent();
}

void Window::OnClose()
{
	if(!WindowClose.empty())
		WindowClose();
}

void Window::SetMouseVisible( bool visible )
{
	if (mMouseVisible != visible)
	{
		mMouseVisible = visible;
		ShowCursor(mMouseVisible);
	}
}


} // Namespace RcEngine