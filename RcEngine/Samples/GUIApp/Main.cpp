#include <MainApp/Application.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/FrameBuffer.h>
#include <Scene/SceneManager.h>
#include <Resource/ResourceManager.h>
#include <Graphics/TextureResource.h>
#include <GUI/UIManager.h>
#include <GUI/Button.h>
#include <GUI/CheckBox.h>
#include <GUI/ComboBox.h>
#include <GUI/Label.h>
#include <GUI/LineEdit.h>
#include <GUI/ListBox.h>
#include <GUI/TextEdit.h>
#include <GUI/UIWindow.h>
#include <GUI/Slider.h>
#include <GUI/RadioButton.h>
#include <Core/Environment.h>
#include <Graphics/SpriteBatch.h>


using namespace RcEngine;

class GUIApp : public Application
{
public:
	GUIApp(const String& config)
		: Application(config) {}

	~GUIApp(void) {}

protected:

	void Initialize()
	{
		InitGUIWindow1();
		InitGUIWindow2();
		InitGUIWindow3();

		//SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
		//mSpriteBatch = sceneMan->CreateSpriteBatch();

		//ResourceManager& resMan =  ResourceManager::GetSingleton();
		//auto textureRes = resMan.GetResourceByName<TextureResource>(RT_Texture, "./Arthas/Sword_2H_Frostmourne_D_01_Glow.dds", "Custom");
		//mTexture = textureRes->GetTexture();
	}

	void InitGUIWindow1()
	{
		UIElement* rootElem = UIManager::GetSingleton().GetRoot();

		mUIWindow = new UIWindow;
		mUIWindow->InitGuiStyle(nullptr);
		mUIWindow->SetName("Panel");
		mUIWindow->SetTitle(L"Panel1");
		mUIWindow->SetPosition(int2(50, 50));
		mUIWindow->SetSize(int2(350, 300));
		rootElem->AddChild( mUIWindow );

		int uiY = 40;

		mCheckBox = new CheckBox();
		mCheckBox->InitGuiStyle(nullptr);
		mCheckBox->SetName("CheckBox");
		mCheckBox->SetPosition(int2(20, uiY));
		mCheckBox->SetSize(int2(150, mCheckBox->GetSize().Y()));
		mCheckBox->SetText(L"CheckBox");
		mCheckBox->SetChecked(true);
		mCheckBox->EventStateChanged.bind(this, &GUIApp::CheckBoxClicked);
		mUIWindow->AddChild( mCheckBox );      

		uiY += mCheckBox->GetSize().Y() + 25;


		RadioButton* radioButton1 = new RadioButton();
		radioButton1->InitGuiStyle(nullptr);
		radioButton1->SetText(L"Radio1");
		radioButton1->SetPosition(int2(20, uiY));
		radioButton1->SetSize(int2(150, radioButton1->GetSize().Y()));
		mUIWindow->AddChild( radioButton1 );     

		RadioButton* radioButton2 = new RadioButton();
		radioButton2->InitGuiStyle(nullptr);
		radioButton2->SetText(L"Radio2");
		radioButton2->SetPosition(int2(20 + 100, uiY));
		radioButton2->SetSize(int2(150, radioButton2->GetSize().Y()));
		mUIWindow->AddChild( radioButton2 );  

		mRadioGroup.AddButton(radioButton1);
		mRadioGroup.AddButton(radioButton2);
		
		uiY += radioButton2->GetSize().Y() + 25;

		mSlider = new Slider(UI_Horizontal);
		mSlider->InitGuiStyle(nullptr);
		mSlider->SetName("Slider");    
		mSlider->SetPosition(int2(20 , uiY));
		mSlider->SetTrackLength(200);
		mSlider->SetValue(50);
		mSlider->EventValueChanged.bind(this, &GUIApp::SliderDraged);
		mUIWindow->AddChild( mSlider );  

		uiY += mSlider->GetSize().Y() + 25;

		mButton = new Button();
		mButton->InitGuiStyle(nullptr);
		mButton->SetName("Button");      
		mButton->SetPosition(int2(20, uiY));
		mButton->SetSize(int2(120, 30));
		mButton->SetText(L"Button");
		mButton->EventButtonClicked.bind(this, &GUIApp::ButtonClicked);
		mUIWindow->AddChild( mButton );

		uiY += mButton->GetSize().Y() + 25;

		mComboBox = new ComboBox();
		mComboBox->InitGuiStyle(nullptr);
		mComboBox->SetName("ComboBox");      
		mComboBox->SetPosition(int2(20, uiY));
		mComboBox->SetSize(int2(120, 30));
		mComboBox->InsertItem(0, L"Item0");
		mComboBox->InsertItem(1, L"Item1");
		mComboBox->InsertItem(2, L"Item2");
		mComboBox->InsertItem(3, L"Item3");
		mComboBox->EventSelectionChanged.bind(this, &GUIApp::ComboBoxItemSelected);
		mUIWindow->AddChild( mComboBox );    

	}

