#include "ForwardPathApp.h"
#include <MainApp/Application.h>
#include <Graphics/CameraController1.h>
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
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
#include <Graphics/SamplerState.h>
#include <Graphics/Pipeline.h>
#include <Graphics/Renderer.h>
#include <Graphics/FrameBuffer.h>
#include <GUI/Label.h>
#include <GUI/Slider.h>
#include <GUI/UIWindow.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <GUI/CheckBox.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <Graphics/CascadedShadowMap.h>
#include <Scene/Light.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Texture.h>
#include <Scene/SubEntity.h>

ForwardPathApp::ForwardPathApp(const String& config)
	:Application(config), mCameraControler(0), mFramePerSecond(0)
{

}

ForwardPathApp::~ForwardPathApp(void)
{
}

void ForwardPathApp::Initialize()
{
	mForwardPath = new ForwardPath();
	mForwardPath->OnGraphicsInit();

	InitGUI();
}

void ForwardPathApp::InitGUI()
{

}

void ForwardPathApp::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	mCamera->CreateLookAt(float3(-67.8, 57.9, 62.9), float3(-67.3, 57.4, 62.3), float3(0.3, 0.9, -0.4));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);
	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);

	SetupScene();
	SetupLights();
}

void ForwardPathApp::CalculateFrameRate()
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

void ForwardPathApp::WindowResize( uint32_t width, uint32_t height )
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.CreatePerspectiveFov(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

	mForwardPath->OnWindowResize(width, height);
}

void ForwardPathApp::SetupScene()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	// Ground
	Entity* entity = sceneMan.CreateEntity("Ground", "./Geo/Ground.mesh",  "Custom");
	SceneNode* sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Ground");
	sceneNode->SetPosition(float3(0, 0, 0));
	sceneNode->AttachObject(entity);

	//// Teapot 
	entity = sceneMan.CreateEntity("Teapot", "./Geo/Teapot.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Teapot");
	sceneNode->Rotate(QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::ToRadian(-90.0)));
	sceneNode->SetPosition(float3(50, 0, 0));
	sceneNode->AttachObject(entity);

	entity = sceneMan.CreateEntity("Nanosuit", "./Nanosuit/Nanosuit.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Nanosuit");
	sceneNode->SetScale(float3(2,2,2));
	sceneNode->SetPosition(float3(-50,0,0));
	sceneNode->AttachObject(entity);

	entity = sceneMan.CreateEntity("Tank", "./Tank/tank.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Tank");
	sceneNode->SetScale(float3(10, 10, 10));
	sceneNode->SetPosition(float3(0,0,0));
	sceneNode->AttachObject(entity);

	// Load Skybox
	shared_ptr<TextureResource> skyBox = resMan.GetResourceByName<TextureResource>(RT_Texture, "MeadowTrail.dds", "General");
	sceneMan.CreateSkyBox(skyBox->GetTexture());
}

void ForwardPathApp::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	mDirLight = sceneMan.CreateLight("Sun", LT_DirectionalLight);
	mDirLight->SetDirection(float3(0, -0.5, -1));
	mDirLight->SetLightColor(float3(1, 1, 1));
	mDirLight->SetLightIntensity(1.0);
	mDirLight->SetCastShadow(true);
	mDirLight->SetShadowCascades(4);
	sceneMan.GetRootSceneNode()->AttachObject(mDirLight);
}

void ForwardPathApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mCameraControler->Update(deltaTime);
	mMainWindow->SetTitle(std::to_string(mFramePerSecond));
}

void ForwardPathApp::Render()
{
	mForwardPath->RenderScene();
}


int main(int argc, char* argv[])
{
	//wchar_t text1[255];
	//std::swprintf(text1, 255, L"BlendArea: %f", 1.1111);
	//wsprintf(text1, L"BlendArea: %.3f", 1.1111);
	//std::wcout << L"Test " << text1;

	ForwardPathApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}