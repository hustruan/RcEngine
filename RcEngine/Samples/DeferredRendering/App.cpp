#include "App.h"
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Math/MathUtil.h>
#include <GUI/UIManager.h>
#include <GUI/Label.h>
#include <GUI/CheckBox.h>
#include <GUI/Button.h>
#include <GUI/UIWindow.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/CascadedShadowMap.h>

App::App( const String& config )
	: Application(config),
	  mFramePerSecond(0)
{

}

App::~App(void)
{
	delete mDeferredPath;
}

void App::Initialize()
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	mDeferredPath = new DeferredPath;
	mDeferredPath->OnGraphicsInit();

	mDeferredPath->mVisualLights = true;
	mDeferredPath->mVisualLightsWireframe = false;

	InitGUI();
}

void App::LoadContent()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	FileSystem::GetSingleton().RegisterPath("../Media/Mesh/Sphere", "Custom");

	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->SetScale(0.45f);
	sponzaNode->AttachObject(sponzaEntity);

	Entity* sphere = sceneMan.CreateEntity("Sphere", "Sphere.mesh",  "Custom");
	mCameraTargetNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sphere");
	mCameraTargetNode->AttachObject(sphere);

	// Set up lights
	SetupLights();
	
	// Set up camera
	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	mCamera->CreateLookAt(float3(213.4, 405.5, -30.5),  float3(214.3, 405.1, -30.6), float3(0.4, 0.9, -0.0));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);

	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);
}

void App::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	/*Light* mDirLight = sceneMan.CreateLight("Sun", LT_DirectionalLight);
	mDirLight->SetDirection(float3(0, -8, -1));
	mDirLight->SetLightColor(float3(1, 1, 1));
	mDirLight->SetCastShadow(false);
	sceneMan.GetRootSceneNode()->AttachObject(mDirLight);*/

	//mPointLight = sceneMan.CreateLight("Point", LT_PointLight);
	//mPointLight->SetLightColor(float3(1, 1, 0));
	//mPointLight->SetRange(80.0f);
	//mPointLight->SetAttenuation(1.0f, 0.0f);
	//mPointLight->SetPosition(float3(550, 81, -18));
	//sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

	//mSpotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
	//mSpotLight->SetLightColor(float3(0, 1, 0));
	//mSpotLight->SetRange(250.0f);
	//mSpotLight->SetPosition(float3(-442, 80, -16));
	//mSpotLight->SetDirection(float3(-1, 0, 0));
	//mSpotLight->SetAttenuation(1.0f, 0.0f);
	//mSpotLight->SetSpotAngle(Mathf::ToRadian(30), Mathf::ToRadian(40));
	//sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);


	{
		float3 direction = Normalize(float3(-111.5f, 398.1f, 3.6f) - float3(-111.1, 380.1, 73.4));
		for (int i = 0; i < 4; ++i)
		{
			Light* spotLight = sceneMan.CreateLight("Spot" + std::to_string(i), LT_SpotLight);
			spotLight->SetLightColor(float3(1, 1, 0));
			spotLight->SetRange(150);
			spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, 3.6f));
			spotLight->SetDirection(direction);
			spotLight->SetAttenuation(1.0f, 0.0f);
			spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
			sceneMan.GetRootSceneNode()->AttachObject(spotLight);
			mSpotLights.push_back(spotLight);
		}

		direction = Normalize(float3(-111.5f, 398.1f, 35.7f) - float3(-111.1, 380.1, -111.3));
		for (int i = 0; i < 4; ++i)
		{
			Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
			spotLight->SetLightColor(float3(0, 1, 1));
			spotLight->SetRange(150);
			spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, -35.7f));
			spotLight->SetDirection(direction);
			spotLight->SetAttenuation(1.0f, 0.0f);
			spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
			sceneMan.GetRootSceneNode()->AttachObject(spotLight);
			mSpotLights.push_back(spotLight);
		}
	}

	mLightAnimation.LoadLights("E:/lights.txt");
}

void App::UnloadContent()
{
}

void App::Render()
{
	mDeferredPath->RenderScene();

	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	device.GetScreenFrameBuffer()->SwapBuffers();
}

