#include <Graphics/Sky.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/Material.h>
#include <Resource/ResourceManager.h>
#include <Core/Environment.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>

//namespace {
//
//using namespace RcEngine;
//
//typedef vector<float3> VertexList;
//typedef vector<uint32_t> IndexList;
//
//static const char* SkyBoxPlaneName[Sky::MaxPlaneCount] = {
//	"FrontPlane", "BackPlane", "LeftPlane", "RightPlane", "TopPlane", "BottomPlane"
//};
//
//void Subdivide(VertexList& vertices, IndexList& indices)
//{
//	VertexList vin = vertices;
//	IndexList  iin = indices;
//
//	vertices.resize(0);
//	indices.resize(0);
//
//	//       v1
//	//       *
//	//      / \
//	//     /   \
//	//  m0*-----*m1
//	//   / \   / \
//	//  /   \ /   \
//	// *-----*-----*
//	// v0    m2     v2
//
//	size_t numTris = iin.size()/3;
//	for(size_t i = 0; i < numTris; ++i)
//	{
//		float3 v0 = vin[ iin[i*3+0] ];
//		float3 v1 = vin[ iin[i*3+1] ];
//		float3 v2 = vin[ iin[i*3+2] ];
//
//		float3 m0 = 0.5f*(v0 + v1);
//		float3 m1 = 0.5f*(v1 + v2);
//		float3 m2 = 0.5f*(v0 + v2);
//
//		vertices.push_back(v0); // 0
//		vertices.push_back(v1); // 1
//		vertices.push_back(v2); // 2
//		vertices.push_back(m0); // 3
//		vertices.push_back(m1); // 4
//		vertices.push_back(m2); // 5
//
//		indices.push_back(i*6+0);
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+5);
//
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+4);
//		indices.push_back(i*6+5);
//
//		indices.push_back(i*6+5);
//		indices.push_back(i*6+4);
//		indices.push_back(i*6+2);
//
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+1);
//		indices.push_back(i*6+4);
//	}
//}
//
//void BuildGeometrySphere(uint32_t numSubdivisions, float radius, VertexList& vertices, IndexList& indices)
//{
//	// Put a cap on the number of subdivisions.
//	numSubdivisions = (std::min)(numSubdivisions, uint32_t(5));
//
//	// Approximate a sphere by tesselating an icosahedron.
//
//	const float X = 0.525731f; 
//	const float Z = 0.850651f;
//
//	float3 pos[12] = 
//	{
//		float3(-X, 0.0f, Z),  float3(X, 0.0f, Z),  
//		float3(-X, 0.0f, -Z), float3(X, 0.0f, -Z),    
//		float3(0.0f, Z, X),   float3(0.0f, Z, -X), 
//		float3(0.0f, -Z, X),  float3(0.0f, -Z, -X),    
//		float3(Z, X, 0.0f),   float3(-Z, X, 0.0f), 
//		float3(Z, -X, 0.0f),  float3(-Z, -X, 0.0f)
//	};
//
//	uint32_t k[60] = 
//	{
//		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,    
//		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,    
//		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0, 
//		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7 
//	};
//
//	vertices.resize(12);
//	indices.resize(60);
//
//	for(int i = 0; i < 12; ++i)
//		vertices[i] = pos[i];
//
//	for(int i = 0; i < 60; ++i)
//		indices[i] = k[i];
//
//	for(UINT i = 0; i < numSubdivisions; ++i)
//		Subdivide(vertices, indices);
//
//	// Project vertices onto sphere and scale.
//	for(size_t i = 0; i < vertices.size(); ++i)
//	{
//		vertices[i] = Normalize(vertices[i]);
//		vertices[i] *= radius;
//	}
//}
//
//struct SkyPlaneVertex
//{
//	float3 Position;
//	float3 Tex;		// use texture array, third dimension is sky texture index
//
//	SkyPlaneVertex() {}
//	SkyPlaneVertex( const float3& pos, const float3& tex ) : Position(pos), Tex(tex) {}
//};
//
//}
//
//
//namespace RcEngine {
//
////---------------------------------------------------------------------------
//Sky::Sky( float size  , bool cube /*= false*/ )
//	: mCubeMapSky(cube), 
//	  mRenderOperation(new RenderOperation)
//{
//	if (!cube)
//	{
//		InitializeSkyBox(size);	
//	}
//	else
//	{
//		InitializeSkyCubeMap(size);
//	}
//
//	// init render operation
//	mRenderOperation->PrimitiveType = PT_Triangle_List;
//	mRenderOperation->BindVertexStream(mVertexBuffer, mVertexDecl);
//	mRenderOperation->BindIndexStream(mIndexBuffer, IBT_Bit32);
//}
//
//Sky::~Sky()
//{
//
//}
//
//void Sky::SetMaterial( const shared_ptr<Material>& mat )
//{
//	mMaterial = mat;
//}
//
//void Sky::InitializeSkyBox(float size)
//{
//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
//
//	// Create vertex declaration
//	VertexElement vertexElems[] = {
//		VertexElement(0, VEF_Float3, VEU_Position),
//		VertexElement(sizeof(float3), VEF_Float3, VEU_TextureCoordinate)
//	};
//
//	mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 2);
//
//	SkyPlaneVertex vertices[Sky::MaxPlaneCount*4];
//
//	size_t base;
//	
//	// Left
//	base = Sky::Left * 4;
//	vertices[base+0].Position = float3( -size, -size, size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Left));
//	vertices[base+1].Position = float3( -size, -size, -size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Left));
//	vertices[base+2].Position = float3( -size, size, -size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Left));
//	vertices[base+3].Position = float3( -size, size, size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Left));
//
//	// Right
//	base = Sky::Right * 4;
//	vertices[base+0].Position = float3( size, -size, -size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Right));
//	vertices[base+1].Position = float3( size, -size, size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Right));
//	vertices[base+2].Position = float3( size, size, size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Right));
//	vertices[base+3].Position = float3( size, size, -size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Right));
//
//	// Up
//	base = Sky::Up * 4;
//	vertices[base+0].Position = float3( -size, size, -size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Up));
//	vertices[base+1].Position = float3( size, size, -size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Up));
//	vertices[base+2].Position = float3( size, size, size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Up));
//	vertices[base+3].Position = float3( -size, size, size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Up));
//
//	// Down
//	base = Sky::Down * 4;
//	vertices[base+0].Position = float3( -size, -size, size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Down));
//	vertices[base+1].Position = float3( size, -size, size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Down));
//	vertices[base+2].Position = float3( size, -size, -size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Down));
//	vertices[base+3].Position = float3( -size, -size, -size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Down));
//
//	// Front
//	base = Sky::Front * 4;
//	vertices[base+0].Position = float3( -size, -size, -size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Front));
//	vertices[base+1].Position = float3( size, -size, -size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Front));
//	vertices[base+2].Position = float3( size, size, -size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Front));
//	vertices[base+3].Position = float3( -size, size, -size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Front));
//
//	// Back
//	base = Sky::Back * 4;
//	vertices[base+0].Position = float3( size, -size, size );
//	vertices[base+0].Tex = float3(0.0f, 1.0f, float(Sky::Back));
//	vertices[base+1].Position = float3( -size, -size, size );
//	vertices[base+1].Tex = float3(1.0f, 1.0f, float(Sky::Back));
//	vertices[base+2].Position = float3( -size, size, size );
//	vertices[base+2].Tex = float3(1.0f, 0.0f, float(Sky::Back));
//	vertices[base+3].Position = float3( size, size, size );
//	vertices[base+3].Tex = float3(0.0f, 0.0f, float(Sky::Back));
//
//	ElementInitData vInitData;
//	vInitData.pData = vertices;
//	vInitData.rowPitch = sizeof(vertices);
//	mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);
//
//	uint32_t indices[36];
//	for (size_t i = 0; i < 6; ++i)
//	{
//		indices[i*6+0] = 0 + i*4;
//		indices[i*6+1] = 1+ i*4;
//		indices[i*6+2] = 3+ i*4;
//		indices[i*6+3] = 3+ i*4;
//		indices[i*6+4] = 1+ i*4;
//		indices[i*6+5] = 2+ i*4;
//	}
//
//	ElementInitData iInitData;
//	iInitData.pData = indices;
//	iInitData.rowPitch = sizeof(indices);
//	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);
//
//	// Load material
//	//mMaterial = factory.CreateMaterialFromFile("SkyNormal", "../Media/Materials/SkyNormal.material.xml");
//}
//
//void Sky::InitializeSkyCubeMap(float size)
//{
//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
//
//	// Create vertex declaration
//	VertexElement vertexElems[] = {
//		VertexElement(0, VEF_Float3, VEU_Position),
//	};
//	mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 1);
//
//	std::vector<float3> vertices;
//	std::vector<uint32_t> indices;
//	BuildGeometrySphere(2, size, vertices, indices);
//
//	for(size_t i = 0; i < vertices.size(); ++i)
//	{
//		// Scale on y-axis to turn into an ellipsoid to make a flatter Sky surface
//		vertices[i] = 0.5f*vertices[i];
//	}
//
//	ElementInitData vInitData;
//	vInitData.pData = &vertices[0];
//	vInitData.rowPitch = sizeof(float3) * vertices.size();
//	mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);
//
//	ElementInitData iInitData;
//	iInitData.pData = &indices[0];
//	iInitData.rowPitch = sizeof(uint32_t) * indices.size();
//	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);
//
//	// Load material
//	mMaterial = std::static_pointer_cast<Material>(
//		ResourceManager::GetSingleton().GetResourceByName(RT_Material, "SkyCubeMap.material.xml", "General"));
//
//	mMaterial->Load();
//}