	void InitGUIWindow2()
	{
		UIElement* rootElem = UIManager::GetSingleton().GetRoot();

		mUIWindow2 = new UIWindow;
		mUIWindow2->InitGuiStyle(nullptr);
		mUIWindow2->SetName("Panel2");
		mUIWindow2->SetTitle(L"Panel2");
		mUIWindow2->SetPosition(int2(450, 50));
		mUIWindow2->SetSize(int2(350, 500));
		rootElem->AddChild( mUIWindow2 );

		int uiY = 50;

		mLineEdit = new LineEdit;
		mLineEdit->InitGuiStyle(nullptr);
		mLineEdit->SetPosition(int2(20, uiY));
		mLineEdit->SetSize(int2(300, 30));
		mLineEdit->SetText(L"Line Edit");
		mUIWindow2->AddChild(mLineEdit);

		uiY += mLineEdit->GetSize().Y() + 20;

		mTextEdit = new TextEdit;
		mTextEdit->InitGuiStyle(nullptr);
		mTextEdit->SetPosition(int2(20, uiY));
		mTextEdit->SetSize(int2(300, 350));
		mTextEdit->SetText(L"Text Edit\nRcEngine is a graphics engine implemented in C++, inspired by OGRE and KlayGE.");

		mUIWindow2->AddChild(mTextEdit);
	}

	void InitGUIWindow3()
	{
		UIElement* rootElem = UIManager::GetSingleton().GetRoot();

		mUIWindow3 = new UIWindow();
		mUIWindow3->InitGuiStyle(nullptr);
		mUIWindow3->SetName("Panel2");
		mUIWindow3->SetTitle(L"Panel2");
		mUIWindow3->SetPosition(int2(50, 350));
		mUIWindow3->SetSize(int2(350, 300));
		rootElem->AddChild( mUIWindow3 );
	}

	void CheckBoxClicked(bool bChecked)
	{
		printf("CheckBox %s\n", bChecked ? "true" : "false");
	}

	void ButtonClicked()
	{
		printf("Button Clicked\n");
	}

	void SliderDraged(int32_t value)
	{
		printf("Slider Position: %d\n", value);
	}

	void ComboBoxItemSelected(int32_t index)
	{
		wprintf(L"ComboBox Select: %s\n", mComboBox->GetSelectedItem().c_str());
	}

	void LoadContent()
	{

	}

	void UnloadContent() 
	{

	}

	void Update(float deltaTime)
	{

	}
	
	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		device->GetScreenFrameBuffer()->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0f, 0);
		
		// Update overlays
		UIManager& uiMan = UIManager::GetSingleton();
		uiMan.Render();

		sceneMan->UpdateOverlayQueue();

		RenderBucket& guiBucket =sceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay);       
		for (const RenderQueueItem& renderItem : guiBucket) 
			renderItem.Renderable->Render();

		device->GetScreenFrameBuffer()->SwapBuffers();
	}


	UIWindow* mUIWindow;
	Button* mButton;
	CheckBox* mCheckBox;
	Slider* mSlider;
	ComboBox* mComboBox;
	Label* mLabel;

	RadioButtonGroup mRadioGroup;

	UIWindow* mUIWindow2;
	LineEdit* mLineEdit;
	TextEdit* mTextEdit;

	UIWindow* mUIWindow3;
	

	SpriteBatch* mSpriteBatch;
	shared_ptr<Texture> mTexture;
};

int main()
{
	GUIApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}
