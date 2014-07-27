#ifndef RenderJob_h__
#define RenderJob_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/BoundingBox.h>

namespace RcEngine {

class _ApiExport MeshPart
{
	friend class Mesh;

public:
	MeshPart(Mesh& mesh);
	~MeshPart();

	const String& GetName() const								{ return mName; }
	const BoundingBoxf& GetBoundingBox() const			    { return mBoundingBox; }

	void GetRenderOperation( RenderOperation& op, uint32_t lodIndex );

	uint32_t GetVertexCount() const				{ return mVertexCount; }
	uint32_t GetPrimitiveCount() const			{ return mPrimitiveCount; }
	uint32_t GetIndexCount() const				{ return mIndexCount; }
	uint32_t GetStartIndex() const				{ return mIndexStart; }
	uint32_t GetStartVertex() const			    { return mVertexStart;}

	const String& GetMaterialName() const		{ return mMaterialName; }

	void SetVertexBuffer(const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& vd);
	const shared_ptr<GraphicsBuffer>& GetVertexBuffer() const			{ return mVertexBuffer; }
	const shared_ptr<VertexDeclaration>& GetVertexDeclaration() const	{ return mVertexDecl;}

	void SetIndexBuffer(const shared_ptr<GraphicsBuffer>& indexBuffer, IndexBufferType format);
	const shared_ptr<GraphicsBuffer>& GetIndexBuffer()	const	{ return mIndexBuffer; }
	IndexBufferType GetIndexFormat() const						{ return mIndexFormat;  }

	void Load(Stream& source);
	void Save(Stream& source);

private:
	Mesh& mParentMesh;

	String mName;
	String mMaterialName;

	shared_ptr<GraphicsBuffer>    mVertexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;

	shared_ptr<GraphicsBuffer> mIndexBuffer;
	IndexBufferType			   mIndexFormat;

	BoundingBoxf mBoundingBox;

	uint32_t mPrimitiveCount; // Only support triangle

	uint32_t mIndexStart;
	uint32_t mIndexCount;

	uint32_t mVertexStart;
	uint32_t mVertexCount;
};

}


#endif // RenderJob_h__

