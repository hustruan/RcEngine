#ifndef ShadowMapApp_h__
#define ShadowMapApp_h__

#include <MainApp/Application.h>
#include <Graphics/CameraController1.h>
#include <Graphics/RenderPath.h>

using namespace RcEngine;

class ShadowMapApp : public Application
{
public:
	ShadowMapApp(const String& config);
	~ShadowMapApp(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent() {}
	void Update(float deltaTime);
	void Render();
	void CalculateFrameRate();
	void WindowResize(uint32_t width, uint32_t height);

private:

	int mFramePerSecond;
	RenderDevice* mDevice;
	shared_ptr<Camera> mMainCamera;
	shared_ptr<RenderPath> mRenderPath;
	shared_ptr<Test::FPSCameraControler> mCameraControler;
};

#endif // ShadowMapApp_h__
