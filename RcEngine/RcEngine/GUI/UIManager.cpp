#include <GUI/UIManager.h>
#include <GUI/UIElement.h>
#include <GUI/GuiSkin.h>
#include <GUI/UIWindow.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/Font.h>
#include <Graphics/Material.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>
#include <Input/InputEvent.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>

namespace RcEngine {

SINGLETON_DECL(UIManager)

UIManager::UIManager()
	: mDragElement(nullptr), mFocusElement(nullptr), mRootElement(nullptr), mMainWindow(nullptr),
	  mInitialize(false), mDefaultSkin(nullptr)
{

}

UIManager::~UIManager()
{
	SAFE_DELETE(mRootElement);
	SAFE_DELETE(mDefaultSkin);
	mFocusElement = mDragElement = nullptr;
}


void UIManager::OnGraphicsInitialize()
{
	if (!mInitialize)
	{
		//Keep track of main window
		mMainWindow = Context::GetSingleton().GetApplication().GetMainWindow();

		mRootElement = new UIElement();
		mRootElement->SetName("UIRoot");
		mRootElement->SetSize(int2(mMainWindow->GetWidth(), mMainWindow->GetHeight()));

		ResourceManager& resMan = ResourceManager::GetSingleton();

		mFont = std::static_pointer_cast<Font>(resMan.GetResourceByName(RT_Font,"Consolas Regular", "General"));

		mSpriteBatch = std::make_shared<SpriteBatch>();

		mSpriteBatchFont = std::make_shared<SpriteBatch>( 
			std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "Font.material.xml", "General")) );

		mInitialize = true;
	}
}

void UIManager::OnGraphicsFinalize()
{
	mSpriteBatch.reset();
	mFont.reset();
	mSpriteBatchFont.reset();
	SAFE_DELETE(mDefaultSkin);
}

void UIManager::OnWindowResize( uint32_t width, uint32_t height )
{
	if (mRootElement)
	{
		mRootElement->SetSize(int2(width, height));
	}

	if (!mMinimizeWindows.empty())
	{
		for (UIWindow* win : mMinimizeWindows)
		{
			int2 previousPos = win->GetPosition();
			win->SetPosition(int2(previousPos.X(), previousPos.Y() + height - (int32_t)mHeight));
		}
	}

	mWidth = width;
	mHeight = height;
}

void UIManager::SetFocusElement( UIElement* element )
{
	if (element)
	{
		if (mFocusElement == element)
			return;

		// Search for an element in the hierarchy that can alter focus. If none found, exit
		element = GetFocusableElement(element);
		if (!element)
			return;
	}

	// Remove focus from the old element
	if (mFocusElement)
		mFocusElement = nullptr;

	if (element && element->CanHaveFocus())
	{
		mFocusElement = element;
	}
}

UIElement* UIManager::GetElementAtPoint( const int2& pos )
{
	UIElement* result = 0;
	GetElementAtPoint(result, mRootElement, pos);
	return result;
}

UIElement* UIManager::GetFocusableElement( UIElement* element )
{
	while (element)
	{
		if (element->CanHaveFocus())
			break;
		element = element->GetParent();
	}
	return element;
}

void UIManager::Update( float delta )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	int2 mousePos = InputSystem::GetSingleton().GetMousePos();

	if (mouseVisible)
	{
		UIElement* element = GetElementAtPoint(mousePos);

		if (element && !mDragElement)
			element->OnHover(mousePos);			
	}

	Update(mRootElement, delta);

	// Remove all window not in Minimized state
	mMinimizeWindows.erase( std::remove_if( mMinimizeWindows.begin(), mMinimizeWindows.end(), [](UIWindow* win){
								return win->GetWindowState() != UIWindow::Minimized;}), 
		                    mMinimizeWindows.end() ); 
							
	/*auto restore = std::find_if(mMinimizeWindows.begin(), mMinimizeWindows.end(), [](UIWindow* win){ return win->GetWindowState() != UIWindow::Minimized;});
	
	if (restore != mMinimizeWindows.end())
	{
		auto next = restore;
		next++;
		while(next != mMinimizeWindows.end())
		{
			int2 currPos = (*next)->GetPosition();
			(*next)->SetPosition(currPos - int2(100, 0));
			next++;
		}
		
		mMinimizeWindows.erase(restore);
	}*/
}


