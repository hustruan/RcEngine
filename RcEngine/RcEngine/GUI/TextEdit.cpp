#include <GUI/TextEdit.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>
#include <Core/Exception.h>
#include <cctype>

namespace RcEngine {

static const int32_t BorderWidth = 8;
static const int32_t ScrollBarWidth = 20;

const String TextEdit::StyleName("TextEdit::TextEdit");

TextEdit::TextEdit()
	: mDragMouse(false),
	  mWordWrap(true),
	  mCaretBlinkRate(1.0f),
	  mCaretBlinkTimer(0.0f),
	  mScrollBarWidth(ScrollBarWidth),
	  mTextEditStyle(nullptr),
	  mVertScrollBar(nullptr),
	  mCaretOn(false),
	  mTextEditLevel(TEL_Edit),
	  mNumLines(0),
	  mFirstVisibleY(0),
	  mNumVisibleY(0),
	  mCaretPos(0, 0),
	  mSelectStartPos(0, 0)
{
	mTextColor = ColorRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	mSelTextColor = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	mSelBkColor = ColorRGBA( 0.67f, 0.83F, 1.0f, 0.5f );
	mCaretColor = ColorRGBA( 1.0f, 0, 0, 0 );

	for (size_t i = 0; i < 4; ++i)
		 mBorder[i] = BorderWidth;

	mTextAlign = AlignLeft | AlignTop;

	mVertScrollBar = new ScrollBar(UI_Vertical);
	mVertScrollBar->SetScrollButtonRepeat(true);
	mVertScrollBar->SetVisible(false);
	mVertScrollBar->EventValueChanged.bind(this, &TextEdit::HandleVScrollBar);

	AddChild(mVertScrollBar);
}

TextEdit::~TextEdit()
{

}

void TextEdit::SetText( const std::wstring& text )
{
	mText = text;
	UpdateText();
}

bool TextEdit::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void TextEdit::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(TextEdit::StyleName);
		mTextEditStyle = iter->second;

		mVertScrollBar->InitGuiStyle(styles);
	}
	else
	{
		// use default
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();

		mVertScrollBar->InitGuiStyle(nullptr);
	
		// background in Normal State
		mTextEditStyle = &defaultSkin->TextEdit;
	}

	// Hack: store border in UI_State_Pressed
	mBorder[Border_Left] = mTextEditStyle->StyleStates[UI_State_Pressed].TexRegion.X;
	mBorder[Border_Right] = mTextEditStyle->StyleStates[UI_State_Pressed].TexRegion.Width;
	mBorder[Border_Top] = mTextEditStyle->StyleStates[UI_State_Pressed].TexRegion.Y;
	mBorder[Border_Bottom] = mTextEditStyle->StyleStates[UI_State_Pressed].TexRegion.Height;

	// Scroll bar
	mScrollBarWidth = mVertScrollBar->GetTrackExtext() + 1;

	// Init row height
	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	mRowHeight = mTextEditStyle->Font->GetRowHeight() * fontScale;
}

void TextEdit::Update(float dt)
{
	// Blink the caret
	if (mCaretBlinkRate > 0.0f)
		mCaretBlinkTimer = fmodf(mCaretBlinkTimer + mCaretBlinkRate * dt, 1.0f);
	else
		mCaretBlinkTimer = 0.0f;

	if (HasFocus())
		mCaretOn = (mCaretBlinkTimer > 0.5f);
	else 
	{
		mCaretOn = false;
		ClearSelection();
	}
}

