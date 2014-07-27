#pragma once
#include <MainApp/Application.h>
#include <Graphics/Renderer.h>
#include <Graphics/CameraController1.h>
#include <Graphics/RenderPath.h>

using namespace RcEngine;

class ForwardPathApp : public Application
{
public:
	ForwardPathApp(const String& config);
	virtual ~ForwardPathApp(void);

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
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;

	ForwardPath* mForwardPath;
	Light* mDirLight;
};

