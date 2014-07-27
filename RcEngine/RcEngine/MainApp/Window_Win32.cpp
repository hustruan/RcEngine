#include <MainApp/Window.h>
#include <MainApp/AppSettings.h>
#include <Core/Utility.h>
#include <Core/Exception.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>

#if defined(RcWindows)

#include <windowsx.h>

namespace {

using namespace RcEngine;

static KeyCode VKMapping[256];
static bool VKMappingInitialized = false;

static void InitVKMapping()
{
	VKMapping[VK_BACK] = KC_BackSpace;
	VKMapping[VK_TAB]  = KC_Tab;

	VKMapping[VK_LWIN]  = KC_LeftWin;
	VKMapping[VK_RWIN]  = KC_RightWin;
	VKMapping[VK_APPS]  = KC_Apps;
	VKMapping[VK_SLEEP] = KC_Sleep;

	VKMapping[VK_RETURN]  = KC_Enter;
	VKMapping[VK_LSHIFT]  = KC_LeftShift;
	VKMapping[VK_RSHIFT]  = KC_RightShift;

	VKMapping[VK_RCONTROL]  = KC_RightCtrl;
	VKMapping[VK_LCONTROL]  = KC_LeftCtrl;

	VKMapping[VK_PAUSE]     = KC_Pause;
	VKMapping[VK_ESCAPE]    = KC_Escape;
	VKMapping[VK_SPACE]     = KC_Space;
	VKMapping[VK_KANA]		= KC_Kana;
	VKMapping[VK_CAPITAL]	= KC_CapsLock;
	VKMapping[VK_KANJI]		= KC_Kanji;

	VKMapping[VK_LEFT]		= KC_LeftArrow;
	VKMapping[VK_RIGHT]		= KC_RightArrow;
	VKMapping[VK_UP]		= KC_UpArrow;
	VKMapping[VK_DOWN]		= KC_DownArrow;
	VKMapping[VK_INSERT]    = KC_Insert;
	VKMapping[VK_HOME]		= KC_Home;
	VKMapping[VK_END]       = KC_End;
	VKMapping[VK_DELETE]	= KC_Delete;
	VKMapping[VK_NEXT]      = KC_PageDown;
	VKMapping[VK_PRIOR]     = KC_PageUp;
	VKMapping[VK_SCROLL]    = KC_ScrollLock;
	VKMapping[VK_NUMLOCK]   = KC_NumLock;

	VKMapping[VK_NUMPAD0] = KC_NumPad0;
	VKMapping[VK_NUMPAD1] = KC_NumPad1;
	VKMapping[VK_NUMPAD2] = KC_NumPad2;
	VKMapping[VK_NUMPAD3] = KC_NumPad3;
	VKMapping[VK_NUMPAD4] = KC_NumPad4;
	VKMapping[VK_NUMPAD5] = KC_NumPad5;
	VKMapping[VK_NUMPAD6] = KC_NumPad6;
	VKMapping[VK_NUMPAD7] = KC_NumPad7;
	VKMapping[VK_NUMPAD8] = KC_NumPad8;
	VKMapping[VK_NUMPAD9] = KC_NumPad9;

	VKMapping[VK_MULTIPLY] = KC_NumPadStar;
	VKMapping[VK_ADD]      = KC_NumPadPlus;
	VKMapping[VK_SUBTRACT] = KC_NumPadMinus;
	VKMapping[VK_DECIMAL]  =KC_NumPadPeriod;
	VKMapping[VK_DIVIDE]   = KC_NumPadSlash;

	VKMapping[VK_F1] = KC_F1;
	VKMapping[VK_F2] = KC_F2;
	VKMapping[VK_F3] = KC_F3;
	VKMapping[VK_F4] = KC_F4;
	VKMapping[VK_F5] = KC_F5;
	VKMapping[VK_F6] = KC_F6;
	VKMapping[VK_F7] = KC_F7;
	VKMapping[VK_F8] = KC_F8;
	VKMapping[VK_F9] = KC_F9;
	VKMapping[VK_F10] = KC_F10;
	VKMapping[VK_F11] = KC_F11;
	VKMapping[VK_F12] = KC_F12;

	VKMapping['0'] = KC_0;
	VKMapping['1'] = KC_1;
	VKMapping['2'] = KC_2;
	VKMapping['3'] = KC_3;
	VKMapping['4'] = KC_4;
	VKMapping['5'] = KC_5;
	VKMapping['6'] = KC_6;
	VKMapping['7'] = KC_7;
	VKMapping['8'] = KC_8;
	VKMapping['9'] = KC_9;
	VKMapping['9'] = KC_9;

	VKMapping['A'] = KC_A;
	VKMapping['B'] = KC_B;
	VKMapping['C'] = KC_C;
	VKMapping['D'] = KC_D;
	VKMapping['E'] = KC_E;
	VKMapping['F'] = KC_F;
	VKMapping['G'] = KC_G;
	VKMapping['H'] = KC_H;
	VKMapping['I'] = KC_I;
	VKMapping['J'] = KC_J;
	VKMapping['K'] = KC_K;
	VKMapping['L'] = KC_L;
	VKMapping['M'] = KC_M;
	VKMapping['N'] = KC_N;
	VKMapping['O'] = KC_O;
	VKMapping['P'] = KC_P;
	VKMapping['Q'] = KC_Q;
	VKMapping['R'] = KC_R;
	VKMapping['S'] = KC_S;
	VKMapping['T'] = KC_T;
	VKMapping['U'] = KC_U;
	VKMapping['V'] = KC_V;
	VKMapping['W'] = KC_W;
	VKMapping['X'] = KC_X;
	VKMapping['Y'] = KC_Y;
	VKMapping['Z'] = KC_Z;

	VKMapping[VK_OEM_1] = KC_Semicolon;
	VKMapping[VK_OEM_2] = KC_Slash;
	VKMapping[VK_OEM_3] = KC_Grave;
	VKMapping[VK_OEM_4] = KC_LeftBracket;
	VKMapping[VK_OEM_5] = KC_BackSlash;
	VKMapping[VK_OEM_6] = KC_RightBracket;
	VKMapping[VK_OEM_7] = KC_Apostrophe;

	VKMapping[VK_OEM_COMMA]   = KC_Comma;
	VKMapping[VK_OEM_PERIOD]  = KC_Period;
	VKMapping[VK_OEM_PLUS]    = KC_Equals;

	VKMapping[VK_VOLUME_DOWN]		  = KC_VolumeDown;
	VKMapping[VK_VOLUME_UP]			  = KC_VolumeUp;
	VKMapping[VK_MEDIA_NEXT_TRACK]    = KC_NextTrack;
	VKMapping[VK_MEDIA_PREV_TRACK]    = KC_PrevTrack;
	VKMapping[VK_MEDIA_PLAY_PAUSE]    = KC_PlayPause;
	VKMapping[VK_MEDIA_STOP]		  = KC_MediaStop;
	VKMapping[VK_VOLUME_MUTE]		  = KC_Mute;
	VKMapping[VK_LAUNCH_MAIL]		  = KC_Mail;
	VKMapping[VK_LAUNCH_MEDIA_SELECT] = KC_MediaSelect;

	VKMapping[VK_BROWSER_HOME]		  = KC_WebHome;
	VKMapping[VK_BROWSER_FORWARD]	  = KC_WebBack;
	VKMapping[VK_BROWSER_BACK]		  = KC_WebForward;
	VKMapping[VK_BROWSER_REFRESH]	  = KC_WebRefresh;
	VKMapping[VK_BROWSER_STOP]		  = KC_WebStop;
	VKMapping[VK_BROWSER_SEARCH]	  = KC_WebSearch;
	VKMapping[VK_BROWSER_FAVORITES]	  = KC_WebFavorites;
}

static void MakeKeyEvent(int vkCode, int lParam, InputEvent& e)
{
	bool extended = (lParam >> 24) & 0x01;

	if (vkCode == VK_SHIFT)
	{
		if (e.EventType == InputEventType::KeyDown)
		{
			e.Key.key = KC_LeftShift;

			if( GetKeyState(VK_RSHIFT) & 0x8000)
				e.Key.key = KC_RightShift;
		}
		else
		{
			e.Key.key = KC_LeftShift;

			if (InputSystem::GetSingleton().KeyDown(KC_RightShift))
				e.Key.key = KC_RightShift;
		}	
	}
	else if (vkCode == VK_CONTROL)
	{
		e.Key.key = extended ? KC_RightCtrl : KC_LeftCtrl;
	}
	else if (vkCode == VK_MENU)
	{
		e.Key.key = extended ? KC_RightAlt : KC_LeftAlt;
	}
	else
	{
		e.Key.key = VKMapping[vkCode];
	}
}

}

