#ifndef CheckBox_h__
#define CheckBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport CheckBox : public UIElement
{
public:

	static const String StyleName;

	/**
	 * CheckBox state changed event.
	 */
	typedef fastdelegate::FastDelegate1<bool> StateChangedEventHandler;
	StateChangedEventHandler EventStateChanged;

public:
	CheckBox();
	virtual ~CheckBox();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetChecked(bool enalbe);
	bool IsChecked() const;

	void SetText(const std::wstring& text);
	
protected:
	
	bool mPressed;
	bool mCheched;

	std::wstring mText;

	GuiSkin::GuiStyle* mStyle;
};


}

#endif // CheckBox_h__
