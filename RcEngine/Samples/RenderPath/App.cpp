#include "App.h"
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

using namespace RcEngine;

RenderPathApp::RenderPathApp( const String& config ) 
	:Application(config), mCameraControler(0), mFramePerSecond(0)
{

}

RenderPathApp::~RenderPathApp( void )
{

}

void RenderPathApp::Initialize()
{
	mDeferredPath = new DeferredPath();
	mDeferredPath->OnGraphicsInit();

	InitGUI();
}

void RenderPathApp::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mLabel = new Label();
	mLabel->InitGuiStyle(nullptr);
	mLabel->SetName("FPSLabel");
	mLabel->SetPosition(int2(10, 700));
	mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mLabel->SetSize(int2(100, 100));
	//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mLabel );	

	mWindow = new UIWindow;
	mWindow->InitGuiStyle(nullptr);
	mWindow->SetName("Panel");
	mWindow->SetTitle(L"Window Title");
	mWindow->SetPosition(int2(650, 430));
	mWindow->SetSize(int2(350, 300));
	rootElem->AddChild( mWindow );

	int uiY = 50;

	mSunShadowCheckBox = new CheckBox();
	mSunShadowCheckBox->InitGuiStyle(nullptr);
	mSunShadowCheckBox->SetName("Sun Shadow");
	mSunShadowCheckBox->SetPosition(int2(20, uiY));
	mSunShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	mSunShadowCheckBox->SetText(L"Sun Shadow");
	mSunShadowCheckBox->SetChecked(true);
	mSunShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SunCastShadow);
	mWindow->AddChild( mSunShadowCheckBox );	

	mSpotShadowCheckBox = new CheckBox();
	mSpotShadowCheckBox->InitGuiStyle(nullptr);
	mSpotShadowCheckBox->SetName("Spot Shadow");
	mSpotShadowCheckBox->SetPosition(int2(20 + mSunShadowCheckBox->GetSize().X() + 20, uiY));
	mSpotShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	mSpotShadowCheckBox->SetText(L"Spot Shadow");
	mSpotShadowCheckBox->SetChecked(true);
	mSpotShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SpotCastShadow);
	mWindow->AddChild( mSpotShadowCheckBox );	

	
	uiY += mSunShadowCheckBox->GetSize().Y() + 18;

	mVisualizeCascadesCheckBox = new CheckBox();
	mVisualizeCascadesCheckBox->InitGuiStyle(nullptr);
	mVisualizeCascadesCheckBox->SetName("CheckBox");
	mVisualizeCascadesCheckBox->SetPosition(int2(20, uiY));
	mVisualizeCascadesCheckBox->SetSize(int2(150, mVisualizeCascadesCheckBox->GetSize().Y()));
	mVisualizeCascadesCheckBox->SetText(L"Visualize Cascades");
	mVisualizeCascadesCheckBox->SetChecked(false);
	mVisualizeCascadesCheckBox->EventStateChanged.bind(this, &RenderPathApp::VisualizeCascades);
	mWindow->AddChild( mVisualizeCascadesCheckBox );	

	uiY += mVisualizeCascadesCheckBox->GetSize().Y() + 18;

	mBlendAreaLabel = new Label();
	mBlendAreaLabel->InitGuiStyle(nullptr);
	mBlendAreaLabel->SetName("BlendArea");
	mBlendAreaLabel->SetPosition(int2(20, uiY));
	mBlendAreaLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mBlendAreaLabel->SetTextAlignment(AlignLeft);
	mBlendAreaLabel->SetSize(int2(150, 30));
	mBlendAreaLabel->SetText(L"BlendArea: 0.01");
	mBlendAreaLabel->SetFontSize(20.0f);
	//mBlendAreaLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	mWindow->AddChild( mBlendAreaLabel );	

	mBlendAreaSlider = new Slider(UI_Horizontal);
	mBlendAreaSlider->InitGuiStyle(nullptr);
	mBlendAreaSlider->SetName("Slider");	
	mBlendAreaSlider->SetPosition(int2(20 + mBlendAreaLabel->GetSize().X(), uiY + 5));
	mBlendAreaSlider->SetTrackLength(120);
	mBlendAreaSlider->SetValue(50);
	mBlendAreaSlider->EventValueChanged.bind(this, &RenderPathApp::BlendAreaSliderValueChange);
	mWindow->AddChild( mBlendAreaSlider );	

	uiY += mBlendAreaLabel->GetSize().Y();

	mSaveCameraBtn = new Button();
	mSaveCameraBtn->InitGuiStyle(nullptr);
	mSaveCameraBtn->SetName("Button");	
	mSaveCameraBtn->SetPosition(int2(20, uiY));
	mSaveCameraBtn->SetSize(int2(120, 30));
	mSaveCameraBtn->SetText(L"Dump Camera");
	mSaveCameraBtn->EventButtonClicked.bind(this, &RenderPathApp::DumpCameraBtnClicked);
	mWindow->AddChild( mSaveCameraBtn );	
}

