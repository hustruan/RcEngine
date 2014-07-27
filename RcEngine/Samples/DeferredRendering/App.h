#pragma once
#include <MainApp/Application.h>
#include <Graphics/Renderer.h>
#include <Graphics/CameraController1.h>
#include <Graphics/RenderPath.h>
#include "LightAnimation.h"

using namespace RcEngine;

class App : public Application
{
public:
	App(const String& config);
	~App(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void SetupLights();
	void InitGUI();
	void CalculateFrameRate();

	void WindowResize(uint32_t width, uint32_t height);

	void MouseButtonPress();

	void VisualLights(bool checked);
	void VisualLightsWireframe(bool checked);
	void AddPointLight();
	void DumpLights();

protected:

	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;

	Label* mLabel;
	UIWindow* mControlPanel;

	DeferredPath* mDeferredPath;
	SceneNode* mCameraTargetNode;

	shared_ptr<RenderOperation> mSphere;
	shared_ptr<RenderOperation> mCone;
	shared_ptr<Material> mMaterial;

	Light* mPointLight;
	Light* mSpotLight;

	LightAnimation mLightAnimation;

	std::vector<Light*> mSpotLights;
};

