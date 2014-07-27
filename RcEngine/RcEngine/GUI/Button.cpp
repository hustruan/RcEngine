#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputSystem.h>

namespace RcEngine {

const String Button::StyleName("Button Style");

Button::Button()
	:/* mPressedOffset(1, 2),
	 mHoverOffset(-1, -2),*/
	  mPressedOffset(0, 0),
	  mHoverOffset(0, 0),
	  mRepeatDelay(1.0f),
	  mRepeatRate(0.0f),
	  mRepeatTimer(0.0f)
{

}

Button::~Button()
{

}

void Button::Update( float dt )
{
	if (!mHovering && mPressed)
		SetPressed(false);
	
	// Send repeat events if pressed
	if (mPressed && mRepeatRate > 0.0f)
	{
		mRepeatTimer -= dt;
		if (mRepeatTimer <= 0.0f)
		{
			mRepeatTimer += 1.0f / mRepeatRate;

			if (!EventButtonClicked.empty())
				EventButtonClicked();
		}
	}
}

void Button::SetPressed( bool pressed )
{
	mPressed = pressed;
}

bool Button::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void Button::SetClickedEventRepeat( float delayTime, float rate )
{
	 mRepeatDelay = (std::max)(delayTime, 0.0f);
	 mRepeatRate = (std::max)(rate, 0.0f);
}

bool Button::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		SetPressed(true);
		mHovering = true;

		return true;
	}

	return false;
}

bool Button::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			SetPressed(false);

			if (IsInside(screenPos, true))
			{
				if (!EventButtonClicked.empty())
					EventButtonClicked();
			}	
			else
				mHovering = false;

			return true;
		}
	}

	return false;
}

void Button::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	UIElementState uiState = UI_State_Normal;
	
	float offsetX = 0.0f;
	float offsetY = 0.0f;

	if (mVisible == false)
		uiState = UI_State_Hidden;
	else if (mEnabled == false)
		uiState = UI_State_Disable;
	else if (mPressed)
	{
		uiState = UI_State_Pressed;

		offsetX += mPressedOffset.X();
		offsetY += mPressedOffset.Y();
	}
	else if (mHovering)
	{
		uiState = UI_State_Hover;

		offsetX += mHoverOffset.X();
		offsetY += mHoverOffset.Y();
	}
	else if ( HasFocus() )
		uiState = UI_State_Focus;
	
	int2 screenPos = GetScreenPosition();

	float zOrder = GetDepthLayer();

	Rectanglef btnRegion(screenPos.X() + offsetX, screenPos.Y() + offsetY, (float)mSize.X(), (float)mSize.Y());
	mStyle->DrawNinePatch(spriteBatch, uiState, btnRegion, zOrder);

	if (mText.length())
		mStyle->Font->DrawString(spriteBatchFont, mText, mSize.Y() * 0.7f, AlignCenter, btnRegion, mStyle->ForeColor, zOrder);

	// Reset hovering for next frame
	mHovering = false;
}

void Button::InitGuiStyle( const GuiSkin::StyleMap* styles )
{
	if (!styles)
	{
		// Use default
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defalutSkin->Button;
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(StyleName);
		mStyle = iter->second;
	}

	// if don't use nine patch, set default size
	if (!mStyle->StyleStates[UI_State_Normal].HasOtherPatch())
	{
		int2 size;
		size.X() = mStyle->StyleStates[UI_State_Normal].TexRegion.Width;
		size.Y() = mStyle->StyleStates[UI_State_Normal].TexRegion.Height;
		SetSize(size);
	}
}

void Button::SetText( const std::wstring& txt )
{
	mText = txt;
}




}