void UIManager::Update( UIElement* element, float dt )
{
	element->Update(dt);
	
	for (auto& child : element->GetChildren())
		Update(child, dt);
}

bool UIManager::OnEvent( const InputEvent& event )
{
	bool eventConsumed = false;

	switch (event.EventType)
	{
	case InputEventType::MouseButtonDown:
		{
			eventConsumed = HandleMousePress(
				int2(int(event.MouseButton.x), int(event.MouseButton.y)), 
				event.MouseButton.button);
		}
		break;
	case InputEventType::MouseButtonUp:
		{
			eventConsumed = HandleMouseRelease(
				int2(int(event.MouseButton.x), int(event.MouseButton.y)), 
				event.MouseButton.button);
		}
		break;
	case InputEventType::MouseMove:
		{
			eventConsumed = HandleMouseMove(
				int2(int(event.MouseMove.x), int(event.MouseMove.y)), 
				event.MouseMove.buttons);
		}
		break;
	case InputEventType::MouseWheel:
		{
			eventConsumed = HandleMouseWheel(
				int2(int(event.MouseWheel.x), int(event.MouseWheel.y)), 
				event.MouseWheel.wheel);
		}
		break;
	case InputEventType::KeyDown:
		{
			eventConsumed = HandleKeyPress(event.Key.key);
		}
		break;
	case InputEventType::KeyUp:
		{
			eventConsumed = HandleKeyRelease(event.Key.key);
		}
		break;
	case InputEventType::Char:
		{
			eventConsumed = HandleTextInput(event.Char.unicode);
		}
		break;
	default:
		break;
	}

	return eventConsumed;

}

bool UIManager::HandleMousePress( const int2& screenPos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;
	
	bool eventConsumed = false;

	// Get UIElemet current mouse cover
	UIElement* element = GetElementAtPoint(screenPos);

	if (element && element->IsVisible() && element->IsEnabled())
	{
		 // Handle focusing, bringing to front
		if (button == MS_LeftButton)
		{
			SetFocusElement(element);
			element->BringToFront();
		}

		eventConsumed = element->OnMouseButtonPress(screenPos, button);

		// if button press doesn't consume event, handle drag 
		if (!eventConsumed && !mDragElement && button == MS_LeftButton)
		{
			mDragElement = element;
			element->OnDragBegin(screenPos, button);
		}

		// Mouse position is in UI region, So there is no need to pass event to other Game Objects
		eventConsumed = true;
	}
	else
	{
		// If clicked over no element, or a disabled element, lose focus
		SetFocusElement(NULL);
		eventConsumed = false;
	}	

	return eventConsumed;
}

bool UIManager::HandleMouseRelease( const int2& screenPos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement)
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragEnd(screenPos);
		}

		mDragElement = nullptr;
		eventConsumed = true;
	}
	else
	{
		if (mFocusElement)
		{
			eventConsumed = mFocusElement->OnMouseButtonRelease(screenPos, button);
			eventConsumed = true;
		}
	}

	return eventConsumed;
}

bool UIManager::HandleMouseMove( const int2& screenPos, uint32_t buttons )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement && (buttons & InputEventType::LeftButtonMask) )
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragMove(screenPos, buttons);
		}
		else
		{
			mDragElement->OnDragEnd(screenPos);
			mDragElement = nullptr;
		}	

		// Event consumed 
		eventConsumed = true;
	}

	return eventConsumed;
}

