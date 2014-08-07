#ifndef RenderPathApp_h__
#define RenderPathApp_h__

#include <MainApp/Application.h>
#include <Graphics/CameraController1.h>
#include <Graphics/RenderPath.h>
#include "LightAnimation.h"

using namespace RcEngine;

class RenderPathApp : public Application
{
public:
	RenderPathApp(const String& config);
	~RenderPathApp(void);

protected:

	void Initialize();
	void InitGUI();
	void LoadContent();
	void UnloadContent() {}
	void Update(float deltaTime);
	void Render();
	void CalculateFrameRate();
	void WindowResize(uint32_t width, uint32_t height);
	
	void SetupScene();
	void SetupLights();

protected:
	RenderDevice* mDevice;


	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;


	RcEngine::Test::FPSCameraControler* mCameraControler;

	int mFramePerSecond;
	
	LightAnimation mLightAnimation;
	
	shared_ptr<RenderOperation> mSphere;
	shared_ptr<RenderOperation> mCone;
	shared_ptr<Material> mMaterial;
	
	//Label* mLabel;
	//UIWindow* mWindow;
	//Slider* mBlendAreaSlider;
	//Label* mBlendAreaLabel;
	//Button* mSaveCameraBtn;
	//CheckBox* mVisualizeCascadesCheckBox;
	//CheckBox* mSunShadowCheckBox;
	//CheckBox* mSpotShadowCheckBox;
	//Light* mDirLight;
	//Light* mSpotLight;
	//CascadedShadowMap* mCascadedShadowMap;
};

#endif // RenderPathApp_h__