void App::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);
	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));

	const float3& cameraPos = mCamera->GetPosition();

	float3 cameraTarget = cameraPos + mCamera->GetView() * 50.0f;
	mCameraTargetNode->SetPosition(cameraTarget);

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"FPS: %d, Camera (%.1f, %.1f, %.1f)", mFramePerSecond, cameraPos.X(), cameraPos.Y(), cameraPos.Z());
	mLabel->SetText(buffer);
}

void App::CalculateFrameRate()
{
	static int frameCount = 0;
	static float baseTime = 0;

	frameCount++;

	if (mTimer.GetGameTime()-baseTime >= 1.0f)
	{
		mFramePerSecond = frameCount;
		frameCount = 0;
		baseTime += 1.0f;
	}
}

void App::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mLabel = new Label();
	mLabel->InitGuiStyle(nullptr);
	mLabel->SetName("FPSLabel");
	mLabel->SetPosition(int2(10, 700));
	mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mLabel->SetSize(int2(400, 100));
	//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mLabel );	
	
	mControlPanel = new UIWindow;
	mControlPanel->InitGuiStyle(nullptr);
	mControlPanel->SetName("Panel");
	mControlPanel->SetTitle(L"Control Panel");
	mControlPanel->SetPosition(int2(660, 460));
	mControlPanel->SetSize(int2(350, 300));
	mControlPanel->AddChild( mControlPanel );
	rootElem->AddChild( mControlPanel );

	int uiY = 50;
	
	CheckBox* checkBox;
	checkBox = new CheckBox();
	checkBox->InitGuiStyle(nullptr);
	checkBox->SetName("Visual Lights Shape");
	checkBox->SetPosition(int2(20, uiY));
	checkBox->SetSize(int2(200, checkBox->GetSize().Y()));
	checkBox->SetText(L"Visual Lights Shape");
	checkBox->SetChecked(true);
	checkBox->EventStateChanged.bind(this, &App::VisualLights);
	mControlPanel->AddChild( checkBox );

	uiY += checkBox->GetSize().Y() + 18;

	checkBox = new CheckBox();
	checkBox->InitGuiStyle(nullptr);
	checkBox->SetName("Visual Lights Wireframe");
	checkBox->SetPosition(int2(20, uiY));
	checkBox->SetSize(int2(250, checkBox->GetSize().Y()));
	checkBox->SetText(L"Visual Lights Wireframe");
	checkBox->SetChecked(false);
	checkBox->EventStateChanged.bind(this, &App::VisualLightsWireframe);
	mControlPanel->AddChild( checkBox );

	uiY += checkBox->GetSize().Y() + 18;

	Button* button = new Button();
	button->InitGuiStyle(nullptr);
	button->SetName("Add Point Light");	
	button->SetPosition(int2(20, uiY));
	button->SetSize(int2(200, 30));
	button->SetText(L"Add Point Light");
	button->EventButtonClicked.bind(this, &App::AddPointLight);
	mControlPanel->AddChild( button );	

	uiY += button->GetSize().Y() + 18;

	button = new Button();
	button->InitGuiStyle(nullptr);
	button->SetName("Dump Lights");	
	button->SetPosition(int2(20, uiY));
	button->SetSize(int2(150, 30));
	button->SetText(L"Dump Lights");
	button->EventButtonClicked.bind(this, &App::DumpLights);
	mControlPanel->AddChild( button );	
}

void App::WindowResize(uint32_t width, uint32_t height)
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.CreatePerspectiveFov(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

	mDeferredPath->OnWindowResize(width, height);
}

void App::VisualLights( bool checked )
{
	mDeferredPath->mVisualLights = checked;
}

void App::VisualLightsWireframe( bool checked )
{
	mDeferredPath->mVisualLightsWireframe = checked;
}

void App::AddPointLight()
{
	const float3& cameraPos = mCamera->GetPosition();
	float3 cameraTarget = cameraPos + mCamera->GetView() * 50.0f;
	mLightAnimation.AddPointLight(cameraTarget);
}

void App::DumpLights()
{
	mLightAnimation.SaveLights("E:/lights.txt");
}


int main(int argc, char* argv[])
{
	App app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();


	return 0;
}