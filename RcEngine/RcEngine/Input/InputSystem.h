#ifndef InputSystem_h__
#define InputSystem_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <Input/InputEvent.h>
#include <Math/Vector.h>

namespace RcEngine{

typedef std::pair<uint32_t, uint32_t> InputAction;
typedef std::pair<uint32_t, uint32_t> InputState;
typedef std::pair<uint32_t, uint32_t> InputRange;

enum KeyCode
{
	KC_UnAssigned    				= 0x00,
	KC_Escape                       = 0x01,
	KC_1                            = 0x02,
	KC_2                            = 0x03,
	KC_3                            = 0x04,
	KC_4                            = 0x05,
	KC_5                            = 0x06,
	KC_6                            = 0x07,
	KC_7                            = 0x08,
	KC_8                            = 0x09,
	KC_9                            = 0x0A,
	KC_0                            = 0x0B,
	KC_Minus                        = 0x0C,         // - on main keyboard
	KC_Equals                       = 0x0D,
	KC_BackSpace					= 0x0E,         // backspace
	KC_Tab                          = 0x0F,
	KC_Q                            = 0x10,
	KC_W                            = 0x11,
	KC_E                            = 0x12,
	KC_R                            = 0x13,
	KC_T                            = 0x14,
	KC_Y                            = 0x15,
	KC_U                            = 0x16,
	KC_I                            = 0x17,
	KC_O                            = 0x18,
	KC_P                            = 0x19,
	KC_LeftBracket					= 0x1A,
	KC_RightBracket				    = 0x1B,
	KC_Enter                        = 0x1C,         // Enter on main keyboard
	KC_LeftCtrl                     = 0x1D,
	KC_A                            = 0x1E,
	KC_S                            = 0x1F,
	KC_D                            = 0x20,
	KC_F                            = 0x21,
	KC_G                            = 0x22,
	KC_H                            = 0x23,
	KC_J                            = 0x24,
	KC_K                            = 0x25,
	KC_L                            = 0x26,
	KC_Semicolon					= 0x27,
	KC_Apostrophe				    = 0x28,
	KC_Grave                        = 0x29,         // accent grave
	KC_LeftShift					= 0x2A,
	KC_BackSlash					= 0x2B,
	KC_Z                            = 0x2C,
	KC_X                            = 0x2D,
	KC_C                            = 0x2E,
	KC_V                            = 0x2F,
	KC_B                            = 0x30,
	KC_N                            = 0x31,
	KC_M                            = 0x32,
	KC_Comma                        = 0x33,
	KC_Period                       = 0x34,         // . on main keyboard
	KC_Slash                        = 0x35,         // / on main keyboard
	KC_RightShift					= 0x36,
	KC_NumPadStar					= 0x37,         // * on numeric keypad
	KC_LeftAlt                      = 0x38,
	KC_Space                        = 0x39,
	KC_CapsLock                     = 0x3A,
	KC_F1                           = 0x3B,
	KC_F2                           = 0x3C,
	KC_F3                           = 0x3D,
	KC_F4                           = 0x3E,
	KC_F5                           = 0x3F,
	KC_F6                           = 0x40,
	KC_F7                           = 0x41,
	KC_F8                           = 0x42,
	KC_F9                           = 0x43,
	KC_F10                          = 0x44,
	KC_NumLock                      = 0x45,
	KC_ScrollLock					= 0x46,
	KC_NumPad7                      = 0x47,
	KC_NumPad8                      = 0x48,
	KC_NumPad9                      = 0x49,
	KC_NumPadMinus					= 0x4A,         // - on numeric keypad
	KC_NumPad4                      = 0x4B,
	KC_NumPad5                      = 0x4C,
	KC_NumPad6                      = 0x4D,
	KC_NumPadPlus					= 0x4E,         // + on numeric keypad
	KC_NumPad1                      = 0x4F,
	KC_NumPad2                      = 0x50,
	KC_NumPad3                      = 0x51,
	KC_NumPad0                      = 0x52,
	KC_NumPadPeriod					= 0x53,         // . on numeric keypad
	KC_OEM_102                      = 0x56,         // <> or \| on RT 102-key keyboard (Non-U.S.)
	KC_F11                          = 0x57,
	KC_F12                          = 0x58,
	KC_F13                          = 0x64,         //                     (NEC PC98)
	KC_F14                          = 0x65,         //                     (NEC PC98)
	KC_F15                          = 0x66,         //                     (NEC PC98)
	KC_Kana                         = 0x70,         // (Japanese keyboard)
	KC_ABNT_C1                      = 0x73,         // /? on Brazilian keyboard
	KC_Convert                      = 0x79,         // (Japanese keyboard)
	KC_NoConvert					= 0x7B,         // (Japanese keyboard)
	KC_Yen                          = 0x7D,         // (Japanese keyboard)
	KC_ABNT_C2                      = 0x7E,         // Numpad . on Brazilian keyboard
	KC_NumPadEquals					= 0x8D,         // = on numeric keypad (NEC PC98)
	KC_PrevTrack					= 0x90,         // Previous Track (DKC_CIRCUMFLEX on Japanese keyboard)
	KC_AT                           = 0x91,         //                     (NEC PC98)
	KC_Colon                        = 0x92,         //                     (NEC PC98)
	KC_Underline					= 0x93,         //                     (NEC PC98)
	KC_Kanji                        = 0x94,         // (Japanese keyboard)
	KC_Stop                         = 0x95,         //                     (NEC PC98)
	KC_AX                           = 0x96,         //                     (Japan AX)
	KC_Unlabeled					= 0x97,         //                        (J3100)
	KC_NextTrack					= 0x99,         // Next Track
	KC_NumPadEnter					= 0x9C,         // Enter on numeric keypad
	KC_RightCtrl					= 0x9D,
	KC_Mute                         = 0xA0,         // Mute
	KC_Calculator					= 0xA1,         // Calculator
	KC_PlayPause					= 0xA2,         // Play / Pause
	KC_MediaStop					= 0xA4,         // Media Stop
	KC_VolumeDown					= 0xAE,         // Volume -
	KC_VolumeUp                     = 0xB0,         // Volume +
	KC_WebHome                      = 0xB2,         // Web home
	KC_NumPadComma					= 0xB3,         // , on numeric keypad (NEC PC98)
	KC_NumPadSlash					= 0xB5,         // / on numeric keypad
	KC_SysRQ                        = 0xB7,
	KC_RightAlt                     = 0xB8,         // right Alt
	KC_Pause                        = 0xC5,         // Pause
	KC_Home                         = 0xC7,         // Home on arrow keypad
	KC_UpArrow                      = 0xC8,         // UpArrow on arrow keypad
	KC_PageUp                       = 0xC9,         // PgUp on arrow keypad
	KC_LeftArrow				    = 0xCB,         // LeftArrow on arrow keypad
	KC_RightArrow					= 0xCD,         // RightArrow on arrow keypad
	KC_End                          = 0xCF,         // End on arrow keypad
	KC_DownArrow					= 0xD0,         // DownArrow on arrow keypad
	KC_PageDown                     = 0xD1,         // PgDn on arrow keypad
	KC_Insert                       = 0xD2,         // Insert on arrow keypad
	KC_Delete                       = 0xD3,         // Delete on arrow keypad
	KC_LeftWin                      = 0xDB,         // Left Windows key
	KC_RightWin                     = 0xDC,         // Right Windows key
	KC_Apps                         = 0xDD,         // AppMenu key
	KC_Power                        = 0xDE,         // System Power
	KC_Sleep                        = 0xDF,         // System Sleep
	KC_Wake                         = 0xE3,         // System Wake
	KC_WebSearch					= 0xE5,         // Web Search
	KC_WebFavorites					= 0xE6,         // Web Favorites
	KC_WebRefresh				    = 0xE7,         // Web Refresh
	KC_WebStop                      = 0xE8,         // Web Stop
	KC_WebForward					= 0xE9,         // Web Forward
	KC_WebBack                      = 0xEA,         // Web Back
	KC_MyComputer					= 0xEB,         // My Computer
	KC_Mail                         = 0xEC,         // Mail
	KC_MediaSelect					= 0xED,         // Media Select
};

enum MouseCode
{
	MS_X                            = 0x100,
	MS_Y                            = 0x101,
	MS_Z                            = 0x102,
	MS_XDelta                       = 0x103,
	MS_YDelta                       = 0x104,
	MS_ZDelta                       = 0x105,
	MS_LeftButton                   = 0x106,
	MS_RightButton                  = 0x107,
	MS_MiddleButton                 = 0x108,
	MS_Button3                      = 0x109,
	MS_Button4                      = 0x10A,
	MS_Button5                      = 0x10B,
	MS_Button6                      = 0x10C,
	MS_Button7                      = 0x10D,
};

class _ApiExport InputSystem : public Singleton<InputSystem>
{
public:
	InputSystem();
	~InputSystem();