bool UIManager::HandleMouseWheel( const int2& screenPos, int32_t delta )
{
	bool eventConsumed = false;

	if (mFocusElement)
	{
		eventConsumed =  mFocusElement->OnMouseWheel(delta);
	}
	else
	{
		bool mouseVisible = mMainWindow->IsMouseVisible();

		 // If no element has actual focus, get the element at cursor
		if (mouseVisible)
		{
			UIElement* element = GetElementAtPoint(screenPos);

			if (element && element->IsVisible())
			{
				element->OnMouseWheel(delta);

				// mouse pos is in GUI region
				eventConsumed =  true;
			}
		}
	}

	return eventConsumed;
}

bool UIManager::HandleKeyPress( uint16_t key )
{
	bool eventConsumed = false;

	UIElement* element = GetFocusElement();
	if (element)
	{
		eventConsumed = true;

		// Switch focus between focusable elements in the same top level window
		//if (key == KC_Tab)
		//{
		//	UIElement* topLevel = element->GetParent();
		//	while(topLevel && topLevel->GetParent() != mRootElement)
		//		topLevel = topLevel->GetParent();

		//	if (topLevel)
		//	{
		//		vector<UIElement*> children;
		//		topLevel->FlattenChildren(children);

		//		for (size_t i = 0; i < children.size(); ++i)
		//		{
		//			if ( element == children[i] )
		//			{
		//				size_t next = (i + 1) % children.size();

		//				while( next != i )
		//				{
		//					UIElement* nextElement = children[next];
		//					FocusMode childFocusPolicy = nextElement->GetFocusMode();

		//					if (childFocusPolicy & FM_TabFocus)
		//					{
		//						SetFocusElement(nextElement);
		//						return eventConsumed;
		//					}
		//					else
		//						next =  (next + 1) % children.size();
		//				}

		//				// no other focusable control
		//				break;
		//			}			
		//		}
		//	}
		//}
		//else // If none of the special keys, pass the key to the focused element			
			element->OnKeyPress(key);		
	}

	return eventConsumed;
}

bool UIManager::HandleKeyRelease( uint16_t key )
{
	return false;
}

bool UIManager::HandleTextInput( uint16_t unicode )
{
	bool eventConsumed = false;

	UIElement* element = GetFocusElement();
	if (element)
	{
		eventConsumed = element->OnTextInput(unicode);
		eventConsumed = true;
	}

	return eventConsumed;
}

void UIManager::GetElementAtPoint( UIElement*& result, UIElement* current, const int2& pos )
{
	if (!current)
		return;

	current->SortChildren();

	for ( UIElement* element : current->GetChildren() )
	{
		bool hasChildren = (element->GetNumChildren() > 0);

		if (element->IsVisible() && element->IsEnabled())
		{
			if (element->IsInside(pos, true))
			{
				result = element;

				if (hasChildren)
					GetElementAtPoint(result, element, pos);	
			}
			else
			{
				if (hasChildren && element->CanChildOutside())
				{
					if (element->IsInsideCombined(pos, true))
						GetElementAtPoint(result, element, pos);	
				}
			}
		}
	}
}

void UIManager::Render()
{
	if (mRootElement)
	{
		mSpriteBatchFont->Begin();
		mSpriteBatch->Begin();

		const int2& rootSize = mRootElement->GetSize();
		RenderUIElement(mRootElement, IntRect(0, 0, rootSize.X(), rootSize.Y()));
				
		mSpriteBatch->End();
		mSpriteBatchFont->End();
		
		mSpriteBatch->Flush();
		mSpriteBatchFont->Flush();
	}
}

void UIManager::RenderUIElement( UIElement* element, const IntRect& currentScissor )
{
	// If parent container is not visible, not draw children
	if (!element->IsVisible())
		return;

	// Draw container first
	element->Draw(*mSpriteBatch, *mSpriteBatchFont);

	element->SortChildren();
	std::vector<UIElement*>& children = element->GetChildren();
	for (UIElement* child : children)
	{
		RenderUIElement(child, currentScissor);
	}	
}

