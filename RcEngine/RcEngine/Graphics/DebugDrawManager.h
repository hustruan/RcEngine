#ifndef DebugRenderer_h__
#define DebugRenderer_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <Math/ColorRGBA.h>
#include <Math/BoundingBox.h>
#include <Math/Matrix.h>
#include <Math/Frustum.h>
#include <Math/Vector.h>
#include <Graphics/RenderOperation.h>

namespace RcEngine {

class Skeleton;

class _ApiExport DebugDrawManager : public Singleton<DebugDrawManager>
{
public:
	DebugDrawManager();
	~ DebugDrawManager();

	void OnGraphicsInitialize();

	void DrawBoundingBox(const BoundingBoxf& bbox, const ColorRGBA& color, bool depthEnabled = true);
	void DrawSphere(const float3& center, float radius, const ColorRGBA& color, bool depthEnabled = true);
	void DrawSkeleton(const float4x4& transform, shared_ptr<Skeleton> skeleton, const ColorRGBA& color);
	void DrawFrustum(const Frustumf& frustum, const ColorRGBA& color, bool depthEnabled = true);

private:
	shared_ptr<Effect> mDebugShapeEffect;

	RenderOperation mSphereRop;
	RenderOperation mBoxRop;
	RenderOperation mBoneRop;
	RenderOperation mLineRop;
	RenderOperation mFrustumRop;
	RenderOperation mFullscreenTriangleROP;
	
	std::map<shared_ptr<Skeleton>, RenderOperation> mSkeletonRopMap;
};


}

#endif // DebugRenderer_h__
