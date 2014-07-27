#include <GUI/RadioButton.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputSystem.h>

namespace RcEngine {

const String RadioButton::RadioStyleName("RadioButton");

RadioButton::RadioButton()
	: mChecked(false),
	  mStyle(nullptr),
	  mPressed(mPressed),
	  mCanUncheck(true)
{

}

RadioButton::~RadioButton()
{
}

void RadioButton::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{
		GuiSkin::StyleMap::const_iterator iter;
		iter = styles->find(RadioStyleName);
		mStyle = iter->second;
	}
	else
	{
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defaultSkin->RadioButton;
	}

	mSize.X() = mStyle->StyleStates[UI_State_Normal].TexRegion.Width;
	mSize.Y() = mStyle->StyleStates[UI_State_Normal].TexRegion.Height;
}

void RadioButton::Update( float dt )
{
	if (!mHovering && mPressed)
		mPressed = false;
}

void RadioButton::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	float zOrder = GetDepthLayer();

	int2 screenPos = GetScreenPosition();

	Rectanglef destRect;

	destRect.X = (float)screenPos.X();
	destRect.Y = (float)screenPos.Y();
	destRect.Width = destRect.Height = float(mSize.Y());

	if (mChecked)
	{
		spriteBatch.Draw(mStyle->StyleTex, destRect, &mStyle->StyleStates[UI_State_Pressed].TexRegion, mStyle->StyleStates[UI_State_Pressed].TexColor, zOrder);
	}
	else
	{
		spriteBatch.Draw(mStyle->StyleTex, destRect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}

	// Text
	destRect.SetLeft( destRect.Right() + 0.25f * destRect.Width );
	destRect.SetRight( float(screenPos.X() + mSize.X()) );

	destRect.Y = (float)screenPos.Y();
	destRect.Height = float(mSize.Y());

	mStyle->Font->DrawString(spriteBatchFont, mText, float(mSize.Y() * 1.2f), AlignLeft | AlignVCenter, destRect, mStyle->ForeColor, zOrder);

	// Reset hovering for next frame
	mHovering = false;
}

void RadioButton::SetText( const std::wstring& text )
{
	mText = text;
}

void RadioButton::Toggle()
{
	if (mChecked && mCanUncheck)
		mChecked = false;
	else if (!mChecked)
		mChecked = true;
}

bool RadioButton::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		mPressed = true;
		mHovering = true;

		return true;
	}

	return false;
}

bool RadioButton::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			mPressed = false;

			if (IsInside(screenPos, true))
			{
				Toggle();

				if (!EventButtonClicked.empty())
					EventButtonClicked(this);
			}	
			else
				mHovering = false;

			return true;
		}	
	}

	return false;
}

bool RadioButton::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

//----------------------------------------------------------------------------------------------
RadioButtonGroup::RadioButtonGroup(const std::wstring& name)
	: mSelectedIndex(-1), mGroupName(name)
{

}

RadioButtonGroup::~RadioButtonGroup()
{

}

RadioButton* RadioButtonGroup::CheckedButton() const
{
	for (RadioButton* btn : mRadioButtons)
	{
		if (btn->mChecked)
			return btn;
	}

	return nullptr;
}

int32_t RadioButtonGroup::CheckedIndex() const
{
	auto iter = mRadioButtons.begin();
	int32_t index = 0;

	while (iter != mRadioButtons.end())
	{
		if ( (*iter)->mChecked )
			return index;

		++index;
		++iter;
	}

	return -1;
}

void RadioButtonGroup::AddButton( RadioButton* button )
{
	button->EventButtonClicked.bind(this, &RadioButtonGroup::HandleButtonClicked);
	button->mCanUncheck = false;

	if (button->IsChecked())
		mSelectedIndex = mRadioButtons.size();

	mRadioButtons.push_back(button);
}

void RadioButtonGroup::AddButton( RadioButton* button, int32_t idx )
{
	button->EventButtonClicked.bind(this, &RadioButtonGroup::HandleButtonClicked);
	button->mCanUncheck = false;

	auto where = mRadioButtons.begin();
	std::advance(where, idx);

	if (button->IsChecked())
		mSelectedIndex = idx;

	mRadioButtons.insert(where, button);
}


void RadioButtonGroup::HandleButtonClicked( RadioButton* radioBtn )
{      
	int32_t checkIndex = -1;
	for (size_t i = 0; i < mRadioButtons.size(); ++i)
	{
		if (mRadioButtons[i] == radioBtn)
		{
			checkIndex = i;
			break;
		}
	}

	if(checkIndex != mSelectedIndex)
	{
		if (!EventSelectionChanged.empty())
			EventSelectionChanged(radioBtn->mText);
	}

	mSelectedIndex = checkIndex;

	for (size_t i = 0; i < mRadioButtons.size(); i++)
	{
		if (i != checkIndex)
			mRadioButtons[i]->mChecked = false;
	}
}


}