//GuiSkin* UIManager::GetDefaultSkin()
//{
//	// todo: xml gui skin
//	if (!mDefaultSkin)
//	{
//		mDefaultSkin = new GuiSkin;
//
//		mDefaultSkin->mFont = mFont;
//		mDefaultSkin->mFontSize = 25.0f;
//
//		ResourceManager& resMan = ResourceManager::GetSingleton();
//		mDefaultSkin->mSkinTexAtlas = std::static_pointer_cast<TextureResource>(
//			resMan.GetResourceByName(RT_Texture,"dxutcontrols.dds", "General"))->GetTexture();
//
//		// Button
//		for (int i = 0; i < UI_State_Count; ++i)
//		{
//			mDefaultSkin->Button.StyleStates[i].TexRegion = IntRect(0, 0, 136, 54);
//			mDefaultSkin->Button.StyleStates[i].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);
//		}
//
//		mDefaultSkin->Button.StyleStates[UI_State_Hover].TexRegion = IntRect(136, 0, 252 - 136, 54);
//		mDefaultSkin->Button.Font = mFont;
//		mDefaultSkin->Button.FontSize = mDefaultSkin->mFontSize;
//		mDefaultSkin->Button.BackColor = ColorRGBA(1, 1, 1, 100.0f / 255);
//		mDefaultSkin->Button.ForeColor = ColorRGBA(1, 1, 1, 200.0f / 255);
//		mDefaultSkin->Button.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//		// Slider 
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Normal].TexRegion.SetLeft(1);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Normal].TexRegion.SetRight(93);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Normal].TexRegion.SetTop(187);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Normal].TexRegion.SetBottom(228);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Normal].TexColor = ColorRGBA(0.5, 0.5, 0.5, 150.0f / 255);
//
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Hover].TexRegion.SetLeft(1);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Hover].TexRegion.SetRight(93);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Hover].TexRegion.SetTop(187);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Hover].TexRegion.SetBottom(228);
//		mDefaultSkin->HSliderTrack.StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);
//		mDefaultSkin->HSliderTrack.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Normal].TexRegion.SetLeft(151);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Normal].TexRegion.SetRight(192);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Normal].TexRegion.SetTop(193);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Normal].TexRegion.SetBottom(234);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Hover].TexRegion.SetLeft(151);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Hover].TexRegion.SetRight(192);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Hover].TexRegion.SetTop(193);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Hover].TexRegion.SetBottom(234);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 0, 1, 255.0 / 255);
//
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Pressed].TexRegion.SetLeft(151);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Pressed].TexRegion.SetRight(192);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Pressed].TexRegion.SetTop(193);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Pressed].TexRegion.SetBottom(234);
//		mDefaultSkin->HSliderThumb.StyleStates[UI_State_Pressed].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//		mDefaultSkin->HSliderThumb.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//		// Scroll bar
//		for (int i = 0; i < UI_State_Count; ++i)
//		{
//			mDefaultSkin->HSrollBack.StyleStates[i].TexRegion = IntRect(196, 192, 22, 20);
//			mDefaultSkin->HSrollBack.StyleStates[i].TexColor = ColorRGBA(1, 1, 1, 1);
//
//			mDefaultSkin->HSrollForward.StyleStates[i].TexRegion = IntRect(196, 223, 22, 21);
//			mDefaultSkin->HSrollForward.StyleStates[i].TexColor = ColorRGBA(1, 1, 1, 1);
//		}
//		mDefaultSkin->HSrollBack.StyleStates[UI_State_Hover].TexColor = ColorRGBA(0, 1, 1, 1);
//		mDefaultSkin->HSrollForward.StyleStates[UI_State_Hover].TexColor = ColorRGBA(0, 1, 1, 1);
//
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Normal].TexRegion.SetLeft(196);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Normal].TexRegion.SetRight(218);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Normal].TexRegion.SetTop(212);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Normal].TexRegion.SetBottom(223);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 1);
//
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Hover].TexRegion.SetLeft(196);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Hover].TexRegion.SetRight(218);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Hover].TexRegion.SetTop(212);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Hover].TexRegion.SetBottom(223);
//		mDefaultSkin->HScrollTrack.StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 1, 1, 1);
//		mDefaultSkin->HScrollTrack.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Normal].TexRegion = IntRect(220, 192, 18, 42);
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 2, 2, 1);
//
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Hover].TexRegion = IntRect(220, 192, 18, 42);
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 0, 1, 1);
//
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Pressed].TexRegion = IntRect(220, 192, 18, 42);
//		mDefaultSkin->HSrollThumb.StyleStates[UI_State_Pressed].TexColor = ColorRGBA(1, 1, 1, 1);
//		
//
//		mDefaultSkin->HSrollBack.StyleTex = mDefaultSkin->mSkinTexAtlas;
//		mDefaultSkin->HSrollForward.StyleTex = mDefaultSkin->mSkinTexAtlas;
//		mDefaultSkin->HScrollTrack.StyleTex = mDefaultSkin->mSkinTexAtlas;
//		mDefaultSkin->HSrollThumb.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//
//		// List Box
//		mDefaultSkin->ListBox.StyleStates[UI_State_Normal].TexRegion = IntRect(13, 123, 228, 37);
//		mDefaultSkin->ListBox.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//
//		mDefaultSkin->ListBox.StyleStates[UI_State_Hover].TexRegion = IntRect(16, 166, 224, 17);
//		mDefaultSkin->ListBox.StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//		
//		mDefaultSkin->ListBox.Font = mFont;
//		mDefaultSkin->ListBox.FontSize = mDefaultSkin->mFontSize;
//		mDefaultSkin->ListBox.BackColor = ColorRGBA(0, 0, 0, 255.0 / 255);
//		mDefaultSkin->ListBox.ForeColor = ColorRGBA(0, 0, 0, 255.0 / 255);
//		mDefaultSkin->ListBox.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//
//		// ComboBox
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Normal].TexRegion = IntRect(7, 81, 240, 42);
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Normal].TexColor = ColorRGBA(200/ 255.0f, 200/ 255.0f, 200/ 255.0f, 150 /255.0f);
//
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Focus].TexRegion = IntRect(7, 81, 240, 42);
//	    mDefaultSkin->ComboDropButton.StyleStates[UI_State_Focus].TexColor = ColorRGBA(230/ 255.0f, 230/ 255.0f, 230/ 255.0f, 170 /255.0f);
//
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Disable].TexRegion = IntRect(7, 81, 240, 42);
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Disable].TexColor = ColorRGBA(200/ 255.0f, 200/ 255.0f, 200/ 255.0f, 70 /255.0f);
//
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Hover].TexRegion = IntRect(7, 81, 240, 42);
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Hover].TexColor = ColorRGBA(200/ 255.0f, 200/ 255.0f, 200/ 255.0f, 150 /255.0f);
//
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Pressed].TexRegion = IntRect(7, 81, 240, 42);
//		mDefaultSkin->ComboDropButton.StyleStates[UI_State_Pressed].TexColor = ColorRGBA(200/ 255.0f, 200/ 255.0f, 200/ 255.0f, 150 /255.0f);
//
//		mDefaultSkin->ComboDropButton.Font = mFont;
//		mDefaultSkin->ComboDropButton.FontSize = mDefaultSkin->mFontSize;
//		mDefaultSkin->ComboDropButton.BackColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//		mDefaultSkin->ComboDropButton.ForeColor = ColorRGBA(1, 1, 1, 255.0 / 255);
//		mDefaultSkin->ComboDropButton.StyleTex = mDefaultSkin->mSkinTexAtlas;
//
//		shared_ptr<Texture> windowTex = std::static_pointer_cast<TextureResource>(
//			resMan.GetResourceByName(RT_Texture,"xWinForm.png", "General"))->GetTexture();
//
//		// UIWindow
//		mDefaultSkin->WindowBorder.StyleStates[UI_State_Normal].TexRegion = IntRect(97, 121, 1, 1);
//		mDefaultSkin->WindowBorder.StyleTex = windowTex;
//		mDefaultSkin->WindowBorder.Font = mFont;
//
//		IntRect*& otherPath = mDefaultSkin->WindowBorder.StyleStates[UI_State_Normal].OtherPatch;
//		otherPath = new IntRect[8];
//
//		otherPath[NP_Top_Left] = IntRect(39, 47, 15, 20);
//		otherPath[NP_Top] = IntRect(62, 71, 1, 20);
//		otherPath[NP_Top_Right] = IntRect(24, 83, 15, 20);
//		otherPath[NP_Right] = IntRect(43, 124, 15, 1);
//		otherPath[NP_Bottom_Right] = IntRect(66, 3, 15, 20);
//		otherPath[NP_Bottom] = IntRect(67, 71, 1, 20);
//		otherPath[NP_Bottom_Left] = IntRect(43, 83, 15, 20);
//		otherPath[NP_Left] = IntRect(62, 124, 15, 1);	
//	
//		// Close Button
//		for (size_t i = 0; i < UI_State_Count; ++i)
//		{
//			mDefaultSkin->WindowCloseBtn.StyleStates[i].TexRegion = IntRect(68, 47, 13, 13);
//			mDefaultSkin->WindowCloseBtn.StyleStates[i].TexColor = ColorRGBA(182.0f/255, 182.0f/255, 182.0f/255, 1.0f);
//			
//			// Maximize
//			mDefaultSkin->WindowMaximizeBtn.StyleStates[i].TexRegion = IntRect(77, 106, 13, 13);
//			mDefaultSkin->WindowMaximizeBtn.StyleStates[i].TexColor = ColorRGBA(182.0f/255, 182.0f/255, 182.0f/255, 1.0f);
//			
//			// Minimize
//			mDefaultSkin->WindowMinimizeBtn.StyleStates[i].TexRegion = IntRect(60, 107, 13, 13);
//			mDefaultSkin->WindowMinimizeBtn.StyleStates[i].TexColor = ColorRGBA(182.0f/255, 182.0f/255, 182.0f/255, 1.0f);
//			mDefaultSkin->WindowMinimizeBtn.StyleTex = windowTex;
//
//			// Restore
//			mDefaultSkin->WindowRestoreBtn.StyleStates[i].TexRegion = IntRect(43, 107, 13, 13);
//			mDefaultSkin->WindowRestoreBtn.StyleStates[i].TexColor = ColorRGBA(182.0f/255, 182.0f/255, 182.0f/255, 1.0f);
//			
//		}
//
//		mDefaultSkin->WindowCloseBtn.StyleStates[UI_State_Hover].TexColor = ColorRGBA(200.0f/255, 200.0f/255, 200.0f/255, 1.0f);
//		mDefaultSkin->WindowMaximizeBtn.StyleStates[UI_State_Hover].TexColor = ColorRGBA(200.0f/255, 200.0f/255, 200.0f/255, 1.0f);
//		mDefaultSkin->WindowRestoreBtn.StyleStates[UI_State_Hover].TexColor = ColorRGBA(200.0f/255, 200.0f/255, 200.0f/255, 1.0f);
//
//		mDefaultSkin->WindowCloseBtn.StyleTex = windowTex;
//		mDefaultSkin->WindowMaximizeBtn.StyleTex = windowTex;
//		mDefaultSkin->WindowRestoreBtn.StyleTex = windowTex;
//
//
//		// Check Box
//		mDefaultSkin->CheckBox.StyleStates[UI_State_Normal].TexRegion = IntRect(47, 27, 16, 16); 
//		mDefaultSkin->CheckBox.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 1);
//		mDefaultSkin->CheckBox.StyleStates[UI_State_Pressed].TexRegion = IntRect(67, 27, 16, 16); 
//		mDefaultSkin->CheckBox.StyleStates[UI_State_Pressed].TexColor = ColorRGBA(1, 1, 1, 1);
//		mDefaultSkin->CheckBox.Font = mFont;
//		mDefaultSkin->CheckBox.FontSize = mDefaultSkin->mFontSize;
//		mDefaultSkin->CheckBox.BackColor = ColorRGBA::Black;
//		mDefaultSkin->CheckBox.ForeColor = ColorRGBA::Black;
//		mDefaultSkin->CheckBox.StyleTex = windowTex;
//
//		// Text Edit
//		{		
//			mDefaultSkin->TextEdit.StyleTex = windowTex;
//			mDefaultSkin->TextEdit.FontSize = mDefaultSkin->mFontSize;
//			mDefaultSkin->TextEdit.Font = mFont;
//			mDefaultSkin->TextEdit.StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 1);
//
//			IntRect*& otherPath = mDefaultSkin->TextEdit.StyleStates[UI_State_Normal].OtherPatch;
//			otherPath = new IntRect[8];
//
//			otherPath[NP_Top_Left] = IntRect(58, 47, 5, 10);
//			otherPath[NP_Top] = IntRect(63, 47, 1, 10);
//			otherPath[NP_Top_Right] = IntRect(63, 47, -5, 10);
//
//			otherPath[NP_Left] = IntRect(58, 57, 5, 1);	
//			mDefaultSkin->TextEdit.StyleStates[UI_State_Normal].TexRegion = IntRect(63, 57, 1, 1);
//			otherPath[NP_Right] = IntRect(63, 57, -5, 1);
//
//			otherPath[NP_Bottom_Left] = IntRect(58, 57, 5, -10);	
//			otherPath[NP_Bottom] = IntRect(63, 57, 1, -10);	
//			otherPath[NP_Bottom_Right] = IntRect(63, 57, -5, -10);
//		}
//		
//	}
//
//
//	return mDefaultSkin;
//}