void RenderPathApp::SetupScene()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	shared_ptr<TextureResource> tex = resMan.GetResourceByName<TextureResource>(RT_Texture, "seafloor.dds", "General");
	auto fmt = tex->GetTexture()->GetTextureFormat();

	// Ground
	Entity* entity = sceneMan.CreateEntity("Ground", "./Geo/Ground.mesh",  "Custom");
	SceneNode* sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Ground");
	sceneNode->SetScale(float3(2.5,2.5,2.5));
	sceneNode->SetPosition(float3(0, 0, 0));
	sceneNode->AttachObject(entity);

	// Teapot 
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
	auto b = entity->GetWorldBoundingBox();

	entity = sceneMan.CreateEntity("Tank", "./Tank/tank.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Tank");
	sceneNode->SetScale(float3(10, 10, 10));
	sceneNode->SetPosition(float3(0,0,0));
	sceneNode->AttachObject(entity);

	// Load Skybox
	shared_ptr<TextureResource> skyBox = resMan.GetResourceByName<TextureResource>(RT_Texture, "MeadowTrail.dds", "General");
	sceneMan.CreateSkyBox(skyBox->GetTexture());
}

void RenderPathApp::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	//mDirLight = sceneMan.CreateLight("Sun", LT_DirectionalLight);
	//mDirLight->SetDirection(float3(0, -0.5, -1));
	//mDirLight->SetLightColor(float3(1, 1, 1));
	//mDirLight->SetLightIntensity(1.0);
	//mDirLight->SetCastShadow(true);
	//mDirLight->SetShadowCascades(4);
	//sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

	mSpotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
	mSpotLight->SetDirection(float3(0, -1.5, -1));
	mSpotLight->SetLightColor(float3(1, 1, 1));
	mSpotLight->SetRange(300.0);
	mSpotLight->SetPosition(float3(0.0f, 150.0f, 100.0f));
	mSpotLight->SetAttenuation(1.0f, 0.0f);
	mSpotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(60));
	mSpotLight->SetCastShadow(true);
	mSpotLight->SetSpotlightNearClip(10);
	sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);

}

void RenderPathApp::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	
	mCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);
	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);


	//mMaterial = std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "LightShape.material.xml", "General"));	
	//mMaterial = std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "TangentFrame.material.xml", "General"));
	//mMaterial->Load();

	shared_ptr<Effect> shadowEffect = mDeferredPath->GetDeferredEffect();
	shadowEffect->GetParameterByName("ShadowEnabled")->SetValue(true);
	shadowEffect->GetParameterByName("VisiualizeCascades")->SetValue(false);

	SetupScene();
	SetupLights();
}

void RenderPathApp::Update( float deltaTime )
{
	CalculateFrameRate();

	mCameraControler->Update(deltaTime);

	float3 camera = mCamera->GetPosition();
	float3 target = mCamera->GetLookAt();
	float3 up = mCamera->GetUp();

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"FPS: %d", mFramePerSecond);
	mLabel->SetText(buffer);
}

