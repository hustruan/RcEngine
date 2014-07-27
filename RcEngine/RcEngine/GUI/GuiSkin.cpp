#include <GUI/GuiSkin.h>
#include <IO/FileSystem.h>
#include <Graphics/Texture.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>

namespace {

using namespace RcEngine;

class GuiSkinDefs
{
public:
	GuiSkinDefs()
	{
		mNinePathDefs.insert(std::make_pair("TopLeft", NP_Top_Left));
		mNinePathDefs.insert(std::make_pair("Top",  NP_Top));
		mNinePathDefs.insert(std::make_pair("TopRight", NP_Top_Right));
		mNinePathDefs.insert(std::make_pair("Right", NP_Right));
		mNinePathDefs.insert(std::make_pair("BottomRight", NP_Bottom_Right));
		mNinePathDefs.insert(std::make_pair("Bottom", NP_Bottom));
		mNinePathDefs.insert(std::make_pair("BottomLeft", NP_Bottom_Left));
		mNinePathDefs.insert(std::make_pair("Left", NP_Left));
		mNinePathDefs.insert(std::make_pair("Fill", NP_Fill));

		mStateDefs.insert(std::make_pair("Normal", UI_State_Normal));
		mStateDefs.insert(std::make_pair("Disable", UI_State_Disable));
		mStateDefs.insert(std::make_pair("Hidden", UI_State_Hidden));
		mStateDefs.insert(std::make_pair("Focus", UI_State_Focus));
		mStateDefs.insert(std::make_pair("Hover", UI_State_Hover));
		mStateDefs.insert(std::make_pair("Pressed", UI_State_Pressed));
	}

	~GuiSkinDefs()
	{
	}

	static GuiSkinDefs& GetInstance()
	{
		static GuiSkinDefs singleton;
		return singleton;
	}


	void ReadStateStyles(XMLNodePtr node, GuiSkin::SytleImage& styleImage)
	{
		XMLNodePtr patchNode = node->FirstNode("NinePatch");
		if (patchNode)
		{
			styleImage.OtherPatch = new IntRect[8];
			for ( ; patchNode; patchNode = patchNode->NextSibling("NinePatch") )
			{
				String patchName = patchNode->AttributeString("name", "");

				if (mNinePathDefs[patchName] == NP_Fill)
				{
					styleImage.TexRegion.X = patchNode->AttributeInt("xPos", 0);
					styleImage.TexRegion.Y = patchNode->AttributeInt("yPos", 0);
					styleImage.TexRegion.Width = patchNode->AttributeInt("width", 0);
					styleImage.TexRegion.Height = patchNode->AttributeInt("height", 0);
				}
				else
				{
					styleImage.OtherPatch[mNinePathDefs[patchName]].X = patchNode->AttributeInt("xPos", 0);
					styleImage.OtherPatch[mNinePathDefs[patchName]].Y = patchNode->AttributeInt("yPos", 0);
					styleImage.OtherPatch[mNinePathDefs[patchName]].Width = patchNode->AttributeInt("width", 0);
					styleImage.OtherPatch[mNinePathDefs[patchName]].Height = patchNode->AttributeInt("height", 0);
				}
				
			}
		}
		else
		{
			styleImage.TexRegion.X = node->AttributeInt("xPos", 0);
			styleImage.TexRegion.Y = node->AttributeInt("yPos", 0);
			styleImage.TexRegion.Width = node->AttributeInt("width", 0);
			styleImage.TexRegion.Height = node->AttributeInt("height", 0);
		}
	}

private:
	unordered_map<String, NinePatch> mNinePathDefs;

public:
	unordered_map<String, UIElementState> mStateDefs;
};

ColorRGBA ReadColor(const String& str)
{
	ColorRGBA color;
	if ( sscanf(str.c_str(), "%f %f %f %f", &color[0], &color[1], &color[2], &color[3]) != 4 )
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Input is not ColorRGBA", "ReadColor");

	return color;
}

}

namespace RcEngine {

//GuiSkin::GuiSkin( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
//	: Resource(RT_GuiSkin, creator, handle, name, group)
//{
//
//}
GuiSkin::GuiSkin()
{

}

GuiSkin::~GuiSkin()
{

}

void GuiSkin::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();
	GuiSkinDefs& skinDefs = GuiSkinDefs::GetInstance();

	String mName = "xWinForm.skin.xml";
	String mGroup = "General";

	shared_ptr<Stream> matStream = fileSystem.OpenStream(mName, mGroup);
	Stream& source = *matStream;	

	XMLDoc doc;
	XMLNodePtr node, root = doc.Parse(source);

	mName = root->AttributeString("name", "");
	
