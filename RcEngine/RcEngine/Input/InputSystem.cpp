#include <Input/InputSystem.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Environment.h>

namespace RcEngine{

InputSystem::InputSystem()
	: mMouseMove(0, 0), 
	  mMousePos(0, 0),
	  mKeyState(MS_Button7, false), // All for Key + Mouse + Joysticks,
	  mInitialMouseFoucus(false),
	  mMouseMoveWheel(0),
	  mMouseWheel(0),
	  mLastMouseWheel(0)
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::ClearStates()
{
	mMouseMove = mLastMousePos = mMousePos = Vector<int32_t, 2>(0, 0);
	mMouseMoveWheel = mMouseWheel = mLastMouseWheel = 0;
	std::fill(mKeyState.begin(), mKeyState.end(), false);
	mJustPressed.clear();
	mjustReleased.clear();
}

void InputSystem::FireEvent( const InputEvent& event )
{
	switch (event.EventType)
	{
	case InputEventType::KeyDown:
		{
			mKeyState[event.Key.key] = true;
			mJustPressed.push_back(event.Key.key);
		}
		break;

	case InputEventType::KeyUp:
		{
			mKeyState[event.Key.key] = false;
			mjustReleased.push_back(event.Key.key);
		}
		break;

	case InputEventType::MouseButtonDown:
		{
			mKeyState[event.MouseButton.button] = true;
			mJustPressed.push_back(event.MouseButton.button);
		}
		break;
	case InputEventType::MouseButtonUp:
		{
			mKeyState[event.MouseButton.button] = false;
			mjustReleased.push_back(event.MouseButton.button);
		}
		break;

	case InputEventType::MouseMove:
		{
			mMousePos.X() = event.MouseMove.x;
			mMousePos.Y() = event.MouseMove.y;
		}
		break;

	case InputEventType::MouseWheel:
		{
			//mMousePos.X() = event.MouseWheel.x;
			//mMousePos.Y() = event.MouseWheel.y;
			mMouseWheel += event.MouseWheel.wheel;
		}
		break;

	default:
		break;
	}

	mEventQueue.push(event);
}

bool InputSystem::PollEvent( InputEvent* event )
{
	if ( !mEventQueue.empty() )
	{
		*event = mEventQueue.front();
		mEventQueue.pop();
		return true;
	}

	return false;
}

void InputSystem::BeginEvents()
{
	mJustPressed.clear();
	mjustReleased.clear();
}

void InputSystem::EndEvents()
{
	if (!mInitialMouseFoucus)
	{
		mLastMousePos = mMousePos;
		mInitialMouseFoucus = true;
	}

	// Update Mouse Move
	mMouseMove = mMousePos - mLastMousePos;
	mLastMousePos = mMousePos;

	// Update Wheel Move
	mMouseMoveWheel = mMouseWheel - mLastMouseWheel;
	mLastMouseWheel = mMouseWheel;
}

bool InputSystem::KeyDown( KeyCode key ) const
{
	return mKeyState[key];
}

bool InputSystem::KeyPress( KeyCode key ) const
{
	for (size_t i = 0; i < mJustPressed.size(); ++i)
	{
		if (mJustPressed[i] == key)	
			return true;
	}

	return false;
}

bool InputSystem::KeyRelease( KeyCode key ) const
{
	for (size_t i = 0; i < mjustReleased.size(); ++i)
	{
		if (mjustReleased[i] == key)	
			return true;
	}

	return false;
}

bool InputSystem::MouseButtonDown( MouseCode button ) const
{
	return mKeyState[button];
}

bool InputSystem::MouseButtonPress( MouseCode button ) const
{
	for (size_t i = 0; i < mJustPressed.size(); ++i)
	{
		if (mJustPressed[i] == button)	
			return true;
	}

	return false;
}

bool InputSystem::MouseButtonRelease( MouseCode button ) const
{
	for (size_t i = 0; i < mjustReleased.size(); ++i)
	{
		if (mjustReleased[i] == button)	
			return true;
	}

	return false;
}

void InputSystem::DispatchActions(float delata) const
{
	for (auto& kv : mActions)
	{
		// KeyCode kv.first
		bool value = (std::find(mJustPressed.begin(), mJustPressed.end(), kv.first) != mJustPressed.end());

		// action kv.second
		auto foundHandler = mActionHandlers.find(kv.second);
		if ( foundHandler != mActionHandlers.end() && !foundHandler->second.empty())
			foundHandler->second(kv.second, value, delata);
	}

}

void InputSystem::DispatchStates(float deltaTime) const
{
	for (auto& kv : mStates)
	{
		bool value = mKeyState[kv.first];

		auto foundHandler = mStateHandlers.find(kv.second);
		if ( foundHandler != mStateHandlers.end() && !foundHandler->second.empty())
			foundHandler->second(kv.second, value, deltaTime);
	}
}

void InputSystem::DispatchRanges(float deltaTime) const
{
	for (auto iter = mRanges.begin(); iter != mRanges.end(); ++iter)
	{
		int32_t value;

		switch(iter->first)
		{
		case MS_X:
			value = mMousePos.X();
			break;
		case MS_Y:
			value = mMousePos.Y();
			break;
		case MS_XDelta:
			value = mMouseMove.X();
			break;
		case MS_YDelta:
			value = mMouseMove.Y();
			break;
		case MS_Z:

		default:
			assert(false);
		}

		auto foundHandler = mRangeHandlers.find(iter->second);
		if ( foundHandler != mRangeHandlers.end() && !foundHandler->second.empty())
			foundHandler->second(iter->second, value, deltaTime);
	}
}

void InputSystem::AddActionHandler( uint32_t action, InputActionHandler handler )
{
	if (HasAction(action))
	{
		mActionHandlers[action] = handler;
	}
}

void InputSystem::AddStateHandler(uint32_t state, InputStateHandler handler)
{
	if (HasState(state))
	{
		mStateHandlers[state] = handler;
	}
}

void InputSystem::AddRangeHandler(uint32_t range, InputRangeHandler handler)
{
	if (HasRange(range))
	{
		mRangeHandlers[range] = handler;
	}
}

bool InputSystem::HasAction( uint32_t action ) const
{
	for (auto iter = mActions.begin(); iter != mActions.end(); ++iter)
	{
		if (iter->second == action)
		{
			return true;
		}
	}
	return false;
}

bool InputSystem::HasState(uint32_t state) const
{
	for (auto iter = mStates.begin(); iter != mStates.end(); ++iter)
	{
		if (iter->second == state)
		{
			return true;
		}
	}
	return false;
}

bool InputSystem::HasRange(uint32_t range) const
{
	for (auto iter = mRanges.begin(); iter != mRanges.end(); ++iter)
	{
		if (iter->second == range)
		{
			return true;
		}
	}
	return false;
}

void InputSystem::Dispatch( float deltaTime )
{
	DispatchActions(deltaTime);
	DispatchStates(deltaTime);
	DispatchRanges(deltaTime);
}

void InputSystem::ForceCursorToCenter()
{
	Window* mainWindow = Environment::GetSingleton().GetApplication()->GetMainWindow();

	mLastMousePos.X() = mainWindow->GetWidth() / 2;
	mLastMousePos.Y() =  mainWindow->GetHeight() / 2;

	mainWindow->ForceMouseToCenter();
}


}