void TextEdit::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	float zOrder = GetDepthLayer();

	// Draw background first
	//DrawBackground(spriteBatch, spriteBatchFont);
	mTextEditStyle->DrawNinePatch(spriteBatch, UI_State_Normal, mBackRect, zOrder);

	// Draw selected background if has selection
	DrawSelection(spriteBatch, spriteBatchFont);

	// Draw Text
	if (mPrintText.size())
		mTextEditStyle->Font->DrawString(spriteBatchFont, mPrintText, mTextEditStyle->FontSize, mTextAlign, mTextRect, mTextColor, zOrder);

	// Draw Caret
	if (mCaretOn)
	{
		int32_t line = mCaretPos.Y() - mFirstVisibleY;

		if (line >= 0 && line < mNumVisibleY)
		{
			Rectanglef caretRC;

			caretRC.X = mCharPositions[mCaretPos.Y()][mCaretPos.X()];
			caretRC.Y = mTextRect.Y + line * mRowHeight;
			caretRC.Width = 1.0f;
			caretRC.Height = mRowHeight;

			spriteBatch.Draw(mTextEditStyle->StyleTex, caretRC, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA::Black, zOrder);
		}
		
	}

	// Reset
	mHovering = false;
	mCaretOn = false;
}

void TextEdit::DrawSelection( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (HasSelection())
	{	
		int2 start(mSelectStartPos);
		int2 end(mCaretPos);

		if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
			std::swap(start, end);

		Rectanglef rect;
		rect.Height = mRowHeight;

		float zOrder = GetDepthLayer();

		if (start.Y() != end.Y())
		{
			int32_t line;

			// First Line
			line = start.Y() - mFirstVisibleY;
			if (line >= 0 && line < mNumVisibleY)
			{
				rect.SetLeft( mCharPositions[start.Y()][start.X()] );
				rect.SetRight( mCharPositions[start.Y()].back() );
				rect.SetTop( mTextRect.Y + mRowHeight * line );		
				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mSelBkColor, zOrder);
			}

			// Last Line
			line = end.Y() - mFirstVisibleY;
			if (line >= 0 && line < mNumVisibleY)
			{

				rect.SetLeft( mCharPositions[end.Y()].front() );
				rect.SetRight( mCharPositions[end.Y()][end.X()] );
				rect.SetTop( mTextRect.Y + mRowHeight * line );		
				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mSelBkColor, zOrder);
			}

			for (int32_t i = start.Y() + 1; i < end.Y(); ++i)
			{
				line = i - mFirstVisibleY;
				if (line >= 0 && line < mNumVisibleY)
				{
					rect.SetLeft( mCharPositions[i].front() );
					rect.SetRight( mCharPositions[i].back() );
					rect.SetTop( mTextRect.Y + mRowHeight * line);	
					spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mSelBkColor, zOrder);
				}
			}
		}
		else
		{
			int32_t line = start.Y() - mFirstVisibleY;
			if (line >= 0 && line < mNumVisibleY)
			{
				rect.SetLeft( mCharPositions[start.Y()][start.X()] );
				rect.SetRight( mCharPositions[end.Y()][end.X()] );
				rect.SetTop( mTextRect.Y + mRowHeight * line );		
				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mSelBkColor, zOrder);
			}
		}
	}
}

void TextEdit::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton && mNumLines > 0)
	{
		float screenX = (float)screenPos.X();
		float screenY = (float)screenPos.Y();

		if (mTextRect.Contains(screenX, screenY) )
		{
			mCaretPos = GetCaretAtCursor(screenX, screenY);
			mDragMouse = true;
			mSelectStartPos = mCaretPos;
		}
	}
}

void TextEdit::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if ( mDragMouse && (buttons & InputEventType::LeftButtonMask) && mNumLines > 0 )
	{
		float screenX = (float)screenPos.X();
		float screenY = (float)screenPos.Y();

		if (screenY < mTextRect.Top())
		{
			if (mVertScrollBar->IsVisible())
				mVertScrollBar->Scroll(-1);

			mCaretPos.Y() = (std::max)(0, mCaretPos.Y() - 1);
			mCaretPos.X() = (std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()].size() - 1);
		}
		else if (screenY > mTextRect.Bottom())
		{
			if (mVertScrollBar->IsVisible())
				mVertScrollBar->Scroll(1);

			mCaretPos.Y() = (std::min)(mNumLines-1, mCaretPos.Y() + 1);
			mCaretPos.X() = (std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()].size() - 1);
		}
		else
		{
			if (mTextRect.Contains(Clamp(screenX, mTextRect.Left(), mTextRect.Right()), screenY))
				mCaretPos = GetCaretAtCursor(screenX, screenY);
		}	
	}
}

