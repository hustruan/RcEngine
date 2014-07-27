#ifndef ScrollBar_h__
#define ScrollBar_h__

#include <GUI/UIElement.h>
#include <GUI/Slider.h>

namespace RcEngine {

class _ApiExport ScrollBar : public UIElement
{
public:

	// Scroll value changed event
	typedef fastdelegate::FastDelegate1<int32_t> ValueChangedEventHandler;
	ValueChangedEventHandler EventValueChanged;
	
	/**
	 * ScrollBar styles name
	 */
	static const String ForwardStyleName;
	static const String BackStyleName;
	static const String ThumbStyleName;
	static const String TrackStyleName;

public:
	ScrollBar(UIOrientation orient);
	virtual ~ScrollBar();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual bool CanHaveFocus() const;

	virtual void OnResize();

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void OnHover(const int2& screenPos);
	virtual void OnDragBegin(const int2& screenPos, uint32_t buttons);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual bool OnMouseWheel( int32_t delta );
	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool HasCombinedFocus() const;

	/**
	 * In most case, there is no need to call SetSize, just keep the default HScrollBar's height or
	 * VScrollBar's Width in skin texture. So only need to set track length.
	 */
	void SetTrackLength(int32_t length);
	int32_t GetTrackExtext() const									{ return mTrackExtent; }

	/**
	 * The size of the visible area of the scrollable content.
	 */
	void SetScrollableSize(int32_t size); 

	UIOrientation GetOrientation() const							{ return mOrientation; }

	void SetScrollValue(int32_t value);
	int32_t GetScrollValue() const									{ return mValue; }

	void SetScrollRange(int32_t minValue, int32_t maxValue);
	void GetScrollRange(int32_t* pMinValue, int32_t* pMaxValue);

	void SetPageStep(int32_t pageStep)								{ mPageStep = pageStep; }
	int32_t GetPageStep() const										{ return mPageStep; }
	
	void SetSingleStep(int32_t singleSize) 							{ mSingleStep = singleSize; }	
	int32_t GetSingleStep() const									{ return mSingleStep; }


	void SetScrollButtonRepeat(bool enable);

	void Scroll(int32_t delta);

protected:

	void StepBack();
	void StepForward();

	void UpdateRect();
	void UpdateThumb();

protected:

	Button* mForwardButton;
	Button* mBackButton;
	
	UIOrientation mOrientation;

	// Thumb
	int32_t mValue;
	int32_t mMinValue, mMaxValue;

	int32_t mSingleStep;
	int32_t mPageStep;

	int32_t mTrackExtent;
	int32_t mScrollableExtent;

	bool mThumbHovering;

	bool mDragThumb;
	int32_t mDragBeginValue;
	int2 mDragBeginPos;		

	IntRect mThumbRegion;
	IntRect mTrackRegion;

	GuiSkin::GuiStyle* mThumbStyle;
	GuiSkin::GuiStyle* mTrackStyle;
};


}


#endif // ScrollBar_h__
