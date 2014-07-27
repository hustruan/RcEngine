#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Scene/Light.h>
#include <Math/MathUtil.h>
#include <Core/Profiler.h>
#include "Teapot.h"
#include <fstream>

using namespace RcEngine;

class TesselationApp : public Application
{
public:
	TesselationApp(const String& config)
		: Application(config)
	{
		/*FILE* f = fopen("E:/teapot", "w");
		for (int i = 0; i < ARRAY_SIZE(TeapotVertices); ++i)
		{
			fprintf(f, "{ %ff, %ff, %ff },\n", TeapotVertices[i][0], TeapotVertices[i][2], TeapotVertices[i][1]);
		}
		fclose(f);*/
	}

	virtual ~TesselationApp(void)
	{

	}

protected:
	void Initialize()
	{
		mCamera = std::make_shared<Camera>();
		mCamera->CreateLookAt(float3(0, 5, 0), float3(0, 5, 100), float3(0, 1, 0));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 1000.0f);

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(100.0f);
		mCameraControler->SetMoveInertia(true);
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		mTessEffect = resMan.GetResourceByName<Effect>(RT_Effect, "Tessellation.effect.xml", "General");
		
		float2 bezierControlPoints[] = { float2(-200, -200),  float2(-100, 180),  float2(150, 300),  float2(300, -100) };
		
		ElementInitData initData;
		initData.pData = bezierControlPoints;
		initData.rowPitch = sizeof(bezierControlPoints);

		mBezierCurveROP.BindVertexStream(0, 
			factory->CreateVertexBuffer(sizeof(bezierControlPoints), EAH_GPU_Read, BufferCreate_Vertex, &initData));

		VertexElement vertexElement[] = { VertexElement(0, VEF_Float2, VEU_Position, 0) };
		mBezierCurveROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));

		mBezierCurveROP.PrimitiveType = PT_Patch_Control_Point_4;
		mBezierCurveROP.SetVertexRange(0, 4);

		/*mTessEffect->GetParameterByName("NumSegments")->SetValue(50);
		mTessEffect->GetParameterByName("NumStrips")->SetValue(1);*/

		//////////////////////////////////////////////////////////////////////////
		float2 quad[] = { float2(-200, -200), float2(-200, 200), float2(200, 200), float2(200, -200) };
		initData.pData = quad;
		initData.rowPitch = sizeof(quad);

		mTessQuadROP.BindVertexStream(0, 
			factory->CreateVertexBuffer(sizeof(quad), EAH_GPU_Read, BufferCreate_Vertex, &initData));

		mTessQuadROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));

		mTessQuadROP.PrimitiveType = PT_Patch_Control_Point_4;
		mTessQuadROP.SetVertexRange(0, 4);

		mTessEffect->GetParameterByName("TessLevelOuter")->SetValue(float4(2, 2, 2, 2));
		mTessEffect->GetParameterByName("TessLevelInner")->SetValue(float2(8, 8));
	
		//////////////////////////////////////////////////////////////////////////
		BuildTeapotPatch();
	}

	void BuildTeapotPatch()
	{
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		ElementInitData initData;

		initData.pData = TeapotVertices;
		initData.rowPitch = sizeof(TeapotVertices);
		mTessTeapotROP.BindVertexStream(0, 
			factory->CreateVertexBuffer(sizeof(TeapotVertices), EAH_GPU_Read, BufferCreate_Vertex, &initData));

		initData.pData = TeapotIndices;
		initData.rowPitch = sizeof(TeapotIndices);
		mTessTeapotROP.BindIndexStream( 
			factory->CreateIndexBuffer(sizeof(TeapotIndices), EAH_GPU_Read, BufferCreate_Index, &initData), IBT_Bit16);

		VertexElement vertexElement[] = { VertexElement(0, VEF_Float3, VEU_Position, 0) };
		mTessTeapotROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));

		mTessTeapotROP.PrimitiveType = PT_Patch_Control_Point_16;
		mTessTeapotROP.SetIndexRange(0, NumTeapotIndices);


		//mTeapotPointsROP.BindVertexStream(0, mTessTeapotROP.VertexStreams.front());
		//mTeapotPointsROP.BindIndexStream(mTessTeapotROP.IndexBuffer, IBT_Bit16);
		//mTeapotPointsROP.PrimitiveType = PT_Point_List;
		//mTeapotPointsROP.SetVertexRange(0, 290);
		//mTeapotPointsROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));
	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		mCameraControler->Update(deltaTime);
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		device->GetScreenFrameBuffer()->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1, 0);

		float w = static_cast<float>( mMainWindow->GetWidth() );
		float h = static_cast<float>( mMainWindow->GetHeight() );
		mTessEffect->GetParameterByName("ViewportDim")->SetValue(float2(w, h));

		//device->Draw(mBezierCurveEffect->GetTechniqueByName("BezierCurve"), mBezierCurveROP);
		device->Draw(mTessEffect->GetTechniqueByName("TessQuad"), mTessQuadROP);

		/*float4x4 world = CreateScaling(5, 5, 5) * CreateTranslation(0, 0, 60);

		mTessEffect->GetParameterByName("TessLevel")->SetValue(100);
		mTessEffect->GetParameterByName("World")->SetValue(world);
		mTessEffect->GetParameterByName("ViewProj")->SetValue(mCamera->GetEngineViewProjMatrix());
		device->Draw(mTessEffect->GetTechniqueByName("TessTeapot"), mTessTeapotROP);*/

		device->GetScreenFrameBuffer()->SwapBuffers();
	}

	void WindowResize(uint32_t width, uint32_t height)
	{
	
	}

protected:

	shared_ptr<Effect> mTessEffect;
	RenderOperation mBezierCurveROP;
	RenderOperation mTessQuadROP;
	RenderOperation mTessTeapotROP;

	RenderOperation mTeapotPointsROP;

	shared_ptr<Camera> mCamera;
	Test::FPSCameraControler* mCameraControler;
};


int main()
{
	TesselationApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}