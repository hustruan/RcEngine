#ifndef LineEdit_h__
#define LineEdit_h__

#include "GUI/UIElement.h"

namespace RcEngine {

class _ApiExport LineEdit : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<std::wstring> ReturnEventHandler;
	ReturnEventHandler EventReturnPressed;

public:
	LineEdit();
	virtual ~LineEdit();

	virtual void Update(float dt);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	virtual void OnResize();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual void OnDragBegin(const int2& position, uint32_t button);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);

	virtual bool CanHaveFocus() const;

	void SetEditLevel(TextEditLevel level)				{ mTextEditLevel = level; }
	TextEditLevel GetEditLevel() const					{ return mTextEditLevel; }

	void SetText(const std::wstring& text);
	const std::wstring& GetText() const				    { return mText; }

protected:
	void UpdateRect();
	void UpdateText();

	void DrawBackground(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	bool HasSelection() const   { return mSelectStartPos != mCaretPos; }
	void ClearSelection()       { mSelectStartPos = mCaretPos; }

	int32_t GetCaretAtCuror(float screenX) const;
	
	void PlaceCaret(int32_t caret);

protected:
	std::wstring mText;

	int32_t mBorder;

	ColorRGBA mTextColor;
	ColorRGBA mSelTextColor;
	ColorRGBA mSelBkColor;
	ColorRGBA mCaretColor;

	std::vector<float> mCharPositions;

	float mVisibleStartX;

	float mCaretBlinkRate;
	float mCaretBlinkTimer;
	bool mCaretOn;
	bool mDragCursor;

	TextEditLevel mTextEditLevel;

	float mRowHeight;

	Rectanglef mTextRect;

	// Caret Position, measured in char index
	int32_t mCaretPos;
	int32_t mSelectStartPos;

	GuiSkin::GuiStyle* mStyle;
};

}





#endif // LineEdit_h__