	String textureFile = root->AttributeString("texture", "");
	if (fileSystem.Exits(textureFile))
	{
		mSkinTexAtlas = std::static_pointer_cast<TextureResource>(
			resMan.GetResourceByName(RT_Texture, textureFile, "General"))->GetTexture();
	}


	// Font
	{
		node = root->FirstNode("Font");
		String fontName = node->AttributeString("name", "");
		mFont = std::static_pointer_cast<Font>(resMan.GetResourceByName(RT_Font, fontName, "General"));
		mFont->Load();
	}

	if ( node = root->FirstNode("Label") )
	{
		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			Label.Font = mFont;
			Label.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		XMLNodePtr foreNode = node->FirstNode("ForeColor");
		if (foreNode)
		{
			Label.ForeColor = ReadColor(foreNode->AttributeString("color", ""));
		}

		Label.StyleTex = mSkinTexAtlas;
	}
	
	// Check Box
	if( node = root->FirstNode("CheckBox") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];

			skinDefs.ReadStateStyles(stateNode, CheckBox.StyleStates[uiState]);

			CheckBox.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			CheckBox.Font = mFont;
			CheckBox.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		XMLNodePtr backNode = node->FirstNode("BackColor");
		if (backNode)
		{
			CheckBox.BackColor = ReadColor(backNode->AttributeString("color", ""));
		}

		XMLNodePtr foreNode = node->FirstNode("ForeColor");
		if (foreNode)
		{
			CheckBox.ForeColor = ReadColor(foreNode->AttributeString("color", ""));
		}