	void ClearStates();

	void FireEvent(const InputEvent& event);
	bool PollEvent(InputEvent* event);

	// Call before Collect OS Events
	void BeginEvents();

	// Call after Collect OS Events
	void EndEvents();

	void Dispatch(float deltaTime);

	/**
	 * Mouse
	 */
	int32_t GetMouseMoveX() const					{ return mMouseMove.X(); }
	int32_t GetMouseMoveY() const					{ return mMouseMove.Y(); }
	const Vector<int32_t, 2>& GetMouseMove() const  { return mMouseMove; }

	int32_t GetMousePosX() const					{ return mMousePos.X(); }
	int32_t GetMousePoxY() const					{ return mMousePos.Y(); }
	const Vector<int32_t, 2>& GetMousePos() const   { return mMousePos; }

	bool MouseButtonDown(MouseCode button) const;
	bool MouseButtonPress(MouseCode button) const;
	bool MouseButtonRelease(MouseCode button) const;

	/**
	 * Keyboard
	 */
	bool KeyDown(KeyCode key) const;
	bool KeyPress(KeyCode key) const;
	bool KeyRelease(KeyCode key) const;

	// Force mouse cursor to window center
	void ForceCursorToCenter();

	//-----------------------------------------------------------------------------------
	void AddActionHandler(uint32_t action, InputActionHandler handler);
	void AddStateHandler(uint32_t state, InputStateHandler handler);
	void AddRangeHandler(uint32_t range, InputRangeHandler handler);