void TextEdit::OnDragEnd( const int2& position )
{
	mDragMouse = false;
}

bool TextEdit::OnMouseWheel( int32_t delta )
{
	if (mVertScrollBar->IsVisible())
		mVertScrollBar->Scroll(-delta);

	return true;
}

bool TextEdit::OnKeyPress( uint16_t key )
{
	switch (key)
	{
	case KC_C:
	case KC_X:
		{
			if (HasSelection())
			{
				InputSystem& inputSys = InputSystem::GetSingleton();

				if( inputSys.KeyDown(KC_LeftCtrl) || inputSys.KeyDown(KC_RightCtrl) )
				{
					int2 start(mSelectStartPos);
					int2 end(mCaretPos);

					if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
						std::swap(start, end);

					size_t delStart = GetCharFromCaret(start);
					size_t delEnd = GetCharFromCaret(end);

					UIManager::GetSingleton().SetClipBoardText(mText.substr(delStart, delEnd - delStart));

					if (key == KC_X)
					{
						mText.erase(delStart, delEnd - delStart);
						UpdateText();

						int2 newCaretPos = GetCaretFromChar(delStart);		
						PlaceCaret(newCaretPos);
					}	

					//printf("case KC_C KC_X at clock:%d\n", clock()); 
				}
			}
		}
		break;

	case KC_V:
		{
			if (HasSelection())
			{
				int2 start(mSelectStartPos);
				int2 end(mCaretPos);

				if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
					std::swap(start, end);

				size_t delStart = GetCharFromCaret(start);
				size_t delEnd = GetCharFromCaret(end);

				const std::wstring& clip = UIManager::GetSingleton().GetClipBoardText();
				mText.replace(delStart, delEnd - delStart, clip);
				UpdateText();

				int2 newCaretPos = GetCaretFromChar(delStart + clip.length());		
				PlaceCaret(newCaretPos);
			}
			else
			{
				size_t start = GetCharFromCaret(mCaretPos);

				const std::wstring& clip = UIManager::GetSingleton().GetClipBoardText();
				mText.insert(start, clip);
				UpdateText();	

				int2 newCaretPos = GetCaretFromChar(start+clip.length());		
				PlaceCaret(newCaretPos);

				/*if (mVertScrollBar->IsVisible() &&  mCaretPos.Y() >= mFirstVisibleY + mNumVisibleY )
					mVertScrollBar->Scroll(1);*/
			}
		}
		break;

	case KC_LeftArrow:
		{
			if (mCaretPos.X() > 0)
				PlaceCaret(mCaretPos.X() - 1, mCaretPos.Y());
			else if (mCaretPos.X() == 0 && mCaretPos.Y() > 0)
			{
				if (mCaretPos.Y() == mFirstVisibleY && mVertScrollBar->IsVisible())
					mVertScrollBar->Scroll(-1);

				PlaceCaret(mCharPositions[mCaretPos.Y()-1].size() - 1, mCaretPos.Y()-1);
			}

		}
		break;

	case KC_RightArrow:
		{
			if (mCaretPos.X() < (int32_t)mCharPositions[mCaretPos.Y()].size() - 1)
				PlaceCaret(mCaretPos.X() + 1, mCaretPos.Y());
			else 
			{
				if (mCaretPos.Y() < (int32_t)mCharPositions.size() - 1)
				{
					if ( (mCaretPos.Y() == mFirstVisibleY + mNumVisibleY - 1) && mVertScrollBar->IsVisible() )
						mVertScrollBar->Scroll(1);

					PlaceCaret(0, mCaretPos.Y()+1);
				}
			}
		}
		break;

	case KC_UpArrow:
		{
			if (mCaretPos.Y() > 0)
			{
				if (mCaretPos.Y() == mFirstVisibleY && mVertScrollBar->IsVisible())
					mVertScrollBar->Scroll(-1);

				PlaceCaret((std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()-1].size()-1), mCaretPos.Y()-1);
			}
		}
		break;

	case KC_DownArrow:
		{
			if (mCaretPos.Y() < (int32_t)mCharPositions.size() - 1)
			{
				if ( (mCaretPos.Y() == mFirstVisibleY + mNumVisibleY - 1) && mVertScrollBar->IsVisible() )
					mVertScrollBar->Scroll(1);

				PlaceCaret((std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()+1].size()-1), mCaretPos.Y()+1);
			}
		}
		break;

	case KC_PageDown:
		{
			if ( mFirstVisibleY < mNumLines - mNumVisibleY && mVertScrollBar->IsVisible() )
				mVertScrollBar->Scroll(mNumVisibleY);
		}
		break;

	case KC_PageUp:
		{
			if ( mFirstVisibleY > 0 && mVertScrollBar->IsVisible() )
				mVertScrollBar->Scroll(-mNumVisibleY);
		}
		break;

	case KC_Home:
		PlaceCaret(0, mCaretPos.Y());
		break;

	case KC_End:
		PlaceCaret(mCharPositions[mCaretPos.Y()].size()-1 , mCaretPos.Y());
		break;

	case KC_Delete:
		{
			if (HasSelection())
				DeleteSlectedText();
			else
				DeleteNextChar();
		}
		break;

	case KC_BackSpace:
		{
			if (HasSelection())
				DeleteSlectedText();
			else
				DeletePreChar();
		}
		break;

	case KC_Enter:
		{
			size_t caretIdx = GetCharFromCaret(mCaretPos);

			mText.insert(mText.begin() + caretIdx, L'\n');
			UpdateText();		
			PlaceCaret(0, mCaretPos.Y() + 1);

		}
		break;
	case KC_Tab:
		{
			size_t start = GetCharFromCaret(mCaretPos);

			mText.insert(mText.begin() + start, L'\t');
			UpdateText();	

			int2 newCaretPos = GetCaretFromChar(start+1);		
			PlaceCaret(newCaretPos);

			if (mVertScrollBar->IsVisible() &&  mCaretPos.Y() >= mFirstVisibleY + mNumVisibleY )
				mVertScrollBar->Scroll(1);
		}
		break;
	default:
		break;
	}

	return true;
}

