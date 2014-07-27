#include <GUI/UIElement.h>
#include <GUI/UIManager.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

UIElement::UIElement()
	: mParent(nullptr), 
	  mPositionDirty(false), 
	  mHovering(false), 
	  mVisible(true), 
	  mEnabled(true),
	  mPriority(UI_NormalPriority),
	  mSortOrderDirty(false),
	  mChildOutside(false),
	  mBringToFront(false),
	  mPosition(int2::Zero()),
	  mSize(int2::Zero()),
	  mMinSize(int2::Zero()), mMaxSize(INT_MAX, INT_MAX)
{


}

UIElement::~UIElement()
{
	for (UIElement* child : mChildren)
		delete child;
	mChildren.clear();
}

void UIElement::SetPosition( const int2& position )
{
	if (mPosition != position)
	{
		mPosition = position;
		MarkDirty();
	}
}


void UIElement::SetVisible( bool visible )
{
	mVisible = visible;

	// Set all child to visible
	for (UIElement* child : mChildren)
		child->SetVisible(visible);
}


void UIElement::SetSize( const int2& size )
{
	int2 validateSize;
	validateSize.X() = Clamp(size.X(), mMinSize.X(), mMaxSize.X());
	validateSize.Y() = Clamp(size.Y(), mMinSize.Y(), mMaxSize.Y());

	if (mSize != validateSize)
	{
		mSize = validateSize;
		MarkDirty();
		UpdateRect();
	}
}

int2 UIElement::GetScreenPosition()
{
	if (mPositionDirty)
	{
		int2 pos = mPosition;

		const UIElement* current = mParent;

		while(current)
		{
			pos += current->GetPosition();
			current = current->mParent;
		}

		mScreenPosition = pos;
		mPositionDirty = false;
	}

	return mScreenPosition;
}

IntRect UIElement::GetRect() const
{
	return IntRect(mPosition.X(), mPosition.Y(), mSize.X(), mSize.Y());
}

IntRect UIElement::GetScreenRect()
{
	int2 screenPos = GetScreenPosition();
	return IntRect(screenPos.X(), screenPos.Y(), mSize.X(), mSize.Y());
}

IntRect UIElement::GetCombinedScreenRect()
{
	IntRect rect = GetScreenRect();

	for (UIElement* child : mChildren)
		rect = Union(rect, child->GetCombinedScreenRect());

	return rect;
}



void UIElement::MarkDirty()
{
	mPositionDirty = true;

	for (UIElement* child : mChildren)
		child->MarkDirty();
}

uint32_t UIElement::GetNumChildren( bool recursive /*= false*/ ) const
{
	uint32_t allChildren =  mChildren.size();

	if (recursive)
	{
		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			allChildren += (*iter)->GetNumChildren(true);
	}

	return allChildren;
}

UIElement* UIElement::GetChild( const String& name, bool recursive /*= false*/ ) const
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			return *iter;
		}

		if (recursive)
		{
			UIElement* element =  (*iter)->GetChild(name, true);
			if (element)
			{
				return element;
			}
		}
	}

	return nullptr;
}

UIElement* UIElement::GetChild( uint32_t index ) const
{
	return index < mChildren.size() ? mChildren[index] : nullptr;
}

RcEngine::int2 UIElement::ScreenToClient( const int2& screenPosition )
{
	return screenPosition - GetScreenPosition();
}

RcEngine::int2 UIElement::ClientToScreen( const int2& position )
{
	return position + GetScreenPosition();
}

bool UIElement::IsInside(int2 position, bool isScreen )
{
	if (isScreen)
		position = ScreenToClient(position);
	return position.X() >= 0 && position.Y() >= 0 && position.X() < mSize.X() && position.Y() < mSize.Y();
}

bool UIElement::IsInsideCombined( int2 position, bool isScreen )
{
	if (!isScreen)
		position = ClientToScreen(position);

	return GetCombinedScreenRect().Contains(position.X(), position.Y());
}


void UIElement::FlattenChildren( std::vector<UIElement*>& children) const
{
	for (UIElement* element : mChildren)
	{
		children.push_back(element);

		if (element->GetNumChildren())
			element->FlattenChildren(children);
	}
}

