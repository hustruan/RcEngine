#pragma once
#ifndef SponzaApp_h__
#define SponzaApp_h__

#include <MainApp/Application.h>
#include <Graphics/CameraControler.h>
#include <Graphics/CameraController1.h>
#include "Character.h"

using namespace RcEngine;

class SponzaApp : public Application
{
public:
	SponzaApp(const String& config);
	~SponzaApp(void);

protected:

	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void CalculateFrameRate();

	void InitGUI();
	void DrawUI();

protected:
	RcEngine::Test::FPSCameraControler* mCameraControler;

	Character* mPlayer;

	Label* mFPSLabel;
	int mFramePerSecond;


	shared_ptr<FrameBuffer> mGbuffer;
};

#endif // SponzaApp_h__