void RenderPathApp::Render()
{
	mDeferredPath->RenderScene();

	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	//std::vector<float4x4> matWorlds;
	//for (uint32_t i = 0; i < mNanosuit->GetNumSubEntities(); ++i)
	//{
	//	SubEntity* subEnt = mNanosuit->GetSubEntity(i);
	//	auto subEntRop = subEnt->GetRenderOperation();

	//	RenderOperation pointsRop;
	//	pointsRop.PrimitiveType = PT_Point_List;
	//	pointsRop.BindVertexStream(subEntRop->GetStreamUnit(0).Stream, subEntRop->GetStreamUnit(0).VertexDecl);
	//	
	//	uint32_t vertexCount = subEntRop->GetStreamUnit(0).Stream->GetBufferSize() / subEntRop->GetStreamUnit(0).VertexDecl->GetVertexSize();
	//	pointsRop.SetVertexRange(0, vertexCount);

	//	uint32_t matCounts = subEnt->GetWorldTransformsCount();
	//	matWorlds.resize(matCounts);
	//	subEnt->GetWorldTransforms(&matWorlds[0]);

	//	//Last matrix is world transform matrix, previous is skin matrices.
	//	float4x4 world = matWorlds.back();

	//	// Skin matrix
	//	if (matCounts > 1)
	//	{	
	//		EffectParameter* skinMatricesParam = mMaterial->GetCustomParameter("SkinMatrices");
	//		if (skinMatricesParam)
	//		{
	//			// delete last world matrix first
	//			matWorlds.pop_back();
	//			skinMatricesParam->SetValue(matWorlds);
	//		}
	//	}

	//	mMaterial->ApplyMaterial(world);
	//	device.Render(*mMaterial->GetCurrentTechnique(), pointsRop);
	//}

	device.GetScreenFrameBuffer()->SwapBuffers();
}

void RenderPathApp::CalculateFrameRate()
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

void RenderPathApp::VisualizeCascades( bool checked )
{
	EffectParameter* effectParam = mDeferredPath->GetDeferredEffect()->GetParameterByName("VisiualizeCascades");
	bool enable;
	effectParam->GetValue(enable);
	effectParam->SetValue(!enable);
}

void RenderPathApp::SunCastShadow( bool checked )
{
	bool enable = mDirLight->GetCastShadow();
	mDirLight->SetCastShadow(!enable);
}

void RenderPathApp::SpotCastShadow( bool checked )
{
	bool enable = mSpotLight->GetCastShadow();
	mSpotLight->SetCastShadow(!enable);
}

void RenderPathApp::BlendAreaSliderValueChange( int32_t value )
{
	float area = value / float(100.0) * 0.5f;
	mDeferredPath->GetShadowManager()->mCascadeBlendArea = area;
	
	wchar_t text[255];
	std::swprintf(text, 255, L"BlendArea: %.3f", area);
	mBlendAreaLabel->SetText(text);
}

void RenderPathApp::DumpCameraBtnClicked()
{
	float3 camera = mCamera->GetPosition();
	float3 target = mCamera->GetLookAt();
	float3 up = mCamera->GetUp();
	
	FILE * pFile;
	pFile = fopen ("E:/camera.txt","w");
	if (pFile!=NULL)
	{
		fprintf (pFile, "float3(%.1f, %.1f, %.1f), float3(%.1f, %.1f, %.1f), float3(%.1f, %.1f, %.1f)",
			camera.X(), camera.Y(), camera.Z(), target.X(), target.Y(), target.Z(),
			up.X(), up.Y(), up.Z());
		fclose (pFile);
	}
}

void RenderPathApp::WindowResize( uint32_t width, uint32_t height )
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.CreatePerspectiveFov(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

	mDeferredPath->OnWindowResize(width, height);
}


int main(int argc, char* argv[])
{
	//wchar_t text1[255];
	//std::swprintf(text1, 255, L"BlendArea: %f", 1.1111);
	//wsprintf(text1, L"BlendArea: %.3f", 1.1111);
	//std::wcout << L"Test " << text1;
	
	RenderPathApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}