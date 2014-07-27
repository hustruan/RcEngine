#include <GUI/CheckBox.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>

namespace RcEngine {

const String CheckBox::StyleName("CheckBox::CheckBox");

CheckBox::CheckBox( )
	: mCheched(false), mPressed(false), mStyle(nullptr)
{

}

CheckBox::~CheckBox()
{

}

void CheckBox::Update( float delta )
{
	if (!mHovering && mPressed)
		mPressed = false;
}

void CheckBox::SetChecked( bool enalbe )
{
	if (mCheched != enalbe)
	{
		mCheched = enalbe;

		if (!EventStateChanged.empty())
			EventStateChanged(mCheched);
	}
}

bool CheckBox::IsChecked() const
{
	return mCheched;
}

bool CheckBox::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool CheckBox::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			mPressed = false;

			if (IsInside(screenPos, true))
			{
				SetChecked(!mCheched);
			}	
			else
				mHovering = false;

			return true;
		}	
	}

	return false;
}

bool CheckBox::OnMouseButtonPress( const int2& position, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		mPressed = true;
		mHovering = true;

		return true;
	}

	return false;
}

void CheckBox::SetText( const std::wstring& text )
{
	mText = text;
}

void CheckBox::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	float zOrder = GetDepthLayer();

	int2 screenPos = GetScreenPosition();
	
	Rectanglef destRect;

	destRect.X = (float)screenPos.X();
	destRect.Y = (float)screenPos.Y();
	destRect.Width = destRect.Height = float(mSize.Y());

	UIElementState uiState = (mCheched ? UI_State_Pressed : UI_State_Normal);
	spriteBatch.Draw(mStyle->StyleTex, destRect, &mStyle->StyleStates[uiState].TexRegion, mStyle->StyleStates[uiState].TexColor, zOrder);
		
	// Text
	destRect.SetLeft( destRect.Right() + 0.25f * destRect.Width );
	destRect.SetRight( float(screenPos.X() + mSize.X()) );

	destRect.Y = (float)screenPos.Y();
	destRect.Height = float(mSize.Y());

	mStyle->Font->DrawString(spriteBatchFont, mText, float(mSize.Y() * 1.2f), AlignLeft | AlignVCenter, destRect, mStyle->ForeColor, zOrder);

	// Reset hovering for next frame
	mHovering = false;
}

void CheckBox::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (!styles)
	{
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defaultSkin->CheckBox;
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(CheckBox::StyleName);
		mStyle = iter->second;
	}

	mSize.X() = mStyle->StyleStates[UI_State_Normal].TexRegion.Width;
	mSize.Y() = mStyle->StyleStates[UI_State_Normal].TexRegion.Height;
}


}