bool TextEdit::OnTextInput( uint16_t unicode )
{
	bool eventConsumed = true;

	InputSystem& inputSys = InputSystem::GetSingleton();
	if( inputSys.KeyDown(KC_LeftCtrl) || inputSys.KeyDown(KC_RightCtrl) )
		return true;

	if (HasFocus() && unicode >= 0x20)
	{
		if (HasSelection())
		{
			int2 start(mSelectStartPos);
			int2 end(mCaretPos);

			if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
				std::swap(start, end);

			size_t delStart = GetCharFromCaret(start);
			size_t delEnd = GetCharFromCaret(end);

			mText.replace(delStart, delEnd - delStart, 1, (wchar_t)unicode);

			UpdateText();		
			PlaceCaret(start.X() + 1, start.Y());

		}
		else
		{
			size_t start = GetCharFromCaret(mCaretPos);
			wchar_t c = mText[start];
			mText.insert(mText.begin() + start, (wchar_t)unicode);
			UpdateText();	

			int2 newCaretPos = GetCaretFromChar(start+1);		
			PlaceCaret(newCaretPos);

			if (mVertScrollBar->IsVisible() &&  mCaretPos.Y() >= mFirstVisibleY + mNumVisibleY )
				mVertScrollBar->Scroll(1);

		}
	}

	return eventConsumed;
}

