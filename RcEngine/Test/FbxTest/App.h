#ifndef App_h__
#define App_h__
#include <MainApp/Application.h>
#include <Graphics/Renderer.h>
#include <Graphics/CameraController1.h>

using namespace RcEngine;

class FBXApp : public Application
{
public:
	FBXApp(const String& config);
	~FBXApp(void);
protected:

	void Initialize();
	void InitGUI();
	void LoadContent();
	void UnloadContent() {}
	void Update(float deltaTime);
	void Render();
	void CalculateFrameRate();

private:
	void VisualizeCascades(bool checked);
	void CastShadow(bool checked);
	void BlendAreaSliderValueChange( int32_t value );

protected:
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;
	shared_ptr<Pipeline> mPipeline;
	Renderer* mSceneRender;

	shared_ptr<Material> mMaterial;
	Label* mLabel;
	UIWindow* mWindow;
	Slider* mBlendAreaSlider;
	Label* mBlendAreaLabel;
	CheckBox* mVisualizeCascadesCheckBox;
	CheckBox* mCastShadowCheckBox;
	Light* mDirLight;
	Light* mSpotLight;
	CascadedShadowMap* mCascadedShadowMap;
};


#endif // App_h__
