#include <GUI/ListBox.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>

namespace RcEngine {

static const int32_t BorderWidth = 6;

ListBox::ListBox()
	: mSelectedIndex(-1),
	  mNumVisibleItems(0),
	  mPressed(false),
	  mMargin(5)
{
	mVertScrollBar = new ScrollBar(UI_Vertical);
	mVertScrollBar->SetScrollButtonRepeat(false);
	mVertScrollBar->SetVisible(false);

	AddChild(mVertScrollBar);
}

ListBox::~ListBox()
{
	RemoveAllItems();
}

bool ListBox::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool ListBox::HasCombinedFocus() const
{
	if (!mVisible || !mEnabled)
		return false;

	if (mVertScrollBar->HasCombinedFocus())
		return true;
	else 
		return HasFocus();
}

void ListBox::AddItem( const std::wstring& text )
{
	mItems.push_back(text);
	UpdateVScrollBar();

	if (mVertScrollBar->IsVisible())
		mVertScrollBar->SetScrollValue(mItems.size() - mNumVisibleItems);
}

void ListBox::InsertItem( int32_t index, const std::wstring& text )
{
	mItems.insert(mItems.begin() + index, text);
	UpdateVScrollBar();

	if (mVertScrollBar->IsVisible() && index > mNumVisibleItems)
		mVertScrollBar->SetScrollValue(index - mNumVisibleItems);
}

void ListBox::RemoveItem( int32_t index )
{
	if (index < 0 || index >= (int32_t)mItems.size())
		return;

	mItems.erase(mItems.begin() + index);

	if( mSelectedIndex >= ( int )mItems.size() )
		mSelectedIndex = mItems.size() - 1;

	UpdateVScrollBar();
}

void ListBox::RemoveAllItems()
{
	mItems.clear();
}

void ListBox::UpdateRect()
{
	UIElement::UpdateRect();

	int2 screenPos = GetScreenPosition();

	mSelectionRegion.X = (float)screenPos.X();
	mSelectionRegion.Width = (float)mSize.X();
	mSelectionRegion.Y = (float)screenPos.Y() + mBorder[1];
	mSelectionRegion.Height = (float)mSize.Y() - mBorder[1] - mBorder[3];
	
	float oldHeigt = mSelectionRegion.Height;

	mNumVisibleItems = (int32_t)floor(mSelectionRegion.Height / mTextRowHeight + 0.5f);
	mSelectionRegion.Height = mTextRowHeight * mNumVisibleItems;

	mSize.Y() += int32_t(ceilf(mSelectionRegion.Height) - oldHeigt);
	
	UpdateVScrollBar();

	mTextRegion = mSelectionRegion;
	mTextRegion.SetLeft( mSelectionRegion.X + mBorder[0]);
	mTextRegion.SetRight( mSelectionRegion.Right() - mBorder[2]);
}

void ListBox::UpdateVScrollBar()
{
	if ((int32_t)mItems.size() > mNumVisibleItems)
	{
		mVertScrollBar->SetScrollableSize(mItems.size());

		if (mVertScrollBar->IsVisible() == false)
		{
			mVertScrollBar->SetVisible(true);
			mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 1));
			mVertScrollBar->SetSize(int2(mScrollBarWidth-1, mSize.Y()-2));
			mSelectionRegion.Width -= mScrollBarWidth;
		}
		else 
		{
			mSelectionRegion.Width -= mScrollBarWidth;
			mVertScrollBar->SetScrollRange(0, (int)mItems.size() - mNumVisibleItems);	
		}
		
	}
	else
	{
		if (mVertScrollBar->IsVisible())
		{
			mVertScrollBar->SetVisible(false);
			mSelectionRegion.Width += mScrollBarWidth;
		}
	}
}


