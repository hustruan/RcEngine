#ifndef RadioButton_h__
#define RadioButton_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class RadioButtonGroup;

class _ApiExport RadioButton : public UIElement
{
public:
	static const String RadioStyleName;

public:
	RadioButton();
	virtual ~RadioButton();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles /* = nullptr */);
	virtual void Update(float dt);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	bool IsChecked() const			{ return mChecked; }
	void SetChecked(bool enable)	{ mChecked = enable; }

	void Toggle();
	void SetText(const std::wstring& text);

protected:

	std::wstring mText;
	bool mChecked;
	bool mPressed;
	bool mCanUncheck;

	GuiSkin::GuiStyle* mStyle;

	fastdelegate::FastDelegate1<RadioButton*> EventButtonClicked;
	 
	friend class RadioButtonGroup;	
};

class _ApiExport RadioButtonGroup 
{
public:
	typedef fastdelegate::FastDelegate1<std::wstring> SelChangedEventHandler;
	SelChangedEventHandler EventSelectionChanged;

public:
	RadioButtonGroup(const std::wstring& name = L"");
	~RadioButtonGroup();
	
	void AddButton(RadioButton* button);
	void AddButton(RadioButton* button, int32_t idx);

	RadioButton* CheckedButton() const;
	int32_t	CheckedIndex() const;

	const std::vector<RadioButton*>& GetButtons() const			{ return mRadioButtons; }
	
protected:
	void HandleButtonClicked(RadioButton* btn);


protected:
	std::vector<RadioButton*> mRadioButtons;	
	int32_t mSelectedIndex;
	std::wstring mGroupName;
};


}


#endif // RadioButton_h__
