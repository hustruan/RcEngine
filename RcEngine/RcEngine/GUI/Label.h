#ifndef Label_h__
#define Label_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

/**
 * A text label, label is a non focusable control, no input event
 */
class _ApiExport Label : public UIElement
{
public:
	const static String StyleName;

public:
	Label();
	virtual ~Label();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetTextColor(const ColorRGBA& color)		{ mTextColor = color; }
	const ColorRGBA& GetTextColor() const			{ return mTextColor; }

	void SetFontSize(float fontSize)				{ mFontSize = fontSize; }
	float GetFontSize() const						{ return mFontSize; }

	void SetText(const std::wstring& text )			{ mText = text; }
	const std::wstring& GetText() const				{ return mText; }
	
	void SetTextAlignment(uint32_t align)			{ mTextAlignment = align; }
	uint32_t GetTextAlignment() const				{ return mTextAlignment; }

protected:
	GuiSkin::GuiStyle* mStyle;
	shared_ptr<Font> mFont;

	std::wstring mText;
	uint32_t mTextAlignment;
	
	ColorRGBA mTextColor;
	float mFontSize;
};


}



#endif // Label_h__