void ListBox::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{

	}
	else
	{
		// use defualt
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mVertScrollBar->InitGuiStyle(nullptr);

		mLisBoxStyle = &defaultSkin->ListBox;

		if (mLisBoxStyle->StyleStates[UI_State_Normal].HasOtherPatch())
		{
			mBorder[0] = mLisBoxStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Left].Width;
			mBorder[1] = mLisBoxStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Left].Height;
			mBorder[2] = mLisBoxStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Right].Width;
			mBorder[3] = mLisBoxStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Right].Height;
		}
		else 
		{
			for (size_t i = 0; i < 4; ++i)
				mBorder[i] = BorderWidth;
		}
		
		mScrollBarWidth = mVertScrollBar->GetTrackExtext() + 1;
	}

	float fontScale = mLisBoxStyle->FontSize / float(mLisBoxStyle->Font->GetFontSize());
	mTextRowHeight = mLisBoxStyle->Font->GetRowHeight(fontScale);
}

void ListBox::Update( float delta )
{

}

void ListBox::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	float zOrder = GetDepthLayer();

	// Draw background first
	Rectanglef backRC((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	mLisBoxStyle->DrawNinePatch(spriteBatch, UI_State_Normal, backRC, zOrder);

	if (mItems.size())
	{
		int mimItem, maxItem;
		mVertScrollBar->GetScrollRange(&mimItem, &maxItem);

		Rectanglef rc((float)mTextRegion.X, (float)mTextRegion.Y, (float)mTextRegion.Width, (float)mTextRegion.Height);
		rc.SetBottom( rc.Y + mTextRowHeight);

		for (int i = mVertScrollBar->GetScrollValue(); i < maxItem + mNumVisibleItems; ++i)
		{		
			if (rc.Bottom() > (float)mTextRegion.Bottom())
				break;

			if( i == mSelectedIndex )
			{
				// Draw selected highlight
				Rectanglef rcSel;

				rcSel.SetLeft(mSelectionRegion.Left()+1);
				rcSel.SetRight(mSelectionRegion.Right());
				rcSel.SetTop((float)rc.Top());
				rcSel.SetBottom((float)rc.Bottom());
				
				spriteBatch.Draw(mLisBoxStyle->StyleTex, rcSel, &mLisBoxStyle->StyleStates[UI_State_Hover].TexRegion,
					mLisBoxStyle->StyleStates[UI_State_Hover].TexColor, zOrder);
			}

			// draw text
			//Rectanglef region((float)rc.X, (float)rc.Y, (float)rc.Width, (float)rc.Height);
			mLisBoxStyle->Font->DrawString(spriteBatchFont, mItems[i], mLisBoxStyle->FontSize, AlignVCenter, rc, mLisBoxStyle->ForeColor, zOrder);

			rc.Offset(0, mTextRowHeight);
		}
	}
}

void ListBox::OnResize()
{
	UpdateRect();

	for (UIElement* child : mChildren)
		child->OnResize();
}

int32_t ListBox::GetSelectedIndex() const
{
	if (mItems.size() == 0)
		return -1;
	else 
		return mSelectedIndex;
}

void ListBox::SetSelectedIndex( int32_t index, bool fromInput /*= false*/ )
{
	int32_t oldSelectedIndex = mSelectedIndex;

	mSelectedIndex = Clamp(index, 0, (int32_t)mItems.size());

	if (!EventSelection.empty())
		EventSelection(mSelectedIndex);

	if (!EventSelectionChanged.empty() && mSelectedIndex != oldSelectedIndex)
		EventSelectionChanged(mSelectedIndex);
}

bool ListBox::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;
	
	if (button == MS_LeftButton)
	{
		if (mItems.size())
			mPressed = true;

		eventConsumed = true;
	}

	return eventConsumed;
}

bool ListBox::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			if (mSelectionRegion.Contains((float)screenPos.X(), (float)screenPos.Y()))
			{
				int32_t selIndex = mVertScrollBar->GetScrollValue() + (int32_t)floorf((screenPos.Y() - mSelectionRegion.Top()) / mTextRowHeight);
				SetSelectedIndex(selIndex, true);			
			}	

			mPressed = false;
			eventConsumed = true;
		}
	}

	return eventConsumed;
}

bool ListBox::OnMouseWheel( int32_t delta )
{
	if (mVertScrollBar->IsVisible())
		mVertScrollBar->Scroll(-delta);

	return true;
}

}