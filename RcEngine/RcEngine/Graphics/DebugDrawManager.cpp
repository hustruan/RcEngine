#include <Graphics/DebugDrawManager.h>
#include <Graphics/Skeleton.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Geometry.h>
#include <Graphics/Camera.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>
#include <Core/Environment.h>

namespace RcEngine {

DebugDrawManager::DebugDrawManager()
{

}

DebugDrawManager::~DebugDrawManager()
{

}

void DebugDrawManager::OnGraphicsInitialize()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();

	BuildShpereOperation(mSphereRop, 1.0f);
	BuildBoxOperation(mBoxRop);
	BuildFrustumOperation(mFrustumRop);

	mDebugShapeEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DebugView.effect.xml", "General");
}

void DebugDrawManager::DrawSphere( const float3& center, float radius, const ColorRGBA& color, bool depthEnabled /*= true*/ )
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	shared_ptr<Camera> viewCamera = device->GetScreenFrameBuffer()->GetCamera();

	float4x4 world(radius,		0,			0,			0, 
				   0,			radius,		0,			0, 
				   0,			0,		    radius,		0,  
				   center[0],   center[1],  center[2],  1);

	mDebugShapeEffect->GetParameterByName("World")->SetValue(world);

	float3 vColor(color[0], color[1], color[2]);
	mDebugShapeEffect->GetParameterByName("Color")->SetValue(vColor);

	mDebugShapeEffect->GetParameterByName("ViewProj")->SetValue(viewCamera->GetEngineViewProjMatrix());

	if (depthEnabled)
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(0), mSphereRop);
	}
	else
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(1), mSphereRop);
	}	
}

void DebugDrawManager::DrawBoundingBox( const BoundingBoxf& bbox, const ColorRGBA& color, bool depthEnabled /*= true*/ )
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	shared_ptr<Camera> viewCamera = device->GetScreenFrameBuffer()->GetCamera();

	float3 diagonal = bbox.Max - bbox.Min;
	float3 center = bbox.Center();

	float4x4 world(diagonal.X(),		0,			0,			0, 
					0,			diagonal.Y(),		0,			0, 
					0,			0,		    diagonal.Z(),		0,  
					center[0],   center[1],  center[2],  1);

	mDebugShapeEffect->GetParameterByName("World")->SetValue(world);

	float3 vColor(color[0], color[1], color[2]);
	mDebugShapeEffect->GetParameterByName("Color")->SetValue(vColor);

	mDebugShapeEffect->GetParameterByName("ViewProj")->SetValue(viewCamera->GetEngineViewProjMatrix());

	if (depthEnabled)
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(0), mBoxRop);
	}
	else
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(1), mBoxRop);
	}	
}

void DebugDrawManager::DrawSkeleton( const float4x4& transform, shared_ptr<Skeleton> skeleton, const ColorRGBA& color )
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	shared_ptr<Camera> viewCamera = device->GetScreenFrameBuffer()->GetCamera();

	if (mSkeletonRopMap.count(skeleton))
	{
		shared_ptr<GraphicsBuffer> vb = mSkeletonRopMap[skeleton].VertexStreams[0];
		float3* pVertices = static_cast<float3*>(vb->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
		
		for (uint32_t i = 0; i < skeleton->GetNumBones(); ++i)
		{
			Bone* bone = skeleton->GetBone(i);
			if (bone->GetParent())
			{
				*pVertices++ = Transform(bone->GetWorldPosition(), transform);
				*pVertices++ = Transform(bone->GetParent()->GetWorldPosition(), transform);
			}
		}

		vb->UnMap();
	}
	else
	{
		std::vector<float3> vertices;
		for (uint32_t i = 0; i < skeleton->GetNumBones(); ++i)
		{
			Bone* bone = skeleton->GetBone(i);
			if (bone->GetParent())
			{
				vertices.push_back(Transform(bone->GetWorldPosition(), transform));
				vertices.push_back(Transform(bone->GetParent()->GetWorldPosition(), transform));
			}
		}

		RenderOperation& rop = mSkeletonRopMap[skeleton];
		rop.PrimitiveType = PT_Line_List;

		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		ElementInitData vInitData;
		vInitData.pData = &vertices[0];
		vInitData.rowPitch = vertices.size() * sizeof(float3);
		rop.BindVertexStream(0, device->GetRenderFactory()->CreateVertexBuffer(vertices.size() * sizeof(float3), EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, &vInitData));
		rop.SetVertexRange(0, vertices.size());
		
		VertexElement ve(0, VEF_Float3, VEU_Position, 0);
		rop.VertexDecl = factory->CreateVertexDeclaration(&ve, 1);
	}

	mDebugShapeEffect->GetParameterByName("World")->SetValue(float4x4::Identity());

	float3 vColor(color[0], color[1], color[2]);
	mDebugShapeEffect->GetParameterByName("Color")->SetValue(vColor);

	mDebugShapeEffect->GetParameterByName("ViewProj")->SetValue(viewCamera->GetEngineViewProjMatrix());

	device->Draw(mDebugShapeEffect->GetTechniqueByIndex(1), mSkeletonRopMap[skeleton]);
}

void DebugDrawManager::DrawFrustum( const Frustumf& frustum, const ColorRGBA& color, bool depthEnabled /*= true*/ )
{
	shared_ptr<GraphicsBuffer> vertexBuffer = mFrustumRop.VertexStreams.front();

	float3* pData = (float3*)vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard);

	pData[0] = frustum.Corner[0];
	pData[1] = frustum.Corner[1];
	pData[2] = frustum.Corner[5];
	pData[3] = frustum.Corner[4];
	pData[4] = frustum.Corner[2];
	pData[5] = frustum.Corner[3];
	pData[6] = frustum.Corner[7];
	pData[7] = frustum.Corner[6];

	vertexBuffer->UnMap();

	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
	shared_ptr<Camera> viewCamera = device->GetScreenFrameBuffer()->GetCamera();


	mDebugShapeEffect->GetParameterByName("World")->SetValue(float4x4::Identity());

	float3 vColor(color[0], color[1], color[2]);
	mDebugShapeEffect->GetParameterByName("Color")->SetValue(vColor);

	mDebugShapeEffect->GetParameterByName("ViewProj")->SetValue(viewCamera->GetEngineViewProjMatrix());

	if (depthEnabled)
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(0), mFrustumRop);
	}
	else
	{
		device->Draw(mDebugShapeEffect->GetTechniqueByIndex(1), mFrustumRop);
	}	
}

}