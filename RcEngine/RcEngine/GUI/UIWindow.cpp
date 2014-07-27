#include <GUI/UIWindow.h>
#include <GUI/UIManager.h>
#include <GUI/Button.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Math/Math.h>
#include <Input/InputSystem.h>

namespace RcEngine {

static const int DEFAULT_RESIZE_BORDER = 4;
static const int2 MinimizedSize(100, 20);
static const int2 MaximizedPos(0, 0);

UIWindow::UIWindow()
	: mBorderThickness(DEFAULT_RESIZE_BORDER),
	  mWindowState(UIWindow::Normal),
	  mDragMode(UIWindow::Drag_None),
	  mMovable(true),
	  mMaximized(false),
	  mMinimized(false)
{
	mBringToFront = true;

	mCloseBtn = new Button();
	mCloseBtn->SetVisible(false);
	mCloseBtn->SetHoverOffset(int2::Zero());
	mCloseBtn->SetPressedOffset(int2::Zero());
	mCloseBtn->EventButtonClicked.bind(this, &UIWindow::Close);

	mMinimizeBtn = new Button();
	mMinimizeBtn->SetVisible(false);
	mMinimizeBtn->SetHoverOffset(int2::Zero());
	mMinimizeBtn->SetPressedOffset(int2::Zero());
	mMinimizeBtn->EventButtonClicked.bind(this, &UIWindow::Minimize);

	mMaximizeBtn = new Button();
	mMaximizeBtn->SetVisible(false);
	mMaximizeBtn->SetHoverOffset(int2::Zero());
	mMaximizeBtn->SetPressedOffset(int2::Zero());
	mMaximizeBtn->EventButtonClicked.bind(this, &UIWindow::Maximize);

	mRestoreBtn = new Button();
	mRestoreBtn->SetVisible(false);
	mRestoreBtn->SetHoverOffset(int2::Zero());
	mRestoreBtn->SetPressedOffset(int2::Zero());
	mRestoreBtn->EventButtonClicked.bind(this, &UIWindow::Restore);

	AddChild(mCloseBtn);
	AddChild(mMinimizeBtn);
	AddChild(mMaximizeBtn);
	AddChild(mRestoreBtn);

	// Set default border style
	SetBorderStyle(UIWindow::Border_Resizable);
}

UIWindow::~UIWindow()
{

}

void UIWindow::SetMovable( bool movable )
{
	mMovable = movable;
}

void UIWindow::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button != MS_LeftButton)
	{
		mDragMode = Drag_None;
		return ;
	}

	mDragBeginCurosr = screenPos;
	mDragBeginPos = GetPosition();
	mDragBeginSize = GetSize();
	mDragMode = GetDragMode(ScreenToClient(screenPos));
	//SetCursorShape(dragMode_, cursor);
}

