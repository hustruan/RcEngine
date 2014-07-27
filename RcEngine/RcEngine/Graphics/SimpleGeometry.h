#ifndef SimpleGeometry_h__
#define SimpleGeometry_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Scene/SceneObject.h>
#include <Math/BoundingSphere.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport RenderableHelper : public Renderable, public SceneObject
{
public:
	RenderableHelper(const String& name);
	virtual ~RenderableHelper();

	// overload from scene object
	virtual SceneObejctType GetSceneObjectType() const			  { return SOT_Entity; }
	
	// overload from renderable
	const shared_ptr<RenderOperation>& GetRenderOperation() const { return mRenderOperation; }
	const shared_ptr<Material>& GetMaterial() const				  { return mMaterial; }
	uint32_t GetWorldTransformsCount() const					  { return 1; }
	void GetWorldTransforms(float4x4* xform) const;
	bool Renderable() const	{ return true; }
	void SetMaterial( const shared_ptr<Material>& mat )			{ mMaterial = mat; }

protected:
	shared_ptr<Material> mMaterial;
	shared_ptr<RenderOperation> mRenderOperation;
};

class _ApiExport SimpleBox : public RenderableHelper
{
public:
	SimpleBox(const String& name);
	~SimpleBox();

protected:
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
};

class _ApiExport SimpleTexturedQuad : public RenderableHelper
{
public:
	SimpleTexturedQuad(const String& name);
	~SimpleTexturedQuad();

	void SetDiffuseTexture( const shared_ptr<Texture>& diffuseMap );

protected:
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
};

class _ApiExport Sphere : public RenderableHelper
{
public:
	Sphere(const String& name, float radius, int32_t slices, int32_t stacks);
	~Sphere() {}


protected:
	float mRadius;
	int mNumSlices, mNumStacks;
};

} // Namespace RcEngine

#endif // SimpleGeometry_h__

