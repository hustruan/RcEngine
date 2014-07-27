#ifndef InputEvent_h__
#define InputEvent_h__

#include <Core/Prerequisites.h>
#include <Core/StringHash.h>

namespace RcEngine {

struct InputEventType
{
	enum
	{
		None = 0,
		
		KeyDown,
		KeyUp,
		Char,

		MouseButtonDown,
		MouseButtonUp,

		MouseMove,
		MouseWheel,
		
	};

	enum MouseButtonsMask
	{
		NoButton          = 0x00000000,
		LeftButtonMask    = 0x00000001,
		RightButtonMask   = 0x00000002,
		MiddleButtonMask  = 0x00000004,
		XButton1Mask      = 0x00000008,
		XButton2Mask      = 0x00000010
	};

	enum KeyboardModifier
	{
		NoModifier			= 0x00000000,	// No modifier key is pressed.
		ShiftModifier		= 0x02000000,	//	A Shift key on the keyboard is pressed.
		ControlModifier		= 0x04000000,	//	A Ctrl key on the keyboard is pressed.
		AltModifier			= 0x08000000,	//	An Alt key on the keyboard is pressed.
		MetaModifier		= 0x10000000,	//	A Meta key on the keyboard is pressed.
		KeypadModifier		= 0x20000000,	//	A keypad button is pressed.
		GroupSwitchModifier	= 0x40000000,	//	X11 only. A Mode_switch key on the keyboard is pressed.
	};
};

struct MouseButtonEvent
{
    uint8_t type;
	uint32_t button;
    int16_t x, y;
};

struct MouseMotionEvent
{
	 uint8_t type;
	 uint32_t buttons;
	 int16_t x, y;
};

struct MouseWheelEvent
{
	uint8_t type;
	int16_t wheel;
	int16_t x, y;
};

struct KeyboardEvent
{
	uint8_t type;
	uint16_t key;
	uint32_t Modifier;
};

struct CharEvent
{
	uint8_t type;
	uint16_t unicode;
	uint32_t Modifier;
};

union InputEvent
{
	uint8_t EventType;
	MouseButtonEvent MouseButton;
	MouseMotionEvent MouseMove;
	MouseWheelEvent MouseWheel;
	KeyboardEvent Key;
	CharEvent Char;
};

}

#endif // InputEvent_h__
