#include <GUI/ScrollBar.h>
#include <GUI/Slider.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Core/Exception.h>

namespace RcEngine {

const String ScrollBar::ForwardStyleName("ScrollBar::ForwardStyleName");
const String ScrollBar::BackStyleName("ScrollBar::BackStyleName");
const String ScrollBar::ThumbStyleName("ScrollBar::ThumbStyleName");
const String ScrollBar::TrackStyleName("ScrollBar::TrackStyleName");

static const float DEFAULT_SCROLL_STEP = 0.1f;
static const float DEFAULT_REPEAT_DELAY = 0.8f;
static const float DEFAULT_REPEAT_RATE = 5.0f;

ScrollBar::ScrollBar( UIOrientation orient )
	: mOrientation(orient),
	  mThumbHovering(false), 
	  mDragThumb(false),
	  mMinValue(0), 
	  mMaxValue(100), 
	  mValue(0),  
	  mSingleStep(1),
	  mTrackExtent(0),
	  mScrollableExtent(0),
	  mThumbStyle(nullptr), 
	  mTrackStyle(nullptr)
{
	mForwardButton = new Button;
	mForwardButton->SetName("ScrollBar Forward Button");
	mForwardButton->SetPressedOffset(int2::Zero());
	mForwardButton->SetHoverOffset(int2::Zero());
	mForwardButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, DEFAULT_REPEAT_RATE);

	mBackButton = new Button;
	mBackButton->SetName("ScrollBar Back Button");
	mBackButton->SetPressedOffset(int2::Zero());
	mBackButton->SetHoverOffset(int2::Zero());
	mBackButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, DEFAULT_REPEAT_RATE);

	AddChild(mForwardButton);
	AddChild(mBackButton);

	// Events
	mForwardButton->EventButtonClicked.bind(this, &ScrollBar::StepForward);
	mBackButton->EventButtonClicked.bind(this, &ScrollBar::StepBack);
}

ScrollBar::~ScrollBar()
{

}

bool ScrollBar::CanHaveFocus() const
{
	return mEnabled && mVisible;
}

bool ScrollBar::HasCombinedFocus() const
{
	if (!mVisible || !mEnabled)
		return false;

	UIElement* focusElem = UIManager::GetSingleton().GetFocusElement();
	bool hasFocus = (focusElem == this || focusElem == mForwardButton || focusElem == mBackButton);
	return hasFocus;
}

void ScrollBar::SetScrollButtonRepeat( bool enable )
{
	if (enable)
	{
		mForwardButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, DEFAULT_REPEAT_RATE);
		mBackButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, DEFAULT_REPEAT_RATE);
	}
	else 
	{
		mForwardButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, 0.0f);
		mBackButton->SetClickedEventRepeat(DEFAULT_REPEAT_DELAY, 0.0f);
	}
}

void ScrollBar::OnResize()
{
	UpdateRect();
}

void ScrollBar::UpdateRect()
{
	int2 screenPos = GetScreenPosition();

	if (mOrientation == UI_Horizontal)
	{
		int32_t size = GetSize().Y();
		mBackButton->SetPosition(int2(0, 0));
		mBackButton->SetSize(int2(size, size));

		mForwardButton->SetPosition(int2(mSize.X() - size, 0));
		mForwardButton->SetSize(int2(size, size));

		mTrackRegion.X = screenPos.X() + size; 
		mTrackRegion.Width = mSize.X() - size - size;
		mTrackRegion.Y = screenPos.Y();
		mTrackRegion.Height = mSize.Y();
	}
	else
	{
		int32_t size = GetSize().X();

		mBackButton->SetPosition(int2(0, 0));
		mBackButton->SetSize(int2(size, size));

		mForwardButton->SetPosition(int2(0, mSize.Y() - size));
		mForwardButton->SetSize(int2(size, size));

		mTrackRegion.X = screenPos.X(); 
		mTrackRegion.Width = mSize.X();
		mTrackRegion.Y = screenPos.Y() + size;
		mTrackRegion.Height = mSize.Y() - size - size;
	}

	UpdateThumb();
}

