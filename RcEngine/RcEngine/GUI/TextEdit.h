#ifndef UITextEdit_h__
#define UITextEdit_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport TextEdit : public UIElement
{
public:
	static const String StyleName;

public:
	TextEdit();
	virtual ~TextEdit();

	virtual void Update(float dt);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	virtual void OnResize();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual bool OnMouseWheel( int32_t delta );

	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);
	
	virtual bool CanHaveFocus() const;

	void SetText(const std::wstring& text);
	const std::wstring& GetText() const				    { return mText; }

	void SetTextColor(const ColorRGBA& color)			{ mTextColor = color; }
	void SetSelectedTextColor(const ColorRGBA& color)	{ mSelTextColor = color; }

	void SetEditLevel(TextEditLevel level)				{ mTextEditLevel = level; }
	TextEditLevel GetEditLevel() const					{ return mTextEditLevel; }

protected:
	
	/**
	 * Calculate caret position from screen position.
	 */
	int2 GetCaretAtCursor(float screenX, float screenY) const;
	
	/**
	 * Calculate caret position from char index in original text.
	 */
	int2 GetCaretFromChar(int32_t index) const;
	
	/**
	 * Calculate char index at caret pos (x, y).
	 */
	size_t GetCharFromCaret(const int2& caret) const;

	void DrawSelection(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void DrawBackground(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	
	void DeleteSlectedText();
	void DeletePreChar();
	void DeleteNextChar();

	void PlaceCaret(const int2& newCaretPos);
	void PlaceCaret(int32_t newCaretX, int32_t newCaretY);

	void UpdateRect();
	void UpdateText();
	void WrapText();
	void UpdateVisisbleText();

	bool HasSelection() const { return mCaretPos != mSelectStartPos; }
	void ClearSelection()     { mSelectStartPos = mCaretPos; }

	void HandleVScrollBar(int32_t value);
	
protected:

	std::wstring mText;
	std::wstring mWrappedText;
	std::wstring mPrintText;
	
	bool mWordWrap;
	TextEditLevel mTextEditLevel;

	int32_t mFirstVisibleY;
	int32_t mNumVisibleY;
	
	//float mVisibleStartX;
	//float mVisibleStartY;

	int32_t mNumLines;

	Rectanglef mTextRect;
	Rectanglef mBackRect;

	std::vector<int32_t> mWrappedBreakAt;
	std::vector< std::vector<float> > mCharPositions;
	
	uint32_t mTextAlign;

	int32_t mBorder[Border_Count];
	float mRowHeight;
	
	ColorRGBA mTextColor;
	ColorRGBA mSelTextColor;
	ColorRGBA mSelBkColor;
	ColorRGBA mCaretColor;

	ScrollBar* mHorzScrollBar;
	ScrollBar* mVertScrollBar;
	int32_t mScrollBarWidth;

	bool mDragMouse;

	float mCaretBlinkRate;
	float mCaretBlinkTimer;
	bool mCaretOn;

	// Caret Position, measured in char index
	int2 mCaretPos;
	int2 mSelectStartPos;

	GuiSkin::GuiStyle* mTextEditStyle;
};


}


#endif // UITextEdit_h__