namespace RcEngine {

shared_ptr<RenderOperation> CreateCubeShape() 
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
	
	const int32_t vertexCount = 36;

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = sizeof(float3) * vertexCount;
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer = factory->CreateVertexBuffer(vInitData.rowPitch, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, &vInitData);

	VertexElement vdsc[] = { VertexElement(0, VEF_Float3,  VEU_Position, 0),};
	shared_ptr<VertexDeclaration> vertexDecl = factory->CreateVertexDeclaration(vdsc, 1);

	float3* pVertices = static_cast<float3*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));

	// +X : Right
	pVertices[0]	    	= float3(1.f,-1.f,-1.f);
	pVertices[1]	    	= float3(1.f, 1.f,-1.f);
	pVertices[2] 			= float3(1.f, 1.f, 1.f);
	pVertices[3]	    	= float3(1.f,-1.f,-1.f);
	pVertices[4] 			= float3(1.f, 1.f, 1.f);
	pVertices[5]		    = float3(1.f,-1.f, 1.f);

	// -X : Left
	pVertices[6]	    	= float3(-1.f,-1.f,-1.f);
	pVertices[7] 			= float3(-1.f, 1.f, 1.f);
	pVertices[8]	    	= float3(-1.f, 1.f,-1.f);
	pVertices[9]			= float3(-1.f,-1.f,-1.f);
	pVertices[10]			= float3(-1.f,-1.f, 1.f);
	pVertices[11] 			= float3(-1.f, 1.f, 1.f);

	// +Y : Back
	pVertices[12]			= float3(-1.f, 1.f,-1.f);
	pVertices[13] 			= float3( 1.f, 1.f, 1.f);
	pVertices[14]			= float3( 1.f, 1.f,-1.f);
	pVertices[15]			= float3(-1.f, 1.f,-1.f);
	pVertices[16]			= float3(-1.f, 1.f, 1.f);
	pVertices[17] 			= float3( 1.f, 1.f, 1.f);

	// -Y : Front
	pVertices[18]			= float3(-1.f,-1.f,-1.f);
	pVertices[19]			= float3( 1.f,-1.f,-1.f);
	pVertices[20] 			= float3( 1.f,-1.f, 1.f);
	pVertices[21]			= float3(-1.f,-1.f,-1.f);
	pVertices[22] 			= float3( 1.f,-1.f, 1.f);
	pVertices[23]			= float3(-1.f,-1.f, 1.f);

	// +Z : Top
	pVertices[24]			= float3(-1.f,-1.f, 1.f);
	pVertices[25]			= float3( 1.f,-1.f, 1.f);
	pVertices[26] 			= float3( 1.f, 1.f, 1.f);
	pVertices[27]			= float3(-1.f,-1.f, 1.f);
	pVertices[28] 			= float3( 1.f, 1.f, 1.f);
	pVertices[29]			= float3(-1.f, 1.f, 1.f);

	// -Z : Bottom
	pVertices[30]			= float3(-1.f,-1.f,-1.f);
	pVertices[31] 			= float3( 1.f, 1.f,-1.f);
	pVertices[32]			= float3( 1.f,-1.f,-1.f);
	pVertices[33]			= float3(-1.f,-1.f,-1.f);
	pVertices[34]			= float3(-1.f, 1.f,-1.f);
	pVertices[35] 			= float3( 1.f, 1.f,-1.f);

	vertexBuffer->UnMap();

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(0, vertexBuffer);
	mRenderOperation->SetVertexRange(0, vertexCount);
	mRenderOperation->VertexDecl = vertexDecl;

	return mRenderOperation;
}

SkyBox::SkyBox()
{
	mRenderOperation = CreateCubeShape();
	
	// Load material
	mMaterial = std::static_pointer_cast<Material>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Material, "SkyCubeMap.material.xml", "General"));

	mMaterial->Load();
}

void RcEngine::SkyBox::SetPosition( const float3& position )
{
	mWorldTransform = CreateTranslation(position);
}

void SkyBox::GetWorldTransforms( float4x4* xform ) const
{
	xform[0] = mWorldTransform;
}


}