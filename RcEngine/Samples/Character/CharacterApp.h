#pragma once
#include <MainApp/Application.h>
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include "SinbadCharacterController.h"
#include "ThirdPersonCamera.h"

using namespace RcEngine;

class CharacterApp : public Application
{
public:
	CharacterApp(const String& config);
	~CharacterApp(void);

	void Initialize();
	void LoadContent();
	void UnloadContent()
	{

	}
	void CalculateFrameRate();
	void Update(float deltaTime);
	void Render();
	void WindowResize(uint32_t width, uint32_t height);

protected:

	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Camera> mCamera;
	Test::FPSCameraControler* mCameraControler;


	SinbadCharacterController* mSinbadController;
	ThirdPersonCamera* mThirdPersonCamera;

	int mFramePerSecond;
};