void UIWindow::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (mDragMode == Drag_None)
		return;

	int2 delta = screenPos - mDragBeginCurosr;

	switch (mDragMode)
	{
	case Drag_Move:
		{			
			SetPosition(mDragBeginPos + delta);
		}
		break;
	case Drag_Resize_Bottom:
		{
			SetSize( int2(mDragBeginSize.X(), mDragBeginSize.Y() + delta.Y()) );
		}		
		break;
	case Drag_Resize_BottomRight:
		{
			SetSize( mDragBeginSize + delta );
		}
		break;
	case Drag_Resize_Right:
		{
			SetSize( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()) );
		}	
		break;
	case Drag_Resize_TopLeft:
		{
			SetPosition(mDragBeginPos + delta);
			SetSize(mDragBeginSize - delta);
		}
		break;
	case Drag_Resize_Top:
		{
			SetPosition( int2(mDragBeginPos.X(), mDragBeginPos.Y() + delta.Y()));
			SetSize( int2(mDragBeginSize.X(), mDragBeginSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_TopRight:
		{
			SetPosition( int2(mDragBeginPos.X(), mDragBeginPos.Y() + delta.Y()));
			SetSize( int2(mDragBeginSize.X() + delta.X(), mDragBeginSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_BottomLeft:
		{
			SetPosition( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()));
			SetSize( int2(mDragBeginSize.X() - delta.X(), mDragBeginSize.Y() + delta.Y()) );
		}
		break;
	case Drag_Resize_Left:
		{
			SetPosition( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()));
			SetSize( int2(mDragBeginSize.X() - delta.X(), mDragBeginSize.Y()) );
		}
		break;
	default:
		break;
	}

	for (UIElement* child : mChildren)
		child->OnResize();

	//SetCursorShape(dragMode_, cursor);
}

void UIWindow::OnDragEnd( const int2& position )
{
	mDragMode = Drag_None;
}

void UIWindow::Close()
{
	RemoveFromParent();
	delete this;
}

void UIWindow::Minimize()
{
	int2 minPos;
	if (UIManager::GetSingleton().GetMinimizedPosition(this, &minPos))
		mMinimizedPos = minPos;
	else 
		return;

	if (mWindowState == Normal)
	{
		mLastNormalPos = GetPosition();
		mLastNormalSize = GetSize();
	}

	mWindowState = Minimized;
	mMinimized = false;

	//mAnimationPos = float2(mPosition.X(), mPosition.Y());
	//mAnimationSize = float2(mSize.X(), mSize.Y());

	// invisible all child
	for (UIElement* child : mChildren)
		child->SetVisible(false);

	mMinimizeBtn->SetVisible(false);	

	mMaximizeBtn->SetVisible(true);
	mCloseBtn->SetVisible(true);
	mMinimizeBtn->SetVisible(true);
}

void UIWindow::Maximize()
{
	if (mWindowState == Normal)
	{
		mLastNormalPos = GetPosition();
		mLastNormalSize = GetSize();
	}

	if (mWindowState == Minimized)
	{
		for (UIElement* child : mChildren)
			child->SetVisible(true);
	}

	// Bring to topmost
	BringToFront();
	mMaximumSize = UIManager::GetSingleton().GetMaximizedSize(this);
	mWindowState = Maximized;
	mMaximized = false;
}

void UIWindow::Restore()
{
	if (mWindowState == Minimized)
	{
		for (UIElement* child : mChildren)
			child->SetVisible(true);
	}

	mWindowState = Normal;

	mMaximizeBtn->SetVisible(true);
	mRestoreBtn->SetVisible(false);
}

UIWindow::DragMode UIWindow::GetDragMode( const int2& position )
{
	DragMode mode = Drag_None;

	if (mWindowState == Maximized || mWindowState == Minimized)
		return Drag_None;

	const int2& windowSize = GetSize();
	const int32_t windowWidth = windowSize.X();
	const int32_t windowHeight = windowSize.Y();

	// Top row
	if (position.Y() < mBorderThickness)
	{
		if (mMovable)
			mode = Drag_Move;

		if (IsResizable())
		{
			mode = Drag_Resize_Top;

			if (position.X() < mBorderThickness)
				mode = Drag_Resize_TopLeft;

			if (position.X() >= windowWidth - mBorderThickness)
				mode = Drag_Resize_TopRight;
		}
	}
	// Bottom row
	else if (position.Y() >= windowHeight - mBorderThickness)
	{
		if (IsResizable())
		{
			mode = Drag_Resize_Bottom;

			if (position.X() < mBorderThickness)
				mode = Drag_Resize_BottomLeft;

			if (position.X() >= windowWidth - mBorderThickness)
				mode = Drag_Resize_BottomRight;
		}
	}
	// Middle
	else
	{
		if (IsResizable())
		{
			if (position.X() < mBorderThickness)
				mode = Drag_Resize_Left;

			if (position.X() >= windowWidth - mBorderThickness)
				mode = Drag_Resize_Right;
		}
	}

	return mode;
}

void UIWindow::Update( float delta )
{
	UpdateState(delta);
}

void UIWindow::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	DrawBorder(spriteBatch, spriteBatchFont);
}

void UIWindow::DrawBorder( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	UIElementState uiState = UI_State_Normal;

	int2 screenPos = GetScreenPosition();
	//printf("DrawBorder: %d, %d\n", screenPos.X(), screenPos.Y());

	Rectanglef destRect;
	IntRect sourceRectL, sourceRectM, sourceRectR;

	float topHeight = (float)mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top].Height;
	float bottomHeight = (float)mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom].Height;

	float zOrder = GetDepthLayer();

	// Draw Top
	sourceRectL = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Left];
	sourceRectM = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top];
	sourceRectR = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top_Right];

	destRect.X = (float)screenPos.X();
	destRect.Y = (float)screenPos.Y();
	destRect.Width = (float)sourceRectL.Width;
	destRect.Height = (float)sourceRectL.Height;
	spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectL, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

	destRect.X = destRect.Right();
	destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
	spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectM, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	
	// Draw Title
	{
		Rectanglef rect = destRect;

		rect.Width = (std::min)(rect.Width, mSize.X() - 52.0f - sourceRectL.Width);

		uint32_t align = AlignTop | AlignHCenter;;
		if (mWindowState == Minimized)
			align = AlignTop | AlignLeft;

		//std::wstring title = mTitle + std::to_wstring(mPriority);

		mStyle->Font->DrawString(spriteBatchFont, mTitle, (float)mBorderThickness, align, rect, ColorRGBA::Black, zOrder);
	}
	

	destRect.X = destRect.Right();
	destRect.Width = (float)sourceRectR.Width;
	spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectR, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	
	if (mSize.Y() > topHeight +  bottomHeight)
	{
		// Draw Middle
		sourceRectL = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Left];
		sourceRectM = mStyle->StyleStates[UI_State_Normal].TexRegion;
		sourceRectR = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Right];

		destRect.X = (float)screenPos.X();
		destRect.Y = destRect.Bottom();
		destRect.Width = (float)sourceRectL.Width;
		destRect.Height = mSize.Y() - (topHeight + bottomHeight);
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectL, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectM, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)sourceRectR.Width;
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectR, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}

	if (mSize.Y() > topHeight +  bottomHeight)
	{
		// Draw Bottom
		sourceRectL = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Left];
		sourceRectM = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom];
		sourceRectR = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom_Right];

		float topHeight = (float)mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top].Height;
		float bottomHeight = (float)mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Bottom].Height;

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
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectL, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)mSize.X() - (sourceRectL.Width + sourceRectR.Width);
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectM, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)sourceRectR.Width;
		spriteBatch.Draw(mStyle->StyleTex, destRect, &sourceRectR, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}
  
}