void TextEdit::PlaceCaret( const int2& newCaretPos )
{
	if (newCaretPos.Y() != mCaretPos.Y())
	{
		mCaretPos.Y() = Clamp(newCaretPos.Y(), 0, (int32_t)mCharPositions.size());
	}

	if (newCaretPos.X() != mCaretPos.X())
	{
		mCaretPos.X() = Clamp(newCaretPos.X(), 0, (int32_t)mCharPositions[mCaretPos.Y()].size());
	}

	ClearSelection();
}

void TextEdit::PlaceCaret( int32_t newCaretX, int32_t newCaretY )
{
	if (newCaretY != mCaretPos.Y())
	{
		mCaretPos.Y() = Clamp(newCaretY, 0, (int32_t)mCharPositions.size());
	}

	if (newCaretX != mCaretPos.X())
	{
		mCaretPos.X() = Clamp(newCaretX, 0, (int32_t)mCharPositions[mCaretPos.Y()].size());
	}

	ClearSelection();
}

void TextEdit::UpdateRect()
{
	UIElement::UpdateRect();

	if (!mTextEditStyle)
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Init Gui Style First!", "TextEdit::UpdateText");

	int2 screenPos = GetScreenPosition();

	mBackRect.X = (float)screenPos.X();
	mBackRect.Y = (float)screenPos.Y();
	mBackRect.Width = (float)mSize.X();
	mBackRect.Height = (float)mSize.Y();

	mTextRect.X = (float)screenPos.X() + mBorder[Border_Left];
	mTextRect.Width = (float)mSize.X() - mBorder[Border_Left] - mBorder[Border_Right];

	mTextRect.Y = (float)screenPos.Y() + mBorder[Border_Top];
	mTextRect.Height = (float)mSize.Y() - mBorder[Border_Top] - mBorder[Border_Bottom];

	mNumVisibleY = (int32_t)floor(mTextRect.Height / mRowHeight);

	int32_t shrink = (int32_t)floor( mTextRect.Height - mNumVisibleY * mRowHeight );

	mTextRect.Height -= shrink;
	mBackRect.Height -= shrink;
	mSize.Y() -= shrink;

	if (mVertScrollBar->IsVisible())
		mTextRect.Width -= mScrollBarWidth;

	UpdateText();
}

void TextEdit::UpdateText()
{
	if (!mTextEditStyle)
		return;

	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	const float rowHeight = mTextEditStyle->Font->GetRowHeight(fontScale);

	if (mWordWrap)
	{
		WrapText();

		if (mNumLines > mNumVisibleY)
		{
			if ( mVertScrollBar->IsVisible() == false )
			{
				mVertScrollBar->SetVisible(true);
				mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 1));
				mVertScrollBar->SetSize(int2(mScrollBarWidth-1, mSize.Y() - 2));
					
				mVertScrollBar->SetScrollableSize(mNumLines);

				int32_t remain = mNumLines - mNumVisibleY;
				mVertScrollBar->SetScrollRange(0, remain);
				
				mTextRect.Width -= mScrollBarWidth;
				mBackRect.Width -= mScrollBarWidth;

				if (mWordWrap)
					WrapText();
			}	
			else 
			{
				mVertScrollBar->SetScrollableSize(mNumLines);
				int32_t remain = mNumLines - mNumVisibleY;
				
				if (mFirstVisibleY > remain)
					mFirstVisibleY = remain;

				
				mVertScrollBar->SetScrollRange(0, remain);		
			}
		}
		else
		{
			if (mVertScrollBar->IsVisible())
			{
				mVertScrollBar->SetVisible(false);
				mTextRect.Width += mScrollBarWidth;
				mBackRect.Width += mScrollBarWidth;

				if (mWordWrap)
					WrapText();

				mFirstVisibleY = 0;
			}
		}
	}
	else
	{
		mWrappedText = mText;
		mNumLines = std::count(mWrappedText.begin(), mWrappedText.end(), L'\n') + 1;
	}

	// Keep track of every line char start pos
	float charX = mTextRect.X;
	mCharPositions.resize(1);
	mCharPositions.back().clear();
	mCharPositions.back().push_back(charX);
	for (size_t i = 0; i < mWrappedText.length(); ++i)
	{
		wchar_t ch = mWrappedText[i];

		if (ch == L'\n')
		{
			mCharPositions.resize(mCharPositions.size() + 1);

			charX = (float)mTextRect.X;
			mCharPositions.back().push_back(charX);
		}
		else
		{
			if (ch == L'\t')
			{
				const Font::Glyph& glyph = mTextEditStyle->Font->GetGlyphInfo(L' ');
				charX += glyph.Advance * fontScale * 4;
			}
			else
			{
				const Font::Glyph& glyph = mTextEditStyle->Font->GetGlyphInfo(ch);
				charX += glyph.Advance * fontScale;
			}
	
			mCharPositions.back().push_back(charX);		
		}
	}
	
	UpdateVisisbleText();
}

