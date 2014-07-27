#ifndef Sky_h__
#define Sky_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/Matrix.h>

namespace RcEngine {

//class _ApiExport Sky : public Renderable
//{
//public:
//	enum SkyBoxPlane
//	{
//		Left = 0,
//		Right,
//		Up,
//		Down,
//		Front,
//		Back,
//		MaxPlaneCount
//	};
//
//public:
//	Sky( float size,  bool cube = false );
//	~Sky();
//
//	const shared_ptr<Material>& GetMaterial() const	{ return mMaterial; }
//
//	const shared_ptr<RenderOperation>& GetRenderOperation() const	{ return mRenderOperation; }
//
//	uint32_t GetWorldTransformsCount() const { return 0; }
//	void GetWorldTransforms(float4x4* xform) const { }
//	
//	void SetMaterial( const shared_ptr<Material>& mat );
//
//private:
//	void InitializeSkyBox(float size);
//	void InitializeSkyCubeMap(float size);
//
//private:
//	bool mCubeMapSky;
//	
//	shared_ptr<GraphicsBuffer> mVertexBuffer;
//	shared_ptr<GraphicsBuffer> mIndexBuffer;
//	shared_ptr<VertexDeclaration> mVertexDecl;
//	shared_ptr<Material> mMaterial;
//
//	shared_ptr<RenderOperation> mRenderOperation;
//};

class _ApiExport SkyBox : public Renderable
{
public:
	SkyBox();
	~SkyBox() {}

	void SetPosition(const float3& position);

	const shared_ptr<Material>& GetMaterial() const					{ return mMaterial; }
	const shared_ptr<RenderOperation>& GetRenderOperation() const	{ return mRenderOperation; }
	uint32_t GetWorldTransformsCount() const						{ return 1; }
	void GetWorldTransforms(float4x4* xform) const;

protected:
	shared_ptr<RenderOperation> mRenderOperation;
	shared_ptr<Material> mMaterial;
	float4x4 mWorldTransform;
};



}

#endif // Sky_h__
