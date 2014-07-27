#include "SponzaApp.h"
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
#include <GUI/Label.h>
#include <GUI/UIManager.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>

SponzaApp::SponzaApp( const String& config )
	:Application(config),
	 mCameraControler(0),
	 mFramePerSecond(0)
{

}


SponzaApp::~SponzaApp(void)
{
	SAFE_DELETE(mCameraControler);
}

void SponzaApp::Initialize()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory =  Context::GetSingleton().GetRenderFactory();
}

void SponzaApp::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	Camera& camera = *device.GetCurrentFrameBuffer()->GetCamera();
	//camera->SetViewParams(float3(0, 0, -20), float3(0, 0, 0));
	camera.CreateLookAt(float3(-296, 147, 11), float3(0, 50, 0));
	//camera->SetViewParams(float3(0, 5, -40), float3(0, 5, 0));
	camera.CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	//mCameraControler = new ModelViewerCameraControler();
	//mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);

	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->SetScale(0.45f);
	sponzaNode->AttachObject(sponzaEntity);

	mCameraControler->SetMoveSpeed(50.0f);
	mCameraControler->SetMoveInertia(true);

	InitGUI();
}

void SponzaApp::UnloadContent()
{
	SAFE_DELETE(mPlayer);
}

void SponzaApp::CalculateFrameRate()
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

void SponzaApp::Render()
{
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();
	SceneManager& scenenMan = Context::GetSingleton().GetSceneManager();

	shared_ptr<FrameBuffer> defaultFrameBuffer = renderDevice.GetScreenFrameBuffer();

	float clr = (float)169/255;
	renderDevice.BindFrameBuffer(defaultFrameBuffer);	
	defaultFrameBuffer->Clear(CF_Color | CF_Depth | CF_Stencil, ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);

	//DrawUI();

	// Move to engine level
	/*scenenMan.UpdateRenderQueue(currentFrameBuffer->GetCamera(), RO_StateChange);
	scenenMan.RenderScene();*/

	scenenMan.UpdateRenderQueue(*defaultFrameBuffer->GetCamera(), RO_StateChange);

	RenderQueue* renderQueue = scenenMan.GetRenderQueue();	
	std::vector<RenderQueueItem>&  renderBucket = renderQueue->GetRenderBucket(RenderQueue::BucketOpaque);

	if (renderBucket.size())
	{
		std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
			return lhs.SortKey < rhs.SortKey; });

			for (const RenderQueueItem& renderItem : renderBucket)
			{
				renderItem.Renderable->GetMaterial()->SetCurrentTechnique("PointLighting");
				renderItem.Renderable->Render();
			}
	}

	// Swap Buffer
	defaultFrameBuffer->SwapBuffers();	
}

void SponzaApp::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);

	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));
}

void SponzaApp::InitGUI()
{
	mFPSLabel = new Label();
	mFPSLabel->InitGuiStyle(nullptr);
	mFPSLabel->SetName("FPSLabel");
	mFPSLabel->SetPosition(int2(400, 500));
	mFPSLabel->SetSize(int2(400, 100));
	UIManager::GetSingleton().GetRoot()->AddChild( mFPSLabel );	
}

void SponzaApp::DrawUI()
{
	wchar_t buffer[100];
	int cx = swprintf ( buffer, 100, L"Graphics Demo FPS: %d", mFramePerSecond );
	mFPSLabel->SetText(buffer);

	// Render UI
	UIManager::GetSingleton().Render();
}

int32_t main()
{
	SponzaApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}
