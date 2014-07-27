#ifndef Renderer_h__
#define Renderer_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Pipeline.h>
#include <Math/Matrix.h>

namespace RcEngine {

class CascadedShadowMap;

class _ApiExport Renderer
{
public:
	Renderer();
	~Renderer();

	void Init();

	void SetRenderPipeline(const shared_ptr<Pipeline>& pipeline);
	void RenderScene();

	CascadedShadowMap* GetShadowManager() const { return mCascadedShadowMap; }

//private:

	void DrawFSQuad(const String& tech);

	/**
	 * Draw geometry with light, it can used to generate shadow map
	 */
	void DrawGeometry(const String& tech, const String& matClass, RenderOrder order);

	void DrawLightShape(const String& tech);

	void DrawOverlays();

	void DrawLightGeometry(Light* light, const String& matClass);

	void DrawRenderable(Camera* camera, const String& tech, const String& matClass);

	void SetupViewProjMatirices(const float4x4& view, const float4x4& proj);

private:
	void DrawEntity();

	void UpdateMaterialParameters(Pipeline::PipelineCommand* cmd);

	void CreatePrimitives();

	void DrawDirectionalLightShape(Light* light, const String& tech);
	void DrawPointLightShape(Light* light, const String& tech);
	void DrawSpotLightShape(Light* light, const String& tech);

	void UpdateCascadeShadowMap(Light* light);
	void CalcCropMatrix();

private:
	RenderDevice* mDevice;
	SceneManager* mSceneMan;

	
	bool mShadowEnable;

	enum { MAX_CASCADES = 4 };

	shared_ptr<FrameBuffer> mShadowFB;
	CascadedShadowMap* mCascadedShadowMap;

	shared_ptr<Pipeline> mCurrPipeline;
	shared_ptr<Material> mCurrMaterial;
	
	EffectTechnique* mCurrentTechnique;
	Light* mCurrentLight;

	shared_ptr<RenderOperation> mSpotLightShape;
	shared_ptr<RenderOperation> mPointLightShape;
	shared_ptr<RenderOperation> mFSQuadShape;
};


}


#endif // Renderer_h__