//////////////////////////////////////////////////////////////////////////
namespace RcEngine {

LRESULT CALLBACK Window::WndProcStatic( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (msWindow)
	{
		return msWindow->WndProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

Window::Window(const ApplicationSettings& settings )
	: mInSizeMove(false),
	  mMaximized(false),
	  mMinimized(false),
	  mMouseVisible(true)
{
	msWindow = this;
	mhInstance	= GetModuleHandle(NULL);

	// Init input mappings
	mInputSystem = InputSystem::GetSingletonPtr();	
	if (!mInputSystem)
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Input System Not Initialized!", "Window::Window");

	if (!VKMappingInitialized)
	{
		InitVKMapping();
		VKMappingInitialized = true;
	}

	// Register wnd class
	WString wtitle = StringToWString(settings.AppTitle);

	WNDCLASSEX	wcex;					
	wcex.cbSize			    = sizeof(WNDCLASSEX);
	wcex.style              = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc        = (Window::WndProcStatic);
	wcex.cbClsExtra         = 0;
	wcex.cbWndExtra         = 0;
	wcex.hInstance          = mhInstance;
	wcex.hIcon              = NULL;
	wcex.hCursor            = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground      = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName       = NULL;
	wcex.lpszClassName      = wtitle.c_str();
	wcex.hIconSm            = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,TEXT("Failed To Register The Window Class."), TEXT("ERROR"),  MB_OK|MB_ICONEXCLAMATION);
	}

	// Create window
	DWORD		dwExStyle;												
	DWORD		dwStyle;											
								
	mFullscreen = settings.Fullscreen;

	if (settings.Fullscreen)									
	{
		dwExStyle=WS_EX_APPWINDOW;							
		dwStyle=WS_POPUP;										
		::ShowCursor(FALSE);									
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			
		dwStyle=WS_OVERLAPPEDWINDOW;						
	}

	RECT		rc;		
	rc.left	  = static_cast<LONG>(settings.Left);
	rc.top	  = static_cast<LONG>(settings.Top);	
	rc.right  = static_cast<LONG>(settings.Width + rc.left);	
	rc.bottom = static_cast<LONG>(settings.Height + rc.top);
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);		