void TextEdit::UpdateVisisbleText()
{
	size_t start = 0;
	size_t end = mWrappedText.length() - 1;

	if (mFirstVisibleY > 0)
	{
		size_t lineIdx = 0;
		for (size_t i = 0; i < mWrappedText.length(); ++i)
		{
			if (mWrappedText[i] == L'\n')
			{
				if (++lineIdx == mFirstVisibleY)
				{
					start = i+1;
					break;
				}
			}
		}
	}

	// Update print text
	size_t numLines = (std::min)(mNumVisibleY, mNumLines);
	for (size_t i = start; i < mWrappedText.length(); ++i)
	{
		if (mWrappedText[i] == L'\n')
		{
			if (--numLines == 0)
			{
				end = i;
				break;
			}
		}	
	}

	mPrintText = mWrappedText.substr(start, end - start + 1);
}

void TextEdit::HandleVScrollBar( int32_t value )
{
	int32_t minValue, maxValue;
	mVertScrollBar->GetScrollRange(&minValue, &maxValue);

	mFirstVisibleY = (size_t)value;

	UpdateVisisbleText();
}

void TextEdit::WrapText()
{
	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();

	mNumLines = 0;

	mWrappedText.clear();
	mWrappedBreakAt.clear();

	if (mText.empty())
		return;

	// Wrap Text if enable
	if (mWordWrap)
	{
		bool isBreakable = false;
		bool wasBreakable = false; // value of isBreakable for last char (i-1)
		bool isParens = false;     // true if one of ({[
		bool wasParens = false;    // value of isParens for last char (i-1)

		float x = (float)0;
		float y = (float)0;
		float w = (float)mTextRect.Width;
		float lineWidth = 0.0f;
		float textwidth = 0.0f;
		int32_t lastBreak = -1;
		size_t start = 0;

		Font& font = *mTextEditStyle->Font;
		for (size_t i = 0; i < mText.length(); ++i)
		{
			wchar_t c = mText[i];

			if (c != L'\n')
			{
				const Font::Glyph& glyph = font.GetGlyphInfo(c);	
				float ww = glyph.Advance * fontScale;

				if (c == L'\t')
					ww = font.GetGlyphInfo(L' ').Advance * fontScale * 4;

				isParens = (c == L'(') || (c == L'[') || (c == L'{');	
				// isBreakable is true when we can break after this character.
				isBreakable = ( std::isspace(c) || std::ispunct(c) ) && !isParens;

				if (!isBreakable && i < mText.length() - 1)
				{
					wchar_t nextc = mText[i+1];
					isBreakable = (nextc == L'(') || (nextc == L'[') || (nextc == L'{');	
				}

				int32_t breakAt = -1;	
				bool wrappedBreak = true;

				//if (x + ww > w && lastBreak != -1) // time to break and we know where   
				//	breakAt = lastBreak;

				//if (x + ww > w - 4 && lastBreak == -1) // time to break but found nowhere, break here
				//	breakAt = i;

				if (x + ww > w)
				{
					if (lastBreak == -1)
						breakAt = i - 1;
					else
						breakAt = lastBreak;
				}

				// don't leave the last char alone
				if ( (i == mText.length() - 2) && (x + ww + font.GetGlyphInfo(mText[i+1]).Advance * fontScale) > w )  
					breakAt = (lastBreak == -1) ? i - 1 : lastBreak;

				if (breakAt != -1)
				{
					float thisLineWidth = (lastBreak == -1) ? x + ww : lineWidth;

					mWrappedBreakAt.push_back(breakAt);

					mWrappedText += mText.substr(start, breakAt-start+1) + L'\n';
					start = breakAt + 1;

					mNumLines++;

					textwidth = (std::max)(textwidth, thisLineWidth);
					x = 0; y += mRowHeight;

					wasBreakable = true;
					wasParens = false;
					if ( lastBreak != -1 )
					{
						// Breakable char was found, restart from there
						i = lastBreak;
						lastBreak = -1;
						continue;
					}
				}
				else if (isBreakable)
				{
					lastBreak = i;
					lineWidth = x + ww;
				}

				x += ww;
				wasBreakable = isBreakable;
				wasParens = isParens;
			}
			else
			{
				mWrappedText += mText.substr(start, i-start+1);
				start = i + 1;

				x = 0; y += mRowHeight;
				wasBreakable = false;
				wasParens = false;
				lastBreak = -1;
				lineWidth = 0;
				mNumLines++;
			}

		}

		mNumLines++;
		textwidth = (std::max)(textwidth, x);
		y += mRowHeight;
		mWrappedText += mText.substr( start );
	}
	else
	{
		mWrappedText = mText;
		mNumLines = std::count(mWrappedText.begin(), mWrappedText.end(), L'\n') + 1;
	}
}

