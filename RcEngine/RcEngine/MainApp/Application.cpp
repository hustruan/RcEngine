#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/Effect.h>
#include <Graphics/Material.h>
#include <Graphics/TextureResource.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Mesh.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Font.h>
#include <Resource/ResourceManager.h>
#include <Core/Environment.h>
#include <Core/ModuleManager.h>
#include <Core/Exception.h>
#include <Core/Profiler.h>
#include <Core/XMLDom.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Input/InputSystem.h>
#include <Scene/SceneManager.h>
#include <GUI/UIManager.h>

// C++ 11 thread
#include <thread>

namespace RcEngine {

Application* Application::msApp = nullptr;

Application::Application( const String& config )
	: mEndGame(false),
	  mAppPaused(false),
	  mConfigFile(config)
{
	msApp = this;

	Environment::Initialize();
	InputSystem::Initialize();
	ModuleManager::Initialize();
	FileSystem::Initialize();
	ResourceManager::Initialize();
	ProfilerManager::Initialize();
	UIManager::Initialize();

	// Init System Clock
	SystemClock::InitClock();
	
	// todo add sub scene manager
	new SceneManager;

	ResourceManager& resMan = ResourceManager::GetSingleton();
	resMan.RegisterType(RT_Mesh, "Mesh", Mesh::FactoryFunc);
	resMan.RegisterType(RT_Material, "Material", Material::FactoryFunc);
	resMan.RegisterType(RT_Effect, "Effect", Effect::FactoryFunc);
	resMan.RegisterType(RT_Animation, "Animation",AnimationClip::FactoryFunc);
	resMan.RegisterType(RT_Texture, "Texture", TextureResource::FactoryFunc);
	//resMan.RegisterType(RT_Pipeline, "Pipeline", Pipeline::FactoryFunc);
	resMan.RegisterType(RT_Font, "Font", Font::FactoryFunc);

	LoadConfiguration();

	Environment::GetSingleton().mApplication = this;
}

Application::~Application( void )
{
	
}

void Application::RunGame()
{
	Initialize();

	LoadContent();

	mTimer.Reset();

	do 
	{
		Tick();
	} while ( !mEndGame );

	UnloadContent();
}

void Application::Tick()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
	InputSystem& inputSystem = InputSystem::GetSingleton();

	if (!mActice)
	{
		// Inative, Sleep for a while
		static const std::chrono::milliseconds inactiveSleepTime( 20 );
		std::this_thread::sleep_for( inactiveSleepTime );
	}

	// Andvance Game Time
	mTimer.Tick();

	// Read Input
	inputSystem.BeginEvents();
		mMainWindow->CollectOSEvents();
	inputSystem.EndEvents();

	float deltaTime = mTimer.GetDeltaTime();

	// Process input events
	ProcessEventQueue();

	inputSystem.Dispatch(deltaTime);

	// update
	Update(deltaTime);
	
	// update scene graph
	sceneMan->UpdateSceneGraph(deltaTime);

	// Update UI
	UIManager::GetSingleton().Update(deltaTime);

	// render
	Render();
}

void Application::ProcessEventQueue()
{
	InputSystem& inputSystem = InputSystem::GetSingleton();

	InputEvent event;
	while(inputSystem.PollEvent(&event))
	{
		bool eventConsumed = false;

		eventConsumed = UIManager::GetSingleton().OnEvent(event);
		
		if (!eventConsumed)
		{
			/*if (!mAppPaused)
			eventConsumed = mAppPaused*/

			if (!eventConsumed)
			{
				
				if (event.EventType == InputEventType::KeyDown && event.Key.key == KC_Escape)
				{
					eventConsumed = true;
					mEndGame = true;
					return;
				}
			}
		}
	}

}

void Application::LoadAllModules()
{
	ModuleType deviceType = MT_Render_OpengGL;
	if (mAppSettings.RHDeviceType == RD_Direct3D11)
		deviceType = MT_Render_D3D11;

	ModuleManager::GetSingleton().Load(deviceType);
}

void Application::UnloadAllModules()
{
	ModuleManager::GetSingleton().UnloadAll();
}

void Application::Window_ApplicationActivated()
{
	mActice = true;
}

void Application::Window_ApplicationDeactivated()
{
	mActice = false;
}

void Application::Window_Suspend()
{
	mAppPaused = true;
	mTimer.Stop();
}

