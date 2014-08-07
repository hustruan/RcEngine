#ifndef UIManager_h__
#define UIManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class UIElement;
class Cursor;
class GuiSkin;
class Window;
union InputEvent;

class _ApiExport UIManager : public Singleton<UIManager>
{
public:
	enum UIEvent
	{
		KeyUpDown,
		MouseUpDown,
		MouseMove,
		MouseWheel,
	};

public:
	UIManager();
	~UIManager();

	/**
	 * Called by Render Device Created
	 */
	void OnGraphicsInitialize();

	/**
	 * Called by Render Device Destroyed
	 */
	void OnGraphicsFinalize();

	/**
	 * Called by window resize
	 */
	void OnWindowResize(uint32_t width, uint32_t height);

	void Update(float delta);

	void Render();

	/** 
	 * Handle GUI Event, if event is handled by GUI, no need to pass to other game object.
	 * return true if the event is consumed, false if need to pass to others.
	 */
	bool OnEvent(const InputEvent& event);
	
	void SetFocusElement(UIElement* element);
	UIElement* GetFocusElement()						{ return mFocusElement; }	
	
	/**
	 * Calculate a minimized windows position for current minimizing windows.
	 * If there exits another minimizing window, return false. 
	 */
	bool GetMinimizedPosition(UIWindow* window, int2* pos);

	int2 GetMaximizedSize(UIWindow* window) const;
	

	const shared_ptr<Font>& GetDefaultFont() const      { return mFont; }

	GuiSkin* GetDefaultSkin();

	/**
	 * Get root UI Element
	 */
	UIElement* GetRoot() const								{ return mRootElement; }

	void SetClipBoardText(const std::wstring& text)			{ mClipBoardText = text; }
	const std::wstring& GetClipBoardText() const			{ return mClipBoardText; }
	void ClearClipBoardText()								{ mClipBoardText.clear(); }

private:

	/**
	 * Handle keyboard [press|release] event.
	 * @return 
	 *  If exits a UIElement consume this event, return true, or return false.
	 */
	bool HandleKeyPress(uint16_t key);
	bool HandleKeyRelease(uint16_t key);

	/**
	 * Handle text input event.
	 * @return 
	 *  If exits a UIElement consume this event, return true, or return false.
	 */
	bool HandleTextInput(uint16_t unicode);

	/**
	 * Handle mouse button [press|release] event.
	 * @param 
	 *  pos: Mouse screen position.
	 *  button: Mouse button which cause this event
	 * @return 
	 *  If exits a UIElement consume this event, return true, or return false.
	 */
	bool HandleMousePress(const int2& pos, uint32_t button);
	bool HandleMouseRelease(const int2& pos, uint32_t button);

	/**
	 * Handle mouse move event.
	 * @param 
	 *  pos: Mouse screen position.
	 *  buttons: Button states when mouse move 
	 * @return 
	 *  if exits a UIElement consume this event, return true, or return false.
	 */
	bool HandleMouseMove(const int2& pos, uint32_t buttons);

	/**
	 * Handle mouse move event.
	 * @return 
	 *  if exits a UIElement consume this event, return true, or return false.
	 */
	bool HandleMouseWheel(const int2& pos, int32_t delta);

private:

	UIElement* GetElementAtPoint(const int2& pos);
	void GetElementAtPoint(UIElement*& result, UIElement* current, const int2& pos);

	UIElement* GetFocusableElement(UIElement* element);

	void RenderUIElement(UIElement* element, const IntRect& currentScissor);

	void Update(UIElement* elem, float dt);

protected:

	bool mInitialize;
	
	UIElement* mFocusElement;
	UIElement* mRootElement;
	UIElement* mDragElement;

	int32_t mWidth, mHeight;

	shared_ptr<Font> mFont;
	shared_ptr<SpriteBatch> mSpriteBatchFont;
	shared_ptr<SpriteBatch> mSpriteBatch;

	std::wstring mClipBoardText;

	GuiSkin* mDefaultSkin;
	
	Cursor* mCursor;
	
	// Platform Window
	Window* mMainWindow;

	// InGame UI Window
	std::list<UIWindow*> mMinimizeWindows;
};

}


#endif // UIManager_h__