	template<typename ForwardIterator>
	void AddAction(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mActions.insert(InputAction((*iter).first, (*iter).second));
		}
	}

	template<typename ForwardIterator>
	void AddState(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mStates.insert(InputState((*iter).first, (*iter).second));
		}
	}

	template<typename ForwardIterator>
	void AddRange(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mRanges.insert(InputRange((*iter).first, (*iter).second));
		}
	}

	bool HasAction(uint32_t action) const;
	bool HasState(uint32_t state) const;
	bool HasRange(uint32_t range) const;

private:
	void DispatchActions(float dt) const;
	void DispatchStates(float dt) const;
	void DispatchRanges(float dt) const;

private:
	Vector<int32_t, 2>  mMousePos;
	Vector<int32_t, 2>  mLastMousePos;
	Vector<int32_t, 2>  mMouseMove;

	std::vector<bool>     mKeyState;
	std::vector<int32_t>  mJustPressed;
	std::vector<int32_t>  mjustReleased;
	
	std::queue<InputEvent> mEventQueue;

	// Raw input to Action
	std::unordered_map<uint32_t, uint32_t> mActions;
	std::unordered_map<uint32_t, InputActionHandler> mActionHandlers;

	// Raw input to State 
	std::unordered_map<uint32_t, uint32_t> mStates;
	std::unordered_map<uint32_t, InputStateHandler> mStateHandlers;

	// Raw input to Range
	std::unordered_map<uint32_t, uint32_t> mRanges;
	std::unordered_map<uint32_t, InputRangeHandler> mRangeHandlers;
};

} // Namespace RcEngine


#endif // InputSystem_h__