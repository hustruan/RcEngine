#ifndef UIWindow_h__
#define UIWindow_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport UIWindow : public UIElement
{
public:
	enum WindowState
	{
		Normal,
		Minimized,
		Maximized
	};

	enum DragMode
	{
		Drag_None,
		Drag_Move,
		Drag_Resize_TopLeft,
		Drag_Resize_Top,
		Drag_Resize_TopRight,
		Drag_Resize_BottomRight,
		Drag_Resize_Bottom,
		Drag_Resize_BottomLeft,
		Drag_Resize_Left,
		Drag_Resize_Right,
	};

	enum BorderStyle
	{
		Border_None,
		Border_Fixed,
		Border_Resizable
	};

public:
	UIWindow();
	virtual ~UIWindow();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual void OnDragBegin(const int2& screenPos, uint32_t button);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual bool CanHaveFocus() const;

	void SetMovable(bool movable);

	void SetBorderStyle(BorderStyle style);
	void SetBorderThickness(int32_t thickness);

	void SetTitle(const std::wstring& title)	{ mTitle = title; }

	inline bool IsMinimizing() const { return mWindowState == Minimized && !mMinimized; }
	inline bool IsMaximizing() const { return mWindowState == Maximized && !mMaximized; }
	inline bool IsResizable() const  { return mBorderStyle == Border_Resizable; }
	inline bool IsMovable() const	 { return mMovable; }
	
	inline WindowState GetWindowState() const { return mWindowState; }

	void Minimize();
	void Maximize();
	void Restore();
	void Close();

protected:

	void UpdateRect();
	void UpdateState(float dt);

	void DrawBorder(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	DragMode GetDragMode(const int2& position);

	void SetCursorShape();

protected:

	std::wstring mTitle;

	BorderStyle mBorderStyle;
	WindowState mWindowState;
	DragMode mDragMode;

	bool mMovable;
	int32_t mBorderThickness;

	bool mMinimized; 
	bool mMaximized;

	int2 mLastNormalPos;
	int2 mLastNormalSize;

	int2 mMinimizedPos;
	int2 mMaximumSize;

	float2 mAnimationPos;
	float2 mAnimationSize;

	int2 mDragBeginPos;		// Position in parent region when drag begin
	int2 mDragBeginSize;    // Window size when drag begin
	int2 mDragBeginCurosr;  // Cursor position when drag begin

	Button* mCloseBtn;
	Button* mMinimizeBtn;
	Button* mMaximizeBtn;
	Button* mRestoreBtn;

	GuiSkin::GuiStyle* mStyle;

};


}

#endif // UIWindow_h__