		CheckBox.StyleTex = mSkinTexAtlas;
	}

	// RadioButton
	if( node = root->FirstNode("RadioButton") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];

			skinDefs.ReadStateStyles(stateNode, RadioButton.StyleStates[uiState]);

			RadioButton.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			RadioButton.Font = mFont;
			RadioButton.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		XMLNodePtr backNode = node->FirstNode("BackColor");
		if (backNode)
		{
			RadioButton.BackColor = ReadColor(backNode->AttributeString("color", ""));
		}

		XMLNodePtr foreNode = node->FirstNode("ForeColor");
		if (foreNode)
		{
			RadioButton.ForeColor = ReadColor(foreNode->AttributeString("color", ""));
		}

		RadioButton.StyleTex = mSkinTexAtlas;
	}

	// Button
	if( node = root->FirstNode("Button") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];

			skinDefs.ReadStateStyles(stateNode, Button.StyleStates[uiState]);

			Button.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			Button.Font = mFont;
			Button.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		XMLNodePtr backNode = node->FirstNode("BackColor");
		if (backNode)
		{
			Button.BackColor = ReadColor(backNode->AttributeString("color", ""));
		}

		XMLNodePtr foreNode = node->FirstNode("ForeColor");
		if (foreNode)
		{
			Button.ForeColor = ReadColor(foreNode->AttributeString("color", ""));
		}

		Button.StyleTex = mSkinTexAtlas;
	}

	// TextEdit
	if ( node = root->FirstNode("TextEdit") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, TextEdit.StyleStates[uiState]);

			TextEdit.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			TextEdit.Font = mFont;
			TextEdit.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		TextEdit.StyleTex = mSkinTexAtlas;
	}

	// UIWindow
	if ( node = root->FirstNode("UIWindow") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");

			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, WindowBorder.StyleStates[uiState]);

			WindowBorder.StyleStates[uiState].TexColor  = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			WindowBorder.Font = mFont;
			WindowBorder.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		WindowBorder.StyleTex = mSkinTexAtlas;
	}

	if ( node = root->FirstNode("CloseButton") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, WindowCloseBtn.StyleStates[uiState]);

			WindowCloseBtn.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		WindowCloseBtn.StyleTex = mSkinTexAtlas;
	}

	if ( node = root->FirstNode("MaximizeButton") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, WindowMaximizeBtn.StyleStates[uiState]);

			WindowMaximizeBtn.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		WindowMaximizeBtn.StyleTex = mSkinTexAtlas;
	}

	if ( node = root->FirstNode("MinimizeButton") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, WindowMinimizeBtn.StyleStates[uiState]);

			WindowMinimizeBtn.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		WindowMinimizeBtn.StyleTex = mSkinTexAtlas;
	}

	if ( node = root->FirstNode("RestoreButton") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, WindowRestoreBtn.StyleStates[uiState]);

			WindowRestoreBtn.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		WindowRestoreBtn.StyleTex = mSkinTexAtlas;
	}

	if ( node = root->FirstNode("HSlider") )
	{
		XMLNodePtr trackNode = node->FirstNode("Track");
		for (XMLNodePtr stateNode = trackNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, HSliderTrack.StyleStates[uiState]);

			HSliderTrack.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr thumbNode = node->FirstNode("Thumb");
		for (XMLNodePtr stateNode = thumbNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, HSliderThumb.StyleStates[uiState]);

			HSliderThumb.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		HSliderTrack.StyleTex = mSkinTexAtlas;
		HSliderThumb.StyleTex = mSkinTexAtlas;
	}

	// ScrollBar
	if ( node = root->FirstNode("VScrollBar") )
	{
		XMLNodePtr trackNode = node->FirstNode("Track");
		for (XMLNodePtr stateNode = trackNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, VScrollTrack.StyleStates[uiState]);

			VScrollTrack.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr thumbNode = node->FirstNode("Thumb");
		for (XMLNodePtr stateNode = thumbNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, VSrollThumb.StyleStates[uiState]);

			VSrollThumb.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr forwardNode = node->FirstNode("ForwardButton");
		for (XMLNodePtr stateNode = forwardNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, VSrollForward.StyleStates[uiState]);

			VSrollForward.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr backNode = node->FirstNode("BackButton");
		for (XMLNodePtr stateNode = backNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, VSrollBack.StyleStates[uiState]);

			VSrollBack.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		VSrollBack.StyleTex = mSkinTexAtlas;
		VSrollForward.StyleTex = mSkinTexAtlas;
		VScrollTrack.StyleTex = mSkinTexAtlas;
		VSrollThumb.StyleTex = mSkinTexAtlas;
	}

	// ListBox
	if ( node = root->FirstNode("ListBox") )
	{
		for (XMLNodePtr stateNode = node->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{
			String stateName = stateNode->AttributeString("name", "");
			UIElementState uiState = skinDefs.mStateDefs[stateName];
			skinDefs.ReadStateStyles(stateNode, ListBox.StyleStates[uiState]);

			ListBox.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
		}

		XMLNodePtr fontNode = node->FirstNode("Font");
		if (fontNode)
		{
			String fontName = fontNode->AttributeString("name", "");
			ListBox.Font = mFont;
			ListBox.FontSize = fontNode->AttributeFloat("fontSize", 25.0f);
		}

		XMLNodePtr backNode = node->FirstNode("BackColor");
		if (backNode)
			ListBox.BackColor = ReadColor(backNode->AttributeString("color", ""));

		XMLNodePtr foreNode = node->FirstNode("ForeColor");
		if (foreNode)
			ListBox.ForeColor = ReadColor(foreNode->AttributeString("color", ""));

		ListBox.StyleTex = mSkinTexAtlas;
	}

	// ComboBox
	if ( node = root->FirstNode("ComboBox") )
	{
		XMLNodePtr dropdwonNode = node->FirstNode("DropDwonButton");
		if (dropdwonNode)
		{
			for (XMLNodePtr stateNode = dropdwonNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
			{
				String stateName = stateNode->AttributeString("name", "");
				UIElementState uiState = skinDefs.mStateDefs[stateName];
				skinDefs.ReadStateStyles(stateNode, ComboDropButton.StyleStates[uiState]);

				ComboDropButton.StyleStates[uiState].TexColor = ReadColor(stateNode->AttributeString("color", ""));
			}

			ComboDropButton.StyleTex = mSkinTexAtlas;	
		}
	}
}

void GuiSkin::UnloadImpl()
{

}

//shared_ptr<Resource> GuiSkin::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
//{
//	return std::make_shared<GuiSkin>(/*creator, handle, name, group*/);
//}


void GuiSkin::GuiStyle::DrawNinePatch( SpriteBatch& spriteBatch, UIElementState uiState, const Rectanglef& rect, float zOrder )
{
	if (StyleStates[uiState].HasOtherPatch())
	{
		Rectanglef destRect;
		IntRect sourceRectL, sourceRectM, sourceRectR;

		float topHeight = (float)StyleStates[uiState].OtherPatch[NP_Top].Height;
		float bottomHeight = (float)StyleStates[uiState].OtherPatch[NP_Bottom].Height;

		// Draw Top
		sourceRectL = StyleStates[uiState].OtherPatch[NP_Top_Left];
		sourceRectM = StyleStates[uiState].OtherPatch[NP_Top];
		sourceRectR = StyleStates[uiState].OtherPatch[NP_Top_Right];

		destRect.X = rect.Left();
		destRect.Y = rect.Top();
		destRect.Width = (float)sourceRectL.Width;
		destRect.Height = (float)sourceRectL.Height;
		spriteBatch.Draw(StyleTex, destRect, &sourceRectL, StyleStates[uiState].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = rect.Width - (sourceRectL.Width + sourceRectR.Width);
		spriteBatch.Draw(StyleTex, destRect, &sourceRectM, StyleStates[uiState].TexColor, zOrder);

		destRect.X = destRect.Right();
		destRect.Width = (float)sourceRectR.Width;
		spriteBatch.Draw(StyleTex, destRect, &sourceRectR, StyleStates[uiState].TexColor, zOrder);

		if (rect.Height > topHeight + bottomHeight)
		{
			// Draw Middle
			sourceRectL = StyleStates[uiState].OtherPatch[NP_Left];
			sourceRectM = StyleStates[uiState].TexRegion;
			sourceRectR = StyleStates[uiState].OtherPatch[NP_Right];

			destRect.X = rect.Left();
			destRect.Y = destRect.Bottom();
			destRect.Width = (float)sourceRectL.Width;
			destRect.Height = rect.Height - (topHeight + bottomHeight);
			spriteBatch.Draw(StyleTex, destRect, &sourceRectL, StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = rect.Width - (sourceRectL.Width + sourceRectR.Width);
			spriteBatch.Draw(StyleTex, destRect, &sourceRectM, StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = (float)sourceRectR.Width;
			spriteBatch.Draw(StyleTex, destRect, &sourceRectR, StyleStates[uiState].TexColor, zOrder);
		}

		if (rect.Height > sourceRectL.Height)
		{
			// Draw Bottom
			sourceRectL = StyleStates[uiState].OtherPatch[NP_Bottom_Left];
			sourceRectM = StyleStates[uiState].OtherPatch[NP_Bottom];
			sourceRectR = StyleStates[uiState].OtherPatch[NP_Bottom_Right];

			destRect.X = rect.X;
			destRect.Y = destRect.Bottom();
			destRect.Width = (float)sourceRectL.Width;
			if (rect.Height > topHeight + bottomHeight)
				destRect.Height = bottomHeight;
			else 
			{
				destRect.Y = rect.Y + topHeight;
				destRect.Height = rect.Height - topHeight;
			}
			spriteBatch.Draw(StyleTex, destRect, &sourceRectL, StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = (float)rect.Width - (sourceRectL.Width + sourceRectR.Width);
			spriteBatch.Draw(StyleTex, destRect, &sourceRectM, StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = (float)sourceRectR.Width;
			spriteBatch.Draw(StyleTex, destRect, &sourceRectR, StyleStates[uiState].TexColor, zOrder);
		}
	}
	else
	{
		// Direct Fill
		spriteBatch.Draw(StyleTex, rect, &StyleStates[uiState].TexRegion, StyleStates[uiState].TexColor, zOrder);
	}
}

void GuiSkin::GuiStyle::DrawThreePatch( SpriteBatch& spriteBatch, UIElementState uiState, UIOrientation orient, const Rectanglef& rect, float zOrder )
{
	if (StyleStates[uiState].HasOtherPatch())
	{
		Rectanglef destRect;
		IntRect sourceRect[3];

		if (orient == UI_Horizontal)
		{
			sourceRect[0] = StyleStates[uiState].OtherPatch[NP_Left];
			sourceRect[1] = StyleStates[uiState].TexRegion;
			sourceRect[2] = StyleStates[uiState].OtherPatch[NP_Right];

			destRect.X = rect.X;
			destRect.Width = (float)sourceRect[0].Width;
			destRect.Y = rect.Y;
			destRect.Height = rect.Height;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[0], StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = rect.Width - sourceRect[0].Width - sourceRect[2].Width + 1;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[1], StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Right();
			destRect.Width = (float)sourceRect[2].Width;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[2], StyleStates[uiState].TexColor, zOrder);		
		}
		else
		{
			sourceRect[0] = StyleStates[uiState].OtherPatch[NP_Top];
			sourceRect[1] = StyleStates[uiState].TexRegion;
			sourceRect[2] = StyleStates[uiState].OtherPatch[NP_Bottom];

			destRect.X = rect.X;
			destRect.Width = rect.Width;

			destRect.Y = rect.Y;
			destRect.Height = (float)sourceRect[0].Height;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[0], StyleStates[uiState].TexColor, zOrder);

			destRect.Y = destRect.Bottom();
			destRect.Height = rect.Height - sourceRect[0].Height - sourceRect[2].Height;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[1], StyleStates[uiState].TexColor, zOrder);

			destRect.X = destRect.Bottom();
			destRect.Width = (float)sourceRect[2].Height;
			spriteBatch.Draw(StyleTex, destRect, &sourceRect[2], StyleStates[uiState].TexColor, zOrder);
		}
	}
	else
	{
		// Direct Fill
		spriteBatch.Draw(StyleTex, rect, &StyleStates[uiState].TexRegion, StyleStates[uiState].TexColor, zOrder);
	}
}

}