void UIWindow::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (styles)
	{
	}
	else
	{
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle =&defalutSkin->WindowBorder;
		mBorderThickness = mStyle->StyleStates[UI_State_Normal].OtherPatch[NP_Top].Height;

		GuiSkin::StyleMap styleMap;
		styleMap[Button::StyleName] = &defalutSkin->WindowCloseBtn;
		mCloseBtn->InitGuiStyle(&styleMap);

		styleMap[Button::StyleName] = &defalutSkin->WindowMaximizeBtn;
		mMaximizeBtn->InitGuiStyle(&styleMap);

		styleMap[Button::StyleName] = &defalutSkin->WindowMinimizeBtn;
		mMinimizeBtn->InitGuiStyle(&styleMap);

		styleMap[Button::StyleName] = &defalutSkin->WindowRestoreBtn;
		mRestoreBtn->InitGuiStyle(&styleMap);
	}
}

void UIWindow::SetBorderThickness( int32_t thickness )
{
	mBorderThickness = thickness;
	UpdateRect();
}

void UIWindow::UpdateRect()
{
	int32_t width = mSize.X();

	mCloseBtn->SetPosition(int2(width - 22, 4));
	mMaximizeBtn->SetPosition(int2(width - 37, 4));
	mMinimizeBtn->SetPosition(int2(width - 52, 4));
}

void UIWindow::SetBorderStyle( BorderStyle style )
{
	switch (style)
	{
	case Border_None:
		{
			mCloseBtn->SetVisible(false);
			mMinimizeBtn->SetVisible(false);
			mMaximizeBtn->SetVisible(false);
			mRestoreBtn->SetVisible(false);

			SetBorderThickness(0);
		}
		break;

	case Border_Fixed:
		{
			mCloseBtn->SetVisible(true);
			mMinimizeBtn->SetVisible(false);
			mMaximizeBtn->SetVisible(false);
			mRestoreBtn->SetVisible(false);
		}
		break;

	case Border_Resizable:
	default:
		{
			mCloseBtn->SetVisible(true);
			mMinimizeBtn->SetVisible(true);
			mMaximizeBtn->SetVisible(true);
			mRestoreBtn->SetVisible(false);
		}
		break;
	}
}

