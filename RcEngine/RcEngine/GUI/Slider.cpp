#include <GUI/Slider.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

const String Slider::TrackStyleName("Slider::TrackStyleName");
const String Slider::ThumbStyleName("Slider::ThumbStyleName");

Slider::Slider( UIOrientation orient )
	: mOrientation(orient),
	  mMinimum(0), 
	  mMaximum(100),
	  mValue(0),
	  mSingleStep(1),
	  mDragSlider(false),
	  mThumbHovering(false),
	  mThumbStyle(nullptr),
	  mTrackStyle(nullptr)
{

}

Slider::~Slider()
{

}

void Slider::Update( float delta )
{
	if (mDragSlider)
	{
		mHovering = true;
		mThumbHovering = true;
	}
}

void Slider::OnResize()
{
	UpdateSlider();
}

void Slider::UpdateSlider()
{
	int2 screenPos = GetScreenPosition();

	float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);

	if (mOrientation == UI_Horizontal)
	{		
		mThumbRegion.X = screenPos.X() + ratio * ( mSize.X() - mThumbRegion.Width );
		mThumbRegion.Y = (float)screenPos.Y(); 
	}
	else
	{
		mThumbRegion.X = (float)screenPos.X(); 
		mThumbRegion.Y = screenPos.Y() + ratio * ( mSize.Y() - mThumbRegion.Height );
	}
}

void Slider::UpdateRect()
{
	UpdateSlider();
}

void Slider::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	UIElementState uiThumbState = UI_State_Normal;

	// Thumb don't have focus state
	if (mVisible == false)
		uiThumbState = UI_State_Hidden;
	else if (mEnabled == false)
		uiThumbState = UI_State_Disable;
	else if (mDragSlider)
		uiThumbState = UI_State_Pressed;
	else if (mThumbHovering)
		uiThumbState = UI_State_Hover;
	else 
		uiThumbState = UI_State_Normal;

	int2 screenPos = GetScreenPosition();
	float zOrder = GetDepthLayer();

	const GuiSkin::SytleImage& stateStyle = mThumbStyle->StyleStates[uiThumbState];

	// Draw order: track, thumb
	Rectanglef trackRegion((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	
	// Track
	if (mDragSlider || mHovering)
		mTrackStyle->DrawThreePatch(spriteBatch, UI_State_Hover, mOrientation, trackRegion, zOrder);
	else
		mTrackStyle->DrawThreePatch(spriteBatch, UI_State_Normal, mOrientation, trackRegion, zOrder);

	// Thumb
	{
		Rectanglef rect = mThumbRegion;
		rect.Y = trackRegion.Y + (trackRegion.Height - mThumbRegion.Height) * 0.5f;
		spriteBatch.Draw(mThumbStyle->StyleTex, rect, &stateStyle.TexRegion, stateStyle.TexColor, zOrder);
	}
	

	mHovering = false;
	mThumbHovering = false;
}

void Slider::OnHover( const int2& screenPos )
{
	UIElement::OnHover(screenPos);
	mThumbHovering = mThumbRegion.Contains((float)screenPos.X(), (float)screenPos.Y());
}

void Slider::OnDragBegin( const int2& screenPos, uint32_t buttons )
{
	mDragBeginPos = screenPos;

	// Test if in thumb region
	mDragSlider = mThumbRegion.Contains((float)screenPos.X(), (float)screenPos.Y());
	mDragBeginValue = mValue;
}

void Slider::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (!mDragSlider)
		return; 

	int2 delta = screenPos - mDragBeginPos;
	
	int32_t newValue = mDragBeginValue;

	if (mOrientation == UI_Horizontal)
		newValue += int32_t( delta.X() / float(mSize.X() - mThumbRegion.Width) * float(mMaximum - mMinimum) );
	else
		newValue += int32_t( delta.Y() / float(mSize.Y() - mThumbRegion.Height) * float(mMaximum - mMinimum) );

	SetValueInternal(newValue, true);
}

void Slider::OnDragEnd( const int2& screenPos )
{
	mDragSlider = false;
}

void Slider::StepForward()
{
	SetValueInternal(mValue + mSingleStep, true);
}

void Slider::StepBack()
{
	SetValueInternal(mValue - mSingleStep, true);
}

bool Slider::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	// if in thumb region, OnDragBegin will handle it.
	if (mThumbRegion.Contains((float)screenPos.X(), (float)screenPos.Y()))
		return false;

	if (IsInside(screenPos, true))
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

bool Slider::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	if (IsInside(screenPos, true))
	{
		eventConsumed = true;
	}

	return eventConsumed;
}

void Slider::SetRange( int32_t min, int32_t max )
{
	mMinimum = min; 
	mMaximum = max;

	UpdateSlider();
}

void Slider::GetRange( int32_t& min, int32_t& max )
{
	min = mMinimum; max = mMaximum;
}

void Slider::SetValue( int32_t value )
{
	SetValueInternal(value, false);
}

void Slider::SetValueInternal( int32_t value, bool fromInput )
{
	value = Clamp(value, mMinimum, mMaximum);

	if (value == mValue)
		return;

	mValue = value;
	UpdateSlider();

	if (!EventValueChanged.empty() && fromInput)
		EventValueChanged(mValue);
}

void Slider::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (!styles)
	{
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();

		if (mOrientation == UI_Horizontal)
		{
			mTrackStyle = &defalutSkin->HSliderTrack;
			mThumbStyle = &defalutSkin->HSliderThumb;
		}
		else
		{
			mTrackStyle = &defalutSkin->VSliderTrack;
			mThumbStyle = &defalutSkin->VSliderThumb;
		}
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter;
		iter = styles->find(ThumbStyleName);
		mThumbStyle = iter->second;

		iter = styles->find(TrackStyleName);
		mTrackStyle = iter->second;
	}

	if (mOrientation == UI_Horizontal)
		mTrackExtent = mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Height;
	else
		mTrackExtent = mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Width;

	// Use the extent defined in skin texture
	mThumbRegion.Width = (float)mThumbStyle->StyleStates[UI_State_Normal].TexRegion.Height;
	mThumbRegion.Height = (float)mThumbStyle->StyleStates[UI_State_Normal].TexRegion.Height;
}

void Slider::SetTrackLength( int32_t length )
{
	if (mOrientation == UI_Horizontal)
		SetSize(int2(length, mTrackExtent));
	else 
		SetSize(int2(mTrackExtent, length));
}



}