int2 TextEdit::GetCaretAtCursor( float screenX, float screenY ) const
{
	int2 caret(0, 0);

	float actualX = screenX;

	/*if (!mWordWrap)	
		actualX = mVisibleStartX + (screenX - mTextRect.Left()); 
	else
		actualX = screenX;*/

	caret.Y() = Clamp((int32_t)floorf((screenY - mTextRect.Y) / mRowHeight), 0, mNumVisibleY - 1);
	caret.Y() = Clamp( caret.Y() + mFirstVisibleY, 0, mNumLines-1);

	caret.X() = mCharPositions[caret.Y()].size() - 1;
	for (size_t i = 0; i < mCharPositions[caret.Y()].size() - 1; ++i)
	{
		float half = (mCharPositions[caret.Y()][i] + mCharPositions[caret.Y()][i+1]) * 0.5f;

		if (actualX < half)
		{
			caret.X() = (int32_t)i;
			break;
		}
	}


	return caret;
}

int2 TextEdit::GetCaretFromChar( int32_t index ) const
{
	int2 caret(0, 0);

	size_t wrappedIndex = index;
	for (size_t i = 0; i < mWrappedBreakAt.size(); ++i)
	{
		if ((int32_t)index > mWrappedBreakAt[i]+1)
			wrappedIndex++;
		else
			break;
	}

	for (size_t i = 0; i < wrappedIndex; ++i)
	{
		if (mWrappedText[i] == L'\n')
		{
			caret.Y()++;
			caret.X() = 0;
		}
		else
			caret.X()++;
	}

	return caret;
}

size_t TextEdit::GetCharFromCaret( const int2& caret ) const
{
	size_t wrappedIndex = 0;
	for (int32_t i = 0; i < caret.Y(); ++i)
		wrappedIndex += mCharPositions[i].size();

	wrappedIndex += caret.X();

	wchar_t c = mWrappedText[wrappedIndex];

	size_t textIdx = wrappedIndex;

	if (mWordWrap)
	{
		size_t numPadding = 1;
		for (size_t i = 0; i < mWrappedBreakAt.size(); ++i)
		{
			if (mWrappedBreakAt[i] + numPadding < (int32_t)wrappedIndex)
			{
				textIdx--;
				numPadding++;
			}
			else
				break;
		}
	}
	
	return textIdx;
}