void ScrollBar::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if( styles == nullptr )
	{
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();

		if (mOrientation == UI_Horizontal)
		{
			GuiSkin::StyleMap styleMap;
			styleMap[Button::StyleName] = &defalutSkin->HSrollForward;
			mForwardButton->InitGuiStyle(&styleMap);

			styleMap[Button::StyleName] = &defalutSkin->HSrollBack;
			mBackButton->InitGuiStyle(&styleMap);

			mTrackStyle = &defalutSkin->HScrollTrack;
			mThumbStyle = &defalutSkin->HSrollThumb;

			mTrackExtent = defalutSkin->HSrollForward.StyleStates[UI_State_Normal].TexRegion.Height;
		}
		else
		{
			GuiSkin::StyleMap styleMap;
			styleMap[Button::StyleName] = &defalutSkin->VSrollForward;
			mForwardButton->InitGuiStyle(&styleMap);

			styleMap[Button::StyleName] = &defalutSkin->VSrollBack;
			mBackButton->InitGuiStyle(&styleMap);

			mTrackStyle = &defalutSkin->VScrollTrack;
			mThumbStyle = &defalutSkin->VSrollThumb;

			mTrackExtent = defalutSkin->VSrollForward.StyleStates[UI_State_Normal].TexRegion.Width;
		}
		
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter;
		
		GuiSkin::StyleMap styleMap;

		iter = styles->find(ForwardStyleName);	
		styleMap[Button::StyleName] = iter->second;
		mForwardButton->InitGuiStyle(&styleMap);

		iter = styles->find(BackStyleName);	
		styleMap[Button::StyleName] = iter->second;
		mBackButton->InitGuiStyle(&styleMap);

		iter = styles->find(TrackStyleName);
		mTrackStyle = iter->second;	

		iter = styles->find(ThumbStyleName);
		mThumbStyle = iter->second;
	}
}

void ScrollBar::StepBack()
{
	SetScrollValue(mValue - mSingleStep);
}

void ScrollBar::StepForward()
{
	SetScrollValue(mValue + mSingleStep);
}


void ScrollBar::Scroll( int32_t delta )
{
	SetScrollValue(mValue + delta);
}

void ScrollBar::GetScrollRange( int32_t* pMinValue, int32_t* pMaxValue )
{
	if (pMinValue) *pMinValue = mMinValue;
	if (pMaxValue) *pMaxValue = mMaxValue;
}

void ScrollBar::SetScrollRange( int32_t minValue, int32_t maxValue )
{
	mMaxValue = maxValue;
	mMinValue = minValue;
	mValue = Clamp(mValue, mMinValue, mMaxValue);

	UpdateThumb();
}

void ScrollBar::SetScrollValue( int32_t value )
{
	value = Clamp(value, mMinValue, mMaxValue);

	if (value == mValue)
		return;

	mValue = value;
	UpdateThumb();

	if (!EventValueChanged.empty())
		EventValueChanged(mValue);
}

void ScrollBar::UpdateThumb()
{
	float ratio = float(mValue - mMinValue) / (mMaxValue - mMinValue);

	if (mOrientation == UI_Horizontal)
	{		
		mThumbRegion.Width = mThumbRegion.Height = mSize.Y();
		mThumbRegion.X = int32_t( mTrackRegion.X + ratio * ( mTrackRegion.Width - mThumbRegion.Width ) );
		mThumbRegion.Y = mTrackRegion.Y; 
	}
	else
	{
		float thumbSize = ((float)mScrollableExtent/(mMaxValue - mMinValue + mScrollableExtent)) * mTrackRegion.Height;

		mThumbRegion.Height = (int32_t)thumbSize;
		mThumbRegion.Width = mSize.X();

		mThumbRegion.X = mTrackRegion.X; 
		mThumbRegion.Y = int32_t( mTrackRegion.Y + ratio * ( mTrackRegion.Height - mThumbRegion.Height ) );
	}
}

void ScrollBar::Update( float delta )
{
	if (mDragThumb)
	{
		mHovering = true;
		mThumbHovering = true;
	}
}