	if (!(mhWnd = CreateWindowEx(dwExStyle,							    // Extended Style For The Window
		wtitle.c_str(),													// Class Name
		wtitle.c_str(),						                            // Window Title
		dwStyle |							                            // Defined Window Style
		WS_CLIPSIBLINGS |					                            // Required Window Style
		WS_CLIPCHILDREN,					                            // Required Window Style
		rc.left, rc.top,	                                            // Window Position
		rc.right-rc.left,	                                            // Calculate Window Width
		rc.bottom-rc.top,	                                            // Calculate Window Height
		NULL,								                            // No Parent Window
		NULL,								                            // No Menu
		mhInstance,							                            // Instance
		NULL)))								                            // Dont Pass Anything To WM_CREATE
	{
		MessageBox(NULL,TEXT("Window Creation Error."),TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);
	}


	GetClientRect(mhWnd, &rc);
	mLeft = rc.left;
	mTop = rc.top;
	mWidth = rc.right - rc.left;
	mHeight = rc.bottom - rc.top;
}


Window::~Window(void)
{
	PaintEvent.clear();
	ResumeEvent.clear();
	SuspendEvent.clear();
	ApplicationActivatedEvent.clear();
	ApplicationDeactivatedEvent.clear();
	WindowClose.clear();
	UserResizedEvent.clear();
}