GuiSkin* UIManager::GetDefaultSkin()
{
	// todo: xml gui skin
	if (!mDefaultSkin)
	{
		mDefaultSkin = new GuiSkin;
		mDefaultSkin->TestLoad();
	}

	return mDefaultSkin;
}


bool UIManager::GetMinimizedPosition( UIWindow* window, int2* pos )
{
	for (UIWindow* win : mMinimizeWindows)
	{
		if (win != window && win->IsMinimizing())
			return false;
	}

	mMinimizeWindows.push_back(window);
	
	const int32_t width = mRootElement->GetSize().X();
	const int32_t height = mRootElement->GetSize().Y();

	//using MinimumSize from the Form Class (100 by 40)
	for (int32_t y = height - 20; y > 0; y -= 20)
	{
		for (int32_t x = 0; x < width - 99; x += 100)
		{
			bool isOccupied = false;
			for (UIWindow* win : mMinimizeWindows)
			{
				if (win != window && win->IsVisible() && win->GetPosition().X() == x && win->GetPosition().Y() == y)
				{
					isOccupied = true;
					break;
				}
			}

			if (!isOccupied)
			{
				pos->X() = x;
				pos->Y() = y;
				return true;
			}
		}
	}

	return false;
}

int2 UIManager::GetMaximizedSize( UIWindow* window ) const
{
	int2 maxSize = mRootElement->GetSize();

	for (UIWindow* win : mMinimizeWindows)
	{
		if (win != window && win->IsVisible() && win->GetWindowState() == UIWindow::Minimized)
		{
			int32_t top = win->GetPosition().Y();
			if (top < maxSize.Y())
				maxSize.Y() = top;
		}
	}

	return maxSize;
}

}