void ScrollBar::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	UIElementState uiThumbState = UI_State_Normal;

	// Thumb don't have focus state
	if (mVisible == false)
		uiThumbState = UI_State_Hidden;
	else if (mEnabled == false)
		uiThumbState = UI_State_Disable;
	else if (mDragThumb)
		uiThumbState = UI_State_Pressed;
	else if (mThumbHovering)
		uiThumbState = UI_State_Hover;
	else 
		uiThumbState = UI_State_Normal;

	const GuiSkin::SytleImage& stateStyle = mThumbStyle->StyleStates[uiThumbState];

	// Draw order: track, thumb
	Rectanglef trackRegion((float)mTrackRegion.X, (float)mTrackRegion.Y, (float)mTrackRegion.Width, (float)mTrackRegion.Height);
	Rectanglef thumbRegion((float)mThumbRegion.X, (float)mThumbRegion.Y, (float)mThumbRegion.Width, (float)mThumbRegion.Height);
	
	float zOrder = GetDepthLayer();

	// Track
	if (mDragThumb || mHovering)
		spriteBatch.Draw(mTrackStyle->StyleTex, trackRegion, &mTrackStyle->StyleStates[UI_State_Hover].TexRegion, mTrackStyle->StyleStates[UI_State_Hover].TexColor, zOrder);	
	else
		spriteBatch.Draw(mTrackStyle->StyleTex, trackRegion, &mTrackStyle->StyleStates[UI_State_Normal].TexRegion, mTrackStyle->StyleStates[UI_State_Normal].TexColor, zOrder);	

	// Thumb
	spriteBatch.Draw(mThumbStyle->StyleTex, thumbRegion, &stateStyle.TexRegion, stateStyle.TexColor, zOrder);		

	mHovering = false;
	mThumbHovering = false;
}

void ScrollBar::OnHover( const int2& screenPos )
{
	UIElement::OnHover(screenPos);
	mThumbHovering = mThumbRegion.Contains(screenPos.X(), screenPos.Y());
}

void ScrollBar::OnDragBegin( const int2& screenPos, uint32_t buttons )
{
	mDragBeginPos = screenPos;

	// Test if in thumb region
	mDragThumb = mThumbRegion.Contains(screenPos.X(), screenPos.Y());
	mDragBeginValue = mValue;
}

void ScrollBar::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (!mDragThumb)
		return; 

	int2 delta = screenPos - mDragBeginPos;

	int32_t newValue = mDragBeginValue;

	if (mOrientation == UI_Horizontal)
		newValue += int32_t( delta.X() / float(mTrackRegion.Width- mThumbRegion.Width) * float(mMaxValue - mMinValue) );
	else
		newValue += int32_t( delta.Y() / float(mTrackRegion.Height - mThumbRegion.Height) * float(mMaxValue - mMinValue) );

	SetScrollValue(newValue);
}

void ScrollBar::OnDragEnd( const int2& screenPos )
{
	mDragThumb = false;
}

bool ScrollBar::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	// if in thumb region, OnDragBegin will handle it.
	if (mThumbRegion.Contains(screenPos.X(), screenPos.Y()))
		return false;

	if (mTrackRegion.Contains(screenPos.X(), screenPos.Y()))
	{
		if (mOrientation == UI_Horizontal)
		{
			if (screenPos.X() > mThumbRegion.X)
				StepForward();
			else
				StepBack();
		}
		else
		{
			if (screenPos.Y() > mThumbRegion.Y)
				StepForward();
			else
				StepBack();
		}

		eventConsumed = true;
	}

	return eventConsumed;
}

bool ScrollBar::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	if (mTrackRegion.Contains(screenPos.X(), screenPos.Y()))
	{
		eventConsumed = true;
	}

	return eventConsumed;
}

bool ScrollBar::OnMouseWheel( int32_t delta )
{
	Scroll(-delta);

	return true;
}

void ScrollBar::SetTrackLength( int32_t length )
{
	if (mTrackExtent == 0)
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Init Gui Style first", "ScrollBar::SetTrackLength");

	if (mOrientation == UI_Horizontal)
		SetSize(int2(length, mTrackExtent));
	else 
		SetSize(int2(mTrackExtent, length));
}

void ScrollBar::SetScrollableSize( int32_t size )
{
	mScrollableExtent = size;
}



}