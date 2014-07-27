#ifndef UIElement_h__
#define UIElement_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/Vector.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <GUI/GuiSkin.h>

namespace RcEngine {

class Cursor;
class GuiSkin;

#define UI_MinPriority         0
#define UI_MaxPriority        100
#define UI_NormalPriority     50
#define UI_CursorPriority     100
#define UI_DropPopPriority    (UI_CursorPriority - 1)
#define UI_TopMostPriority    (UI_DropPopPriority - 1)

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();

	/**
	 * Init GUI Style. NULL for default GUI style.
	 */
	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	// Events
	virtual void OnResize();

	virtual void OnHover(const int2& screenPos);
	
	virtual void OnDragBegin(const int2& screenPos, uint32_t button);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);
	virtual bool OnMouseWheel( int32_t delta );

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnKeyRelease(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);

	virtual bool CanHaveFocus() const;
	
	const String& GetName() const				{ return mName; }
	void SetName(const String& name)			{ mName = name; }
	
	void SetPosition(const int2& position);
	const int2& GetPosition() const				{ return mPosition; }
	
	int2 GetScreenPosition();

	/**
	 * Set control size, if this is first time call SetSize, make sure
	 * control position has been set already. 
	 */
	void SetSize(const int2& size);
	const int2& GetSize() const					{ return mSize; }

	void SetMinSize(const int2& size);
	const int2& GetMinSize() const				{ return mMinSize; }
	const int2& GetMaxSize() const				{ return mMaxSize; }
	
	void SetVisible(bool visible);
	bool IsVisible() const						{ return mVisible; }

	bool HasFocus() const;

	/**
	 * Some Control may build based on other controls.
	 * So HasFocus will return false while the focus in on the internal controls.
	 * HasCombinedFocus will consider those internal controls.
	 */
	virtual bool HasCombinedFocus() const;

	bool IsMouseHover() const					{ return mHovering; }

	void SetEnable(bool enable)					{ mEnabled = enable; }
	bool IsEnabled() const						{ return mEnabled; }
	bool CanBringToFront() const				{ return mBringToFront; }

	const std::wstring& GetToolTip() const		{ return mToolTipText; }
	void SetToolTip(const std::wstring& txt)    { mToolTipText = txt; }

	IntRect GetRect() const;
	IntRect GetScreenRect();

	/**
	 * Screen rect contains all child 
	 */
	IntRect GetCombinedScreenRect();

	bool IsInside(int2 position, bool isScreen);
	bool IsInsideCombined(int2 position, bool isScreen);
	
	void SetParent(UIElement* parent);
	void RemoveFromParent();
	UIElement* GetParent() const				{ return mParent; }

	uint32_t GetNumChildren(bool recursive = false) const;
	
	UIElement* GetChild(const String& name, bool recursive = false) const;
	UIElement* GetChild(uint32_t index) const;

	UIElement* GetRoot() const;

	/**
	 * Sort children according to priority.
	 */
	void SortChildren();

	std::vector<UIElement*>& GetChildren()						{ return mChildren; }
	const std::vector<UIElement*>& GetChildren() const			{ return mChildren; }

	void FlattenChildren(std::vector<UIElement*>& children) const;	

	void AddChild(UIElement* child);
	void RemoveChild(UIElement* child);

	int2 ScreenToClient(const int2& screenPosition);
	int2 ClientToScreen(const int2& position);

	bool CanChildOutside() const { return mChildOutside; }

	void BringToFront();
	
	void SetPriority(int32_t priority);
	int32_t GetPriority() const { return mPriority; }

protected:
	virtual void UpdateRect();

	void MarkDirty();

	float GetDepthLayer() const { return float(UI_MaxPriority - mPriority) / UI_MaxPriority; }
	
protected:

	String mName;

	std::wstring mToolTipText;

	UIElement* mParent;
	std::vector<UIElement*> mChildren;

	/*
	 * 一般跟控件Z顺序有关，优先级高的控件先获得消息，
	 * 绘制的时候也画在其他低优先级控件上面.
	 */
	int32_t mPriority;

	bool mBringToFront;

	bool mSortOrderDirty;
	bool mPositionDirty;

	bool mHovering;
	bool mVisible;
	bool mEnabled;

	/**
	 * 有一些控件可以需要Child画在外面
	 */
	bool mChildOutside;

	int2 mPosition;
	int2 mScreenPosition;
	
	int2 mSize;
	int2 mMinSize;
	int2 mMaxSize;

	IntRect mClipBorder;

	float mOpacity;
};

}


#endif // UIElement_h__
