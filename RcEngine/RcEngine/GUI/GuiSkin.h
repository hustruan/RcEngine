#ifndef GuiSkin_h__
#define GuiSkin_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

enum UIElementState
{
	UI_State_Normal = 0,
	UI_State_Disable,
	UI_State_Hidden,
	UI_State_Focus,
	UI_State_Hover,
	UI_State_Pressed,
	UI_State_Count
};

enum UIOrientation
{
	UI_Horizontal,
	UI_Vertical
};

enum NinePatch
{
	NP_Top_Left = 0,
	NP_Top,
	NP_Top_Right,
	NP_Right,
	NP_Bottom_Right,
	NP_Bottom,
	NP_Bottom_Left,
	NP_Left,
	NP_Fill,
	NP_Count
};

enum TextEditLevel
{
	TEL_Label,
	TEL_NoEdit,
	TEL_Edit
};

// Border
enum Border
{
	Border_Left = 0,
	Border_Top,
	Border_Right,
	Border_Bottom,
	Border_Count
};

/**
 * GuiSkin is a collection of GUIStyles that can be applied to your GUI Controls.
 */
class _ApiExport GuiSkin /*: public Resource*/
{
public:

	struct SytleImage
	{
		/**
		 * Only fill patch or all 9 patch, 3 patch.
		 */
		IntRect TexRegion;

		IntRect* OtherPatch;

		ColorRGBA  TexColor;

		SytleImage() : TexColor(ColorRGBA::White), OtherPatch(nullptr) {}
		~SytleImage() { if (OtherPatch) delete[] OtherPatch; }

		inline bool HasOtherPatch() const { return OtherPatch != nullptr; }
	};

	struct GuiStyle
	{
		SytleImage StyleStates[UI_State_Count];
		shared_ptr<Texture> StyleTex;

		shared_ptr<Font> Font;
		float FontSize;

		ColorRGBA BackColor;
		ColorRGBA ForeColor; 

		void DrawThreePatch(SpriteBatch& spriteBatch, UIElementState uiState, UIOrientation orient, const Rectanglef& rect, float zOrder);
		void DrawNinePatch(SpriteBatch& spriteBatch, UIElementState uiState, const Rectanglef& rect, float zOrder);
	};

	typedef std::map<String, GuiStyle*> StyleMap;

public:
	//GuiSkin(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	GuiSkin();
	~GuiSkin();

	void TestLoad() { LoadImpl(); }

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	//static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);


public:

	String mName;
	
	shared_ptr<Font> mFont;
	float mFontSize;

	shared_ptr<Texture> mSkinTexAtlas; 

	GuiStyle Label;

	// Button
	GuiStyle Button;

	// Check box
	GuiStyle CheckBox;

	// RadioButton
	GuiStyle RadioButton;

	GuiStyle ListBox;

	GuiStyle TextEdit;

	// Slider
	GuiStyle HSliderTrack, HSliderThumb;  // Normal or hover
	GuiStyle VSliderTrack, VSliderThumb;  // Normal or hover

	// ScrollBar
	GuiStyle HScrollTrack, HSrollThumb, HSrollForward, HSrollBack;
	GuiStyle VScrollTrack, VSrollThumb, VSrollForward, VSrollBack;

	// ComboBox
	GuiStyle ComboDropButton;

	// UIWindow
	GuiStyle WindowBorder;
	GuiStyle WindowCloseBtn, WindowMinimizeBtn, WindowRestoreBtn, WindowMaximizeBtn;
};

}


#endif // GuiSkin_h__
