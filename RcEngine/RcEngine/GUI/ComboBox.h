#ifndef ComboBox_h__
#define ComboBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ComboBox : public UIElement
{
public:
	/**
	 * ComboBox selected item changed event
	 */
	typedef fastdelegate::FastDelegate1<int32_t> SelectionChangedEventHandler;
	SelectionChangedEventHandler EventSelectionChanged;

	static const String DropButtonStyleName;
	static const String ComboBoxStyleName;

public:
	ComboBox();
	virtual ~ComboBox();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual void Update(float dt);
	virtual void OnResize();

	virtual void OnHover(const int2& screenPos);

	// Child Control will handle focus
	virtual bool CanHaveFocus() const;
	virtual bool HasCombinedFocus() const;
	
	void AddItem(const std::wstring& text);
	void InsertItem(int32_t index, const std::wstring& text);

	void SetDropHeight(int32_t height);

	void SetSelectedIndex(int32_t index);
	int32_t GetSelectedIndex() const;

protected:
	void UpdateRect();
	void ToggleDropDownList();
	void HandleSelection(int32_t selIdx);

protected:
	ListBox* mItemList;
	LineEdit* mText;
	Button* mDropButton;

	int32_t mSelectedIndex;
	int32_t mDropHeight;

	GuiSkin::GuiStyle* mMainStyle;	
};


}

#endif // ComboBox_h__