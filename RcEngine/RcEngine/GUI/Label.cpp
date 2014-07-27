#include <GUI/Label.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Core/Exception.h>

namespace RcEngine {

const String Label::StyleName("Label Style");

Label::Label()
	: mTextAlignment(AlignCenter),
	  mTextColor(ColorRGBA::Black)
{

}

Label::~Label()
{

}


void Label::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mFont)
		return;
	
	float zOrder = GetDepthLayer();

	int2 screenPos = GetScreenPosition();

	Rectanglef rect((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	mFont->DrawString(spriteBatchFont, mText, mFontSize, mTextAlignment, rect, mTextColor, zOrder);
}

void Label::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (styles)
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(StyleName);
		mStyle = iter->second;
	}
	else
	{
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();	
		mStyle = &defaultSkin->Label;
	}

	mFont = mStyle->Font;
	mFontSize = mStyle->FontSize;
	mTextColor = mStyle->ForeColor;
}


}