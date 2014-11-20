#include <Graphics/Geometry.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/RenderOperation.h>
#include <Core/Environment.h>
#include <Math/Math.h>

namespace RcEngine {

void BuildShpereOperation( RenderOperation& oOperation, float r, int32_t nRings, int32_t nSegments )
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	int32_t vertexCount = (nRings + 1) * (nSegments+1);
	int32_t indicesCount =  6 * nRings * (nSegments + 1);

	uint32_t vbSize = 3 * vertexCount * sizeof(float);
	shared_ptr<GraphicsBuffer> vertexBuffer= factory->CreateVertexBuffer(vbSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, nullptr);

	uint32_t ibSize = indicesCount * sizeof(unsigned short);
	shared_ptr<GraphicsBuffer> indexBuffer = factory->CreateIndexBuffer(ibSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Index, nullptr);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
	unsigned short* pIndices = static_cast<unsigned short*>(indexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));

	float fDeltaRingAngle = Mathf::PI / nRings;
	float fDeltaSegAngle = Mathf::TWO_PI / nSegments;
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			if (ring != nRings) {
				// each vertex (except the last) has six indices pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;               
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;				
			}
		}; // end for seg
	} // end for ring

	vertexBuffer->UnMap();
	indexBuffer->UnMap();

	oOperation.PrimitiveType = PT_Triangle_List;
	oOperation.BindVertexStream(0, vertexBuffer);
	oOperation.BindIndexStream(indexBuffer, IBT_Bit16);
	oOperation.IndexCount = indicesCount;

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	oOperation.VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);
}

void BuildConeOperation( RenderOperation& oOperation, float radius, float height, int32_t nCapSegments )
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	uint16_t vertexCount = nCapSegments+1;
	uint16_t indicesCount = (nCapSegments+nCapSegments-2)*3;

	uint32_t vbSize = 3 * vertexCount * sizeof(float);
	shared_ptr<GraphicsBuffer> vertexBuffer= factory->CreateVertexBuffer(vbSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, nullptr);

	uint32_t ibSize = indicesCount * sizeof(uint16_t);
	shared_ptr<GraphicsBuffer> indexBuffer = factory->CreateIndexBuffer(ibSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Index,nullptr);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
	uint16_t* pIndices = static_cast<uint16_t*>(indexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));

	std::vector<float3> Vertices;
	std::vector<int> Indices;

	uint16_t topPointOffset = 0;
	*pVertex++ = 0.0f;
	*pVertex++ = 0.0f;
	*pVertex++ = 0.0f;

	int ringStartOffset = 1;
	float deltaAngle = (Mathf::TWO_PI / nCapSegments);
	for (uint16_t i = 0; i < nCapSegments; i++)
	{
		float x0 = radius* cosf(i*deltaAngle);
		float z0 = radius * sinf(i*deltaAngle);

		*pVertex++ = x0;
		*pVertex++ = height;
		*pVertex++ = z0;
	}

	for (uint16_t i = 0; i < nCapSegments; ++i)
	{
		*pIndices++ = topPointOffset;
		*pIndices++ = ringStartOffset+i;
		*pIndices++ = ringStartOffset+ (i+1)%nCapSegments;
	}

	// Caps
	for (uint16_t i = 0; i < nCapSegments - 2; ++i)
	{
		*pIndices++ = ringStartOffset;
		*pIndices++ = ringStartOffset+i+1+1;
		*pIndices++ = ringStartOffset+i+1;
	}

	vertexBuffer->UnMap();
	indexBuffer->UnMap();


	oOperation.PrimitiveType = PT_Triangle_List;
	oOperation.BindVertexStream(0, vertexBuffer);
	oOperation.BindIndexStream(indexBuffer, IBT_Bit16);
	oOperation.IndexCount = indicesCount;

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	oOperation.VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);
}

void BuildBoxOperation( RenderOperation& oOperation )
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	float3 vertices[] =
	{
		float3( -1.0f,  1.0f, -1.0f ),
		float3(  1.0f,  1.0f, -1.0f ),
		float3(  1.0f,  1.0f,  1.0f ),
		float3( -1.0f,  1.0f,  1.0f ),
		float3( -1.0f, -1.0f, -1.0f ),
		float3(  1.0f, -1.0f, -1.0f ),
		float3(  1.0f, -1.0f,  1.0f ),
		float3( -1.0f, -1.0f,  1.0f ),
	};

	uint16_t indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};


	ElementInitData vInit;
	vInit.pData = vertices;
	vInit.rowPitch = sizeof(vertices);
	oOperation.BindVertexStream(0, factory->CreateVertexBuffer(sizeof(vertices), EAH_GPU_Read, BufferCreate_Vertex, &vInit));

	ElementInitData iInit;
	iInit.pData = indices;
	iInit.rowPitch = sizeof(indices);
	oOperation.BindIndexStream(factory->CreateIndexBuffer(sizeof(indices), EAH_GPU_Read, BufferCreate_Index, &iInit), IBT_Bit16);

	oOperation.PrimitiveType = PT_Triangle_List;
	oOperation.IndexCount = ARRAY_SIZE(indices);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	oOperation.VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);
}

void BuildFullscreenTriangle( RenderOperation& oOperation )
{
	oOperation.SetVertexRange(0, 3);
	oOperation.PrimitiveType = PT_Triangle_List;
}

void BuildFrustumOperation( RenderOperation& oOperation )
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	float3 vertices[] =
	{
		float3( -1.0f,  1.0f, -1.0f ),
		float3(  1.0f,  1.0f, -1.0f ),
		float3(  1.0f,  1.0f,  1.0f ),
		float3( -1.0f,  1.0f,  1.0f ),
		float3( -1.0f, -1.0f, -1.0f ),
		float3(  1.0f, -1.0f, -1.0f ),
		float3(  1.0f, -1.0f,  1.0f ),
		float3( -1.0f, -1.0f,  1.0f ),
	};

	uint16_t indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};


	ElementInitData vInit;
	vInit.pData = vertices;
	vInit.rowPitch = sizeof(vertices);
	oOperation.BindVertexStream(0, factory->CreateVertexBuffer(sizeof(vertices), EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, &vInit));

	ElementInitData iInit;
	iInit.pData = indices;
	iInit.rowPitch = sizeof(indices);
	oOperation.BindIndexStream(factory->CreateIndexBuffer(sizeof(indices), EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Index, &iInit), IBT_Bit16);

	oOperation.PrimitiveType = PT_Triangle_List;
	oOperation.IndexCount = ARRAY_SIZE(indices);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	oOperation.VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);
}

}