#ifndef Window_h__
#define Window_h__

#include <Core/Prerequisites.h>

#if defined(RcWindows) 
	#include <Windows.h>
#elif defined(RcAndroid)
#endif

namespace RcEngine {

struct ApplicationSettings;
class InputSystem;

class _ApiExport Window
{
public:
	Window(const ApplicationSettings& settings);
	~Window(void);

#ifdef RcWindows
	HWND GetHwnd() const		{ return mhWnd; }
#endif

	uint32_t GetWidth() const	{ return mWidth; }
	uint32_t GetHeight() const  { return mHeight; }
	int32_t GetLeft() const	    { return mLeft; }
	int32_t GetTop() const		{ return mTop; }
	
	bool InSizeMove() const		{ return mInSizeMove; }
	bool IsMouseVisible() const { return mMouseVisible; }

	void Reposition(int32_t left, int32_t top);
	void ShowWindow();
	void SetTitle(const String& title);

	// Mouse 
	void SetMousePosition(int32_t x, int32_t y);
	void ForceMouseToCenter();
	void SetMouseVisible(bool visible);
	
	// After Resize or reposition, this will compute new window size
	void UpdateWindowSize();

	void CollectOSEvents();

private:
	void OnUserResized();
	void OnSuspend();
	void OnResume();
	void OnApplicationActivated();
	void OnApplicationDeactivated();
	void OnPaint();
	void OnClose();

#ifdef RcWindows
private:
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

public:
	EventHandler PaintEvent;
	EventHandler ResumeEvent;
	EventHandler SuspendEvent;
	EventHandler ApplicationActivatedEvent;
	EventHandler ApplicationDeactivatedEvent;
	EventHandler WindowClose;
	EventHandler UserResizedEvent;

protected:

	InputSystem* mInputSystem;

	int32_t mLeft, mTop;
	uint32_t mWidth, mHeight;
	std::string mName;

	bool mMouseVisible;
	bool mFullscreen;
	bool mInSizeMove;
	bool mMinimized, mMaximized;

#ifdef RcWindows
	HWND mhWnd;
	HINSTANCE mhInstance;
#endif

public:
	static Window* msWindow;
};

} // Namespace RcEngine

#endif // Window_h__