LRESULT Window::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int mouseButtonsMask = 0;
	static bool mouseInsideClient = false;
	static TRACKMOUSEEVENT trackMouse;

	switch (uMsg)
	{
	case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mMinimized = true;
				mMaximized = false;
				OnSuspend();
			}
			else if(wParam == SIZE_MAXIMIZED)
			{
				mMinimized = false;
				mMaximized = true;
				OnUserResized();
			}
			else
			{
				if (mMinimized)
					OnResume();

				mMinimized = false;
				mMaximized = false;

				if(!mInSizeMove)
					OnUserResized();
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonDown;
			e.MouseButton.button = MS_LeftButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;
	
	case WM_LBUTTONUP:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonUp;
			e.MouseButton.button = MS_LeftButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonDown;
			e.MouseButton.button = MS_RightButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;
	
	case WM_RBUTTONUP:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonUp;
			e.MouseButton.button = MS_RightButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_MBUTTONDOWN:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonDown;
			e.MouseButton.button = MS_MiddleButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_MBUTTONUP:
		{
			InputEvent e;
			e.MouseButton.type = InputEventType::MouseButtonUp;
			e.MouseButton.button = MS_MiddleButton;
			e.MouseButton.x = GET_X_LPARAM(lParam); 
			e.MouseButton.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_MOUSEWHEEL:
		{
			InputEvent e;
			e.MouseWheel.type = InputEventType::MouseWheel;
			e.MouseWheel.wheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			e.MouseWheel.x = GET_X_LPARAM(lParam); 
			e.MouseWheel.y = GET_Y_LPARAM(lParam); 
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_MOUSEMOVE:
		{
			// For mouse leave client track
			if (!mouseInsideClient)
			{
				trackMouse.cbSize = sizeof(TRACKMOUSEEVENT);
				trackMouse.dwFlags = TME_LEAVE | TME_HOVER;
				trackMouse.dwHoverTime = 10;
				trackMouse.hwndTrack = hWnd;
				TrackMouseEvent(&trackMouse);
				mouseInsideClient = true;
			}

			InputEvent e;
			e.MouseMove.type = InputEventType::MouseMove;
			e.MouseMove.x = LOWORD( lParam );
			e.MouseMove.y = HIWORD( lParam );
	
			// According to WM_MOUSEMOVE on MSDN
			e.MouseMove.buttons = 0;		
			if( wParam & 0x0001 ) e.MouseMove.buttons |= InputEventType::LeftButtonMask;
			if( wParam & 0x0010 ) e.MouseMove.buttons |= InputEventType::MiddleButtonMask;
			if( wParam & 0x0002 ) e.MouseMove.buttons |= InputEventType::RightButtonMask;

			mInputSystem->FireEvent(e);
		}
		break;

	case WM_MOUSELEAVE:
		{
			mouseInsideClient = false;
			mInputSystem->ClearStates();
		}
		break;
	
	case WM_MOUSEHOVER:
		{
			//printf("WM_MOUSEHOVER\n");
		}
		break;	

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			InputEvent e;
			e.Key.type = InputEventType::KeyDown;
			MakeKeyEvent(wParam, lParam, e);
			mInputSystem->FireEvent(e);

			//printf("wParam = %d WM_KEYDOWN at clock:%d\n", wParam, clock()); 
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			InputEvent e;
			e.Key.type = InputEventType::KeyUp;
			MakeKeyEvent(wParam, lParam, e);
			mInputSystem->FireEvent(e);		
		}
		break;

	case WM_CHAR:
		{
			InputEvent e;
			e.Char.type = InputEventType::Char;
			e.Char.unicode = wParam;
			mInputSystem->FireEvent(e);
		}
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_ACTIVATEAPP:
		{
			if (wParam)
				OnApplicationActivated();
			else
				OnApplicationDeactivated();
		}
		break;

	case WM_ENTERSIZEMOVE:
		{
			OnSuspend();
			mInSizeMove = true;
			//OnUserResized();
		}

		break;

	case WM_EXITSIZEMOVE:
		{
			OnUserResized();
			mInSizeMove = false;
			OnResume();
		}
		break;

	case WM_DESTROY:
		{
			WString wtitle = StringToWString(mName);
			UnregisterClass(wtitle.c_str(), mhInstance);

			OnClose();

			PostQuitMessage(0);
		}	
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Window::SetTitle( const String& title )
{
	WString text = StringToWString(title);
	::SetWindowText(mhWnd, text.c_str());
}

void Window::UpdateWindowSize()
{
	RECT rect;
	::GetWindowRect(mhWnd, &rect);

	int32_t newLeft = rect.left;
	int32_t newTop = rect.top;

	if ((newLeft != mLeft) || (newTop != mTop))
	{
		mLeft = newLeft;
		mTop = newTop;
		Reposition(newLeft, newTop);
	}

	::GetClientRect(mhWnd, &rect);	

	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;
}

void Window::ForceMouseToCenter()
{
	POINT pt;
	pt.x = mWidth >> 1;
	pt.y = mHeight >> 1;

	::ClientToScreen(mhWnd, &pt);
	SetCursorPos(pt.x, pt.y);

	// Invisible mouse
	SetMouseVisible(false);
}

void Window::SetMousePosition(int32_t x, int32_t y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	::ClientToScreen(mhWnd, &pt);
	::SetCursorPos(pt.x, pt.y);
}

void Window::CollectOSEvents()
{
	MSG message;
	while ( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) ) 
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void Window::ShowWindow()
{
	::ShowWindow(mhWnd, SW_SHOWNORMAL);
	::UpdateWindow(mhWnd);
}

void Window::Reposition( int32_t left, int32_t top )
{
	if(mhWnd && !mFullscreen)
	{
		mLeft = left;
		mTop = top;
		::SetWindowPos(mhWnd, 0, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

}

#endif