void TextEdit::DeleteSlectedText()
{
	size_t x = 0, y = 0;
	
	int2 start(mSelectStartPos);
	int2 end(mCaretPos);

	if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
		std::swap(start, end);
		
	size_t delStart = GetCharFromCaret(start);
	size_t delEnd = GetCharFromCaret(end);

	mText.erase(delStart, delEnd - delStart);
	UpdateText();
	
	int2 newCaretPos = GetCaretFromChar(delStart);		
	PlaceCaret(newCaretPos);

	//std::wcout << mText << std::endl;
}

void TextEdit::DeleteNextChar()
{
	size_t delStart = GetCharFromCaret(mCaretPos);

	if (delStart < mText.size())
	{
		mText.erase(delStart, 1);
		UpdateText();

		int2 newCaretPos = GetCaretFromChar(delStart);		
		PlaceCaret(newCaretPos);
	}	

	//std::wcout << mText << std::endl;
}

void TextEdit::DeletePreChar()
{
	size_t delStart = GetCharFromCaret(mCaretPos);
	if (delStart > 0)
	{
		auto c = mText[delStart-1];

		mText.erase(delStart-1, 1);
		UpdateText();

		int2 newCaretPos = GetCaretFromChar(delStart-1);		
		PlaceCaret(newCaretPos);
	}

	//std::wcout << mText << std::endl;
}

void TextEdit::DrawBackground( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	int2 screenPos = GetScreenPosition();

	Rectanglef destRect;
	IntRect sourceRectL, sourceRectM, sourceRectR;

	float topHeight = (float)mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top].Height;
	float bottomHeight = (float)mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom].Height;

	float zOrder = GetDepthLayer();

	// Draw Top
	sourceRectL = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Left];
	sourceRectM = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top];
	sourceRectR = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Right];

	destRect.X = mBackRect.Left();
	destRect.Y = mBackRect.Top();
	destRect.Width = (float)sourceRectL.Width;
	destRect.Height = (float)sourceRectL.Height;
	spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectL, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

	destRect.X = destRect.Right();
	destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
	spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectM, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

	destRect.X = destRect.Right();
	destRect.Width = (float)sourceRectR.Width;
	spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectR, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

	if (mBackRect.Height > sourceRectL.Height)
	{
		// Draw Middle
		sourceRectL = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Left];
		sourceRectM = mTextEditStyle->StyleStates[UI_State_Normal].TexRegion;
		sourceRectR = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Right];

		destRect.X = (float)screenPos.X();
		destRect.Y = destRect.Bottom();
		destRect.Width = (float)sourceRectL.Width;
		destRect.Height = mSize.Y() - (topHeight + bottomHeight);
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectL, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectM, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)sourceRectR.Width;
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectR, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}

	if (mSize.Y() > sourceRectL.Height)
	{
		// Draw Bottom
		sourceRectL = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Left];
		sourceRectM = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom];
		sourceRectR = mTextEditStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Right];

		destRect.X = (float)screenPos.X();
		destRect.Y = destRect.Bottom();
		destRect.Width = (float)sourceRectL.Width;
		if (mSize.Y() > topHeight + bottomHeight)
			destRect.Height = bottomHeight;
		else 
		{
			destRect.Y = screenPos.Y() + topHeight;
			destRect.Height = mSize.Y() - bottomHeight;
		}
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectL, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectM, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)sourceRectR.Width;
		spriteBatch.Draw(mTextEditStyle->StyleTex, destRect, &sourceRectR, mTextEditStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}
}

void TextEdit::OnResize()
{
	UpdateRect();

	for (UIElement* child : mChildren)
		child->OnResize();
}



}