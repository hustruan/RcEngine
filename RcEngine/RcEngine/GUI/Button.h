#ifndef Button_h__
#define Button_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Button : public UIElement
{
public:
	/**
	 * Button clicked event.
	 */
	typedef fastdelegate::FastDelegate0<> ButtonClickedEventHandler;
	ButtonClickedEventHandler EventButtonClicked;

	const static String StyleName;

public:
	Button();
	virtual ~ Button();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetPressedOffset(const int2& offset)	{ mPressedOffset = offset; }
	const int2& GetPressedOffset() const        { return mPressedOffset; }

	void SetHoverOffset(const int2& offset)		{ mHoverOffset = offset; }
	const int2& GetHoverOffset() const			{ return mHoverOffset; }

	void SetText(const std::wstring& txt);
	const std::wstring& GetText() const         { return mText; }

	void SetClickedEventRepeat(float delayTime, float rate);

private:

	void SetPressed(bool pressed);

protected:
	std::wstring mText;

	int2 mPressedOffset;
	int2 mHoverOffset;

	bool mPressed;

	float mRepeatDelay;		// repeat fire clicked event after delayed time when button is pressed
	float mRepeatRate;
	float mRepeatTimer;

	GuiSkin::GuiStyle* mStyle;
};


}

#endif // Button_h__
