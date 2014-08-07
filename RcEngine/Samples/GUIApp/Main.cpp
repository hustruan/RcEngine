#include <MainApp/Application.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/FrameBuffer.h>
#include <Scene/SceneManager.h>
#include <GUI/UIManager.h>
#include <GUI/Button.h>
#include <GUI/CheckBox.h>
#include <GUI/Label.h>
#include <GUI/LineEdit.h>
#include <GUI/ListBox.h>
#include <GUI/UIWindow.h>
#include <Core/Environment.h>

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
		InitGUI();
	}

	void InitGUI()
	{
		UIElement* rootElem = UIManager::GetSingleton().GetRoot();

		Label* label = new Label();
		label->InitGuiStyle(nullptr);
		label->SetName("FPSLabel");
		label->SetPosition(int2(10, 700));
		label->SetTextColor(ColorRGBA(1, 0, 0, 1));
		label->SetSize(int2(100, 100));
		//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
		rootElem->AddChild( label );   

		UIWindow* window = new UIWindow;
		window->InitGuiStyle(nullptr);
		window->SetName("Panel");
		window->SetTitle(L"Window Title");
		window->SetPosition(int2(650, 430));
		window->SetSize(int2(350, 300));
		rootElem->AddChild( window );

		int uiY = 50;

		CheckBox* checkBox = new CheckBox();
		checkBox->InitGuiStyle(nullptr);
		checkBox->SetName("CheckBox");
		checkBox->SetPosition(int2(20, uiY));
		checkBox->SetSize(int2(150, checkBox->GetSize().Y()));
		checkBox->SetText(L"CheckBox");
		checkBox->SetChecked(true);
		//checkBox->EventStateChanged.bind(this, &RenderPathApp::SunCastShadow);
		window->AddChild( checkBox );        

		uiY += checkBox->GetSize().Y() + 18;


		//mBlendAreaSlider = new Slider(UI_Horizontal);
		//mBlendAreaSlider->InitGuiStyle(nullptr);
		//mBlendAreaSlider->SetName("Slider");    
		//mBlendAreaSlider->SetPosition(int2(20 + mBlendAreaLabel->GetSize().X(), uiY + 5));
		//mBlendAreaSlider->SetTrackLength(120);
		//mBlendAreaSlider->SetValue(50);
		//mBlendAreaSlider->EventValueChanged.bind(this, &RenderPathApp::BlendAreaSliderValueChange);
		//window->AddChild( mBlendAreaSlider );  

		//uiY += mBlendAreaLabel->GetSize().Y();

		Button* button = new Button();
		button->InitGuiStyle(nullptr);
		button->SetName("Button");      
		button->SetPosition(int2(20, uiY));
		button->SetSize(int2(120, 30));
		button->SetText(L"Button");
		//button->EventButtonClicked.bind(this, &RenderPathApp::DumpCameraBtnClicked);
		window->AddChild( button );    
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

		// Update overlays
		UIManager& uiMan = UIManager::GetSingleton();

		uiMan.Render();
		sceneMan->UpdateOverlayQueue();

		RenderBucket& guiBucket =sceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay);       
		for (const RenderQueueItem& renderItem : guiBucket) 
			renderItem.Renderable->Render();

		device->GetScreenFrameBuffer()->SwapBuffers();
	}
};

int main()
{
	GUIApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}