void UIWindow::UpdateState(float dt)
{
	 int2 diff, advance;

	 //Restore the window to its original size and position
	 if (mWindowState == UIWindow::Normal && (mMaximized || mMinimized))
	 { 
		 diff = mLastNormalPos - mPosition;
		 advance = int2(diff.X() / 5, diff.Y() / 5);
		 if(advance.X() != 0 || advance.Y() != 0)
			 SetPosition(mPosition + advance);  
		 else 
			 SetPosition(mLastNormalPos);

		 diff = mLastNormalSize - mSize;
		 advance = int2(diff.X() / 5, diff.Y() / 5);
		 if(advance.X() != 0 || advance.Y() != 0)
			 SetSize(mSize + advance);
		 else 
			 SetSize(mLastNormalSize);	
		
		 for (UIElement* child : mChildren)
			 child->OnResize();

		 if (mPosition == mLastNormalPos && mSize == mLastNormalSize)
		 {	
			 mMinimized = false;
			 mMinimized = false;	
		 }
	 }
	 //Minimize the window
	 else if (mWindowState == UIWindow::Minimized && !mMinimized)
	 {
		 //float2 diff = float2(mMinimizedPos.X() - mAnimationPos.X(), mMinimizedPos.Y() - mAnimationPos.Y());
		 //float2 advance = diff * 5.0f * dt;

		 //if (LengthSquared(diff) > 4.0f)
		 //{
			// mAnimationPos += advance;
			// SetPosition(int2((int32_t)mAnimationPos.X(), (int32_t)mAnimationPos.Y()));  
		 //}
		 //else
		 //{
			// mAnimationPos = float2(mMinimizedPos.X(), mMinimizedPos.Y());
			// SetPosition(mMinimizedPos);
		 //}

		 //diff = float2(MinimizedSize.X() - mSize.X(), MinimizedSize.Y() - mSize.Y());
		 //advance = diff * 5.0f * dt;
		 //if (LengthSquared(diff) > 4.0f)
		 //{
			//mAnimationSize += advance;
			//SetSize(int2((int32_t)mAnimationSize.X(), (int32_t)mAnimationSize.Y()));
		 //}
		 //else
		 //{
			// mAnimationSize = float2(MinimizedSize.X(), MinimizedSize.Y());
			// SetSize(MinimizedSize);
		 //}
	 
		 diff = mMinimizedPos - mPosition;
		 advance = int2(diff.X() / 5, diff.Y() / 5);

	     if(advance.X() != 0 || advance.Y() != 0)
			SetPosition(mPosition + int2((int32_t)advance.X(), (int32_t)advance.Y()));  
		 else 
			SetPosition(mMinimizedPos);
			

		 diff = MinimizedSize - mSize;
		 advance = int2(diff.X() / 5, diff.Y() / 5);
		 if(advance.X() != 0 || advance.Y() != 0)
			 SetSize(mSize + int2((int32_t)advance.X(), (int32_t)advance.Y()));
		 else 
			 SetSize(MinimizedSize);	

		 for (UIElement* child : mChildren)
			 child->OnResize();

		 if (mPosition == mMinimizedPos && mSize == MinimizedSize)
		 {	
			 mMinimized = true;

			 // Replace minimize button with restore button.
			 mRestoreBtn->SetPosition(mMinimizeBtn->GetPosition());
			 mRestoreBtn->SetVisible(true);
			 mMinimizeBtn->SetVisible(false);
		 }
	 }
	 //Maximize the window
	 else if (mWindowState == UIWindow::Maximized && !mMaximized)
	 {

		 diff = MaximizedPos - mPosition;
		 advance = int2(diff.X() / 5, diff.Y() / 5);
		 if(advance.X() != 0 || advance.Y() != 0)
			 SetPosition(mPosition + int2((int32_t)advance.X(), (int32_t)advance.Y()));  
		 else 
			 SetPosition(MaximizedPos);

		 diff = mMaximumSize - mSize;
		 advance = int2(diff.X() / 5, diff.Y() / 5);
		 if(advance.X() != 0 || advance.Y() != 0)
			 SetSize(mSize + int2((int32_t)advance.X(), (int32_t)advance.Y()));
		 else 
			 SetSize(mMaximumSize);	

		 for (UIElement* child : mChildren)
			 child->OnResize();

		 if (mPosition == MaximizedPos && mSize == mMaximumSize)
		 {
			 mMaximized = true;

			 // Replace minimize button with restore button.
			 mRestoreBtn->SetPosition(mMaximizeBtn->GetPosition());
			 mRestoreBtn->SetVisible(true);
			 mMaximizeBtn->SetVisible(false);
		 }
	 }
}

bool UIWindow::CanHaveFocus() const
{
	return mVisible && mEnabled;
}





}