void Application::Window_Resume()
{
	mAppPaused = true;
	mTimer.Start();
}

void Application::Window_Paint()
{

}

void Application::Window_UserResized()
{
	uint32_t width = mMainWindow->GetWidth();
	uint32_t height = mMainWindow->GetHeight();

	if (mAppSettings.Width != width || mAppSettings.Height != height)
	{
		mAppSettings.Width = width;
		mAppSettings.Height = height;

		Environment::GetSingleton().GetRenderDevice()->OnWindowResize(width, height);
		UIManager::GetSingleton().OnWindowResize(width, height);

		WindowResize(width, height);
	}	
}

void Application::Window_Close()
{
	mEndGame = true;
}

void Application::Create()
{
	// Create main window
	mMainWindow = new Window(mAppSettings);
	mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
	mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
	mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
	mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
	mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);
	mMainWindow->WindowClose.bind(this, &Application::Window_Close);
	mMainWindow->UserResizedEvent.bind(this, &Application::Window_UserResized);

	// load all modules
	LoadAllModules();

	// UI Graphics initailize
	UIManager::GetSingleton().OnGraphicsInitialize();

	// Show main window
	mMainWindow->ShowWindow();
}

void Application::Release()
{
	// Delete Scene Manager
	//SceneManager* pSceneMan = Environment::GetSingleton().GetSceneManagerPtr();
	//delete pSceneMan;

	/*DebugDrawManager::Finalize();
	ResourceManager::Finalize();
	ModuleManager::Finalize();
	FileSystem::Finalize();
	InputSystem::Finalize();
	Environment::Finalize();*/

	//UIManager::Finalize();
}

void Application::LoadConfiguration()
{
	FileStream config;
	if ( !config.Open(mConfigFile) )
	{
		std::cout << "App config file " + mConfigFile + " doesn't found!" << std::endl;
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "App config file " + mConfigFile + " doesn't found!", "Application::ReadConfiguration");
	}


	XMLDoc xmlConfig;
	XMLNodePtr appNode = xmlConfig.Parse(config);

	mAppSettings.AppTitle = appNode->AttributeString("Title", "RcEngine");

	XMLNodePtr graphicNode = appNode->FirstNode("Graphics");

	mAppSettings.Left = graphicNode->Attribute("Left")->ValueInt();
	mAppSettings.Top = graphicNode->Attribute("Top")->ValueInt();
	mAppSettings.Width = graphicNode->Attribute("Width")->ValueUInt();
	mAppSettings.Height = graphicNode->Attribute("Height")->ValueUInt();
	mAppSettings.Fullscreen = graphicNode->Attribute("FullScreen")->ValueInt() != 0;
	mAppSettings.ColorFormat = PF_RGBA8_UNORM;
	mAppSettings.DepthStencilFormat = PF_D24S8;

	XMLNodePtr node;
	
	node = graphicNode->FirstNode("Sample");
	if (node)
	{
		mAppSettings.SampleCount = node->AttributeUInt("Count", 1);
		mAppSettings.SampleQuality = node->AttributeUInt("Quality", 0); 
	}

	node = graphicNode->FirstNode("SyncInterval");
	if (node)
	{
		mAppSettings.SyncInterval = node->AttributeUInt("Interval", 0);  
	}

	node = appNode->FirstNode("RenderSystem");
	if (node)
	{
		String name = node->AttributeString("System", "OpenGL");
		if (name == "OpenGL")
			mAppSettings.RHDeviceType = RD_OpenGL;
		else if (name == "Direct3D11")
			mAppSettings.RHDeviceType = RD_Direct3D11;
		else
		{
			assert(false);
		}
	}
	else
		mAppSettings.RHDeviceType = RD_OpenGL;

	XMLNodePtr resNode = appNode->FirstNode("Resource");
	for (XMLNodePtr groupNode = resNode->FirstNode("Group"); groupNode; groupNode = groupNode->NextSibling("Group"))
	{
		String groupName = groupNode->Attribute("Name")->ValueString();
		for (XMLNodePtr pathNode = groupNode->FirstNode("Path"); pathNode; pathNode = pathNode->NextSibling("Path"))
		{
			String pathName = pathNode->Attribute("Name")->ValueString();
			ResourceManager::GetSingleton().AddResourceGroup(groupName);
			FileSystem::GetSingleton().RegisterPath(pathName, groupName);
		}
	}
}


} // Namespace RcEngine