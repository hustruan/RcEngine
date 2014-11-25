#ifndef DeferredPath_h__
#define DeferredPath_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderOperation.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>
#include <Resource/Resource.h>

namespace RcEngine {

class CascadedShadowMap;
class RenderDevice;
class AmbientOcclusion;
class SSAO;

class _ApiExport RenderPath
{
public:
	RenderPath();
	virtual ~RenderPath() {}

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height) {}
	virtual void RenderScene() {}

protected:
	void DrawFSQuad(const shared_ptr<Material>& material, const String& tech);
	void DrawOverlays();

protected:
	RenderDevice* mDevice;
	SceneManager* mSceneMan;
	shared_ptr<Camera> mCamera;

	RenderOperation mFullscreenTrangle;
};

/**
 * Multiple pass forward shading.
 */
class _ApiExport ForwardPath : public RenderPath
{
public:
	ForwardPath();
	virtual ~ForwardPath();

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

	CascadedShadowMap* GetShadowManager() const  { return mShadowMan; }

public:
	CascadedShadowMap* mShadowMan;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;
};

/**
 * Stencil-Optimized light pre-pass deferred rendering.
 */
class _ApiExport DeferredPath : public RenderPath
{
public:
	DeferredPath();
	virtual ~DeferredPath();

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

	shared_ptr<Effect> GetDeferredEffect() const { return mDeferredEffect; } 
	CascadedShadowMap* GetShadowManager() const  { return mShadowMan; }
	AmbientOcclusion* GetAmbientOcclusion() const { return mAmbientOcclusion; }

protected:

	void CreateBuffers(uint32_t width, uint32_t height);

	void GenereateGBuffer();
	void DeferredLighting(); // Lighting pass
	void DeferredShading();  // Shading pass
	void PostProcess();


	void DrawDirectionalLightShape(Light* light);
	void DrawSpotLightShape(Light* light);
	void DrawPointLightShape(Light* light);

protected:

	CascadedShadowMap* mShadowMan;
	AmbientOcclusion* mAmbientOcclusion;
	shared_ptr<SSAO> mSSAO;

	// Normal + Specular Shininess,  Albedo + Specular Intensity
	shared_ptr<Texture> mGBuffer[2];
	shared_ptr<RenderView> mGBufferRTV[2];
	
	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;
	shared_ptr<RenderView> mDepthStencilViewReadOnly;

	shared_ptr<Texture> mLightAccumulateBuffer;
	shared_ptr<Texture> mDepthStencilBufferLight; // Used for stencil optimization for light volume
	shared_ptr<RenderView> mLightAccumulateRTV;
	shared_ptr<RenderView> mDepthStencilBufferLightView;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<FrameBuffer> mGBufferFB;
	shared_ptr<FrameBuffer> mLightAccumulateFB;
	shared_ptr<FrameBuffer> mHDRFB;

	shared_ptr<Effect> mDeferredEffect;
	shared_ptr<Effect> mToneMapEffect;

	// Todo: Move to DebugRender
	shared_ptr<Effect> mDebugEffect;

	EffectTechnique* mDirLightTech;
	EffectTechnique* mPointLightTech;
	EffectTechnique* mSpotLightTech;
	EffectTechnique* mShadingTech;


	RenderOperation mSpotLightShape;
	RenderOperation mPointLightShape;
	
	float4x4 mInvViewProj;
	float4x4 mViewProj;

public:
	bool mVisualLights;
	bool mVisualLightsWireframe;
};

/**
 * Tile based deferred shading
 *
 * Todo: refactor a base class for both non-tiled and tiled deferred path 
 */
class _ApiExport TiledDeferredPath : public RenderPath
{
public:
	TiledDeferredPath();
	virtual ~TiledDeferredPath();

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

private:
	void GenereateGBuffer();
	void TiledDeferredLighting();
	void DeferredShading();  // Shading pass

private:

	enum { TileGroupSize = 32 };
	enum { MaxNumLights = 1500 };

	// Normal + Specular Shininess,  Albedo + Specular Intensity
	shared_ptr<Texture> mGBuffer[2];
	shared_ptr<RenderView> mGBufferRTV[2];

	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;
	shared_ptr<RenderView> mDepthStencilViewReadOnly;

	shared_ptr<Texture> mLightAccumulation;
	shared_ptr<UnorderedAccessView> mLightAccumulationUAV;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<FrameBuffer> mGBufferFB;
	shared_ptr<FrameBuffer> mLightAccumulateFB;
	shared_ptr<FrameBuffer> mHDRFB;

	shared_ptr<Effect> mTiledDeferredEffect;
	shared_ptr<Effect> mToneMapEffect;

	/**
	 * Unlike HLSL StructureBuffer, GLSL SSBO is not tightly packed. 
	 * In order to make the C++ structure can both used in HLSL and GLSL, 
	 * It has to add some padding in HLSL structure.
	 */
	struct PointLight
	{
		float3 Color;
		float Range;

		float3 Position;
		uint32_t : 32;  // Padding

		float3 Falloff;
		uint32_t : 32;  // Padding
	};
	shared_ptr<GraphicsBuffer> mLightBuffer;
	shared_ptr<ShaderResourceView> mLightBufferSRV; 

	EffectTechnique* mTileTech;
	EffectTechnique* mShadingTech;
};

class _ApiExport ForwardPlusPath : public RenderPath
{
public:
	ForwardPlusPath();
	~ForwardPlusPath();

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

private:
	void DepthPrePass();
	void TiledLightCulling();
	void ForwardShading();

private:
	
	enum { TileGroupSize = 32 };
	enum { MaxNumLights = 1500 };
	enum { MaxNumLightsPerTile = 256 };

	uint32_t mNumTileX, mNumTileY;

	shared_ptr<Effect> mToneMapEffect;
	shared_ptr<Effect> mTiledLightCullEfffect;
	EffectTechnique* mTileLightCullTech;

	shared_ptr<FrameBuffer> mForwardFB;

	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<GraphicsBuffer> mPointLightsPosRange;
	shared_ptr<GraphicsBuffer> mPointLightsColorFalloff;
	shared_ptr<GraphicsBuffer> mTilePointLightsRange;
	shared_ptr<GraphicsBuffer> mTilePointLightsIndexList;
	shared_ptr<GraphicsBuffer> mPointLightsIndexCounter;

	shared_ptr<ShaderResourceView> mPointLightsPosRangeSRV;
	shared_ptr<ShaderResourceView> mPointLightsColorSRV;
	shared_ptr<ShaderResourceView> mPointLightsFalloffSRV;

	shared_ptr<UnorderedAccessView> mPointLightsIndexCounterUAV;

	shared_ptr<ShaderResourceView> mTilePointLightsRangeSRV;
	shared_ptr<UnorderedAccessView> mTilePointLightsRangeUAV;

	shared_ptr<ShaderResourceView> mTilePointLightsIndexListSRV;
	shared_ptr<UnorderedAccessView> mTilePointLightsIndexListUAV;
};

}


#endif // DeferredPath_h__
