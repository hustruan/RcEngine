#ifndef ListBox_h__
#define ListBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ListBox : public UIElement
{
public:

	// Selection changed event
	typedef fastdelegate::FastDelegate1<int32_t> SelChangedEventHandler;
	SelChangedEventHandler EventSelectionChanged;

	typedef fastdelegate::FastDelegate1<int32_t> SelEventHandler;
	SelEventHandler EventSelection;

public:
	ListBox();
	virtual ~ListBox();

	virtual void OnResize();
	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles  = nullptr );

	virtual bool CanHaveFocus() const;
	virtual bool HasCombinedFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);
	virtual bool OnMouseWheel( int32_t delta );

	void AddItem(const std::wstring& text);
	void InsertItem(int32_t index, const std::wstring& text);
	void RemoveItem(int32_t index);
	void RemoveAllItems();

	void SetSelectedIndex(int32_t index, bool fromInput = false); 
	int32_t GetSelectedIndex() const;

	const std::wstring& GetItem(int32_t index)  { return mItems[index]; }
	const std::wstring& GetSelectedItem() const { return mItems[GetSelectedIndex()]; }
	
	int32_t GetNumItems() const { return (int32_t)mItems.size();}

protected:
	void UpdateRect();
	void UpdateVScrollBar();

protected:

	std::vector<std::wstring> mItems;

	ScrollBar* mVertScrollBar;

	int32_t mScrollBarWidth;

	int32_t mSelectedIndex;
	int32_t mHoveredIndex;

	float mTextRowHeight;

	int32_t mNumVisibleItems;

	/**
	 * Hack:
	 *  Normal: background image
	 *  Hover:  selection image
	 */
	GuiSkin::GuiStyle* mLisBoxStyle;

	Rectanglef mTextRegion;
	Rectanglef mSelectionRegion;

	int32_t mBorder[4];
	int32_t mMargin;

	bool mPressed;
};


}


#endif // ListBox_h__