void UIElement::SortChildren()
{
	if (mSortOrderDirty)
	{
		std::sort(mChildren.begin(), mChildren.end(), [](UIElement* lhs, UIElement* rhs){
					return lhs->mPriority < rhs->mPriority;});
		mSortOrderDirty = false;
	}
}


void UIElement::BringToFront()
{
	// Follow the parent chain to the top level window. If it has BringToFront mode, bring it to front now
	UIElement* root = GetRoot();
	// If element is detached from hierarchy, this must be a no-op
	if (!root)
		return;

	UIElement* ptr = this;
	while (ptr && ptr->GetParent() != root)
		ptr = ptr->GetParent();
	if (!ptr || !ptr->CanBringToFront())
		return;

	// Get the highest priority used by all other top level elements, assign that to the new front element
	// and decrease others' priority where necessary. However, take into account only input-enabled
	// elements and those which have the BringToBack flag set
	unordered_set<int32_t> usedPriorities;

	int32_t maxPriority = UI_MinPriority;
	for (UIElement* other : root->GetChildren())
	{
		if (other->IsEnabled() && other != ptr)
		{
			int32_t priority = other->GetPriority();
			usedPriorities.insert(priority);
			maxPriority = (std::max)(priority, maxPriority);
		}
	}

	if (maxPriority != UI_MinPriority && maxPriority >= ptr->GetPriority())
	{
		ptr->SetPriority(maxPriority);

		int32_t minPriority = maxPriority;
		while (usedPriorities.find(minPriority) != usedPriorities.end())
			--minPriority;

		for (UIElement* other : root->GetChildren())
		{
			int priority = other->GetPriority();

			if (other->IsEnabled()  && other != ptr && priority >= minPriority && priority <= maxPriority)
				other->SetPriority(priority - 1);
		}
	}
}

UIElement* UIElement::GetRoot() const
{
	UIElement* root = mParent;
	if (!root)
		return 0;
	while (root->GetParent())
		root = root->GetParent();
	return root;
}

void UIElement::RemoveChild( UIElement* child )
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		if ((*iter) == child)
		{
			mChildren.erase(iter);
			mSortOrderDirty = true;
			return;
		}
	}
}

void UIElement::AddChild( UIElement* child )
{
	if (child)
	{
		child->RemoveFromParent();
		child->mParent = this;

		child->MarkDirty();
		child->SetPriority(mPriority);

		child->OnResize();

		mChildren.push_back(child);
		mSortOrderDirty = true;
	}
}

void UIElement::RemoveFromParent()
{
	if (mParent)
		mParent->RemoveChild(this);
}

void UIElement::SetParent( UIElement* parent )
{
	if (parent)
		parent->AddChild(this);
}

void UIElement::SetPriority( int32_t priority )
{
	mPriority = priority;
	if (mParent)
		mParent->mSortOrderDirty = true;

	for (UIElement* child : mChildren)
		child->SetPriority(priority);
}

bool UIElement::HasFocus() const
{
	return UIManager::GetSingleton().GetFocusElement() == this;
}

bool UIElement::HasCombinedFocus() const
{
	return HasFocus();
}

void UIElement::Update( float delta )
{

}

void UIElement::OnHover( const int2& screenPos )
{
	mHovering = true;
}

bool UIElement::OnMouseWheel( int32_t delta )
{
	return false;
}

bool UIElement::OnKeyPress( uint16_t key )
{
	return false;
}

bool UIElement::OnKeyRelease( uint16_t key )
{
	return false;
}

bool UIElement::OnTextInput( uint16_t unicode )
{
	return false;
}

void UIElement::OnDragBegin( const int2& screenPos, uint32_t buttons )
{
}

void UIElement::OnDragMove( const int2& screenPos, uint32_t buttons )
{

}

void UIElement::OnDragEnd( const int2& screenPos )
{

}

void UIElement::OnResize()
{

}

bool UIElement::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	return false;
}

bool UIElement::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	return false;
}

bool UIElement::CanHaveFocus() const
{
	return false;
}

void UIElement::UpdateRect()
{

}

void UIElement::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	mHovering = false;
}

void UIElement::InitGuiStyle( const GuiSkin::StyleMap* styles )
{

}








}