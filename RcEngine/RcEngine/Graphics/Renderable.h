#ifndef Renderable_h__
#define Renderable_h__

#include <Core/Prerequisites.h>
#include <Math/BoundingSphere.h>
#include <Math/Matrix.h>

namespace RcEngine {


/** 
 * Renderable discrete objects which will be queued in the render pipeline, grouped by material. Classes
 * implementing this interface must be based on a single material, a single world matrix (or a collection
 * of world matrices which are blended by weights), and must be renderable via a single render operation.
 */
class _ApiExport Renderable
{
public:
	Renderable();
	virtual ~Renderable();

	/**
	 * Get material used to render it.
	 */
	virtual const shared_ptr<Material>& GetMaterial() const = 0;

	/** This is to allow renderables to use a chosen Technique if they wish,
	 * otherwise they will use the best Technique available for the Material they are using.
	 */
	virtual EffectTechnique* GetTechnique() const; 

	/**
	 * Get render operation, render operation contain all geometry data.
	 */
	virtual const shared_ptr<RenderOperation>& GetRenderOperation() const = 0;

	/**
	 * Get world transform matrix, note that it may more than one matrix 
	 * if it is a bone mesh.
	 */
	virtual void GetWorldTransforms(float4x4* xform) const = 0;

	/**
	 * Get world transform matrix count.
	 */
	virtual uint32_t GetWorldTransformsCount() const = 0;

	virtual void Render();

	virtual void OnRenderBegin();
	virtual void OnRenderEnd();
};


} // Namespace RcEngine

#endif // Renderable_h__
