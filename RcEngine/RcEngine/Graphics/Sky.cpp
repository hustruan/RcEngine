#include <Graphics/Sky.h>
#include <Graphics/Material.h>
#include <Graphics/Geometry.h>
#include <Graphics/Renderable.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/TextureResource.h>
#include <Resource/ResourceManager.h>
#include <Core/Exception.h>

namespace RcEngine {

//shared_ptr<RenderOperation> CreateCubeShape() 
//{
//	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
//	
//	const int32_t vertexCount = 36;
//
//	ElementInitData vInitData;
//	vInitData.pData = nullptr;
//	vInitData.rowPitch = sizeof(float3) * vertexCount;
//	vInitData.slicePitch = 0;
//	shared_ptr<GraphicsBuffer> vertexBuffer = factory->CreateVertexBuffer(vInitData.rowPitch, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, &vInitData);
//
//	VertexElement vdsc[] = { VertexElement(0, VEF_Float3,  VEU_Position, 0),};
//	shared_ptr<VertexDeclaration> vertexDecl = factory->CreateVertexDeclaration(vdsc, 1);
//
//	float3* pVertices = static_cast<float3*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
//
//	// +X : Right
//	pVertices[0]	    	= float3(1.f,-1.f,-1.f);
//	pVertices[1]	    	= float3(1.f, 1.f,-1.f);
//	pVertices[2] 			= float3(1.f, 1.f, 1.f);
//	pVertices[3]	    	= float3(1.f,-1.f,-1.f);
//	pVertices[4] 			= float3(1.f, 1.f, 1.f);
//	pVertices[5]		    = float3(1.f,-1.f, 1.f);
//
//	// -X : Left
//	pVertices[6]	    	= float3(-1.f,-1.f,-1.f);
//	pVertices[7] 			= float3(-1.f, 1.f, 1.f);
//	pVertices[8]	    	= float3(-1.f, 1.f,-1.f);
//	pVertices[9]			= float3(-1.f,-1.f,-1.f);
//	pVertices[10]			= float3(-1.f,-1.f, 1.f);
//	pVertices[11] 			= float3(-1.f, 1.f, 1.f);
//
//	// +Y : Back
//	pVertices[12]			= float3(-1.f, 1.f,-1.f);
//	pVertices[13] 			= float3( 1.f, 1.f, 1.f);
//	pVertices[14]			= float3( 1.f, 1.f,-1.f);
//	pVertices[15]			= float3(-1.f, 1.f,-1.f);
//	pVertices[16]			= float3(-1.f, 1.f, 1.f);
//	pVertices[17] 			= float3( 1.f, 1.f, 1.f);
//
//	// -Y : Front
//	pVertices[18]			= float3(-1.f,-1.f,-1.f);
//	pVertices[19]			= float3( 1.f,-1.f,-1.f);
//	pVertices[20] 			= float3( 1.f,-1.f, 1.f);
//	pVertices[21]			= float3(-1.f,-1.f,-1.f);
//	pVertices[22] 			= float3( 1.f,-1.f, 1.f);
//	pVertices[23]			= float3(-1.f,-1.f, 1.f);
//
//	// +Z : Top
//	pVertices[24]			= float3(-1.f,-1.f, 1.f);
//	pVertices[25]			= float3( 1.f,-1.f, 1.f);
//	pVertices[26] 			= float3( 1.f, 1.f, 1.f);
//	pVertices[27]			= float3(-1.f,-1.f, 1.f);
//	pVertices[28] 			= float3( 1.f, 1.f, 1.f);
//	pVertices[29]			= float3(-1.f, 1.f, 1.f);
//
//	// -Z : Bottom
//	pVertices[30]			= float3(-1.f,-1.f,-1.f);
//	pVertices[31] 			= float3( 1.f, 1.f,-1.f);
//	pVertices[32]			= float3( 1.f,-1.f,-1.f);
//	pVertices[33]			= float3(-1.f,-1.f,-1.f);
//	pVertices[34]			= float3(-1.f, 1.f,-1.f);
//	pVertices[35] 			= float3( 1.f, 1.f,-1.f);
//
//	vertexBuffer->UnMap();
//
//	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
//	mRenderOperation->PrimitiveType = PT_Triangle_List;
//	mRenderOperation->BindVertexStream(0, vertexBuffer);
//	mRenderOperation->SetVertexRange(0, vertexCount);
//	mRenderOperation->VertexDecl = vertexDecl;
//
//	return mRenderOperation;
//}

//SkyBox::SkyBox()
//{
//	mRenderOperation = CreateCubeShape();
//	
//	// Load material
//	mMaterial = std::static_pointer_cast<Material>(
//		ResourceManager::GetSingleton().GetResourceByName(RT_Material, "SkyCubeMap.material.xml", "General"));
//
//	mMaterial->Load();
//}
//
//void RcEngine::SkyBox::SetPosition( const float3& position )
//{
//	mWorldTransform = CreateTranslation(position);
//}
//
//void SkyBox::GetWorldTransforms( float4x4* xform ) const
//{
//	xform[0] = mWorldTransform;
//}
//
//void SkyBox::SetTexture(const shared_ptr<Texture>& texture)
//{
//	mMaterial->SetTexture("EnvMap", texture);
//	SetActive(true);
//}
//
//void SkyBox::SetActive(bool bActive)
//{
//	mActive = bActive;
//}

class SkyBoxRenderable : public Renderable
{
public:
	SkyBoxRenderable(SkyBox& skyBox)
		: mCubeRop ( new RenderOperation ),
		  mSkyBox(skyBox)
	{
		BuildBoxOperation(*mCubeRop);
		mMaterial = ResourceManager::GetSingleton().GetResourceByName<Material>(RT_Material, "SkyCubeMap.material.xml", "General");
	}

	const shared_ptr<Material>& GetMaterial() const					{ return mMaterial; }
	const shared_ptr<RenderOperation>& GetRenderOperation() const	{ return mCubeRop; }

	uint32_t GetWorldTransformsCount() const						{ return 1; }
	void GetWorldTransforms(float4x4* xform) const					
	{
		*xform = mSkyBox.GetWorldTransform();
	}

private:
	SkyBox& mSkyBox;
	shared_ptr<Material> mMaterial;
	shared_ptr<RenderOperation> mCubeRop;
};

//////////////////////////////////////////////////////////////////////////
SkyBox::SkyBox(const String& name, const shared_ptr<Texture>& cubeSkyTexture)
	: SceneObject(name, SOT_Sky, true)
{
	mCubeBox = new SkyBoxRenderable(*this);
	SetTexture(cubeSkyTexture);
	SetFlags(NoCastShadow);
}

SkyBox::~SkyBox()
{
	SAFE_DELETE(mCubeBox);
}

void SkyBox::SetTexture(const shared_ptr<Texture>& texure)
{
	mCubeBox->GetMaterial()->SetTexture("EnvMap", texure);
}

// Return a zero extent aabb
const BoundingBoxf& SkyBox::GetWorldBoundingBox() const
{
	static BoundingBoxf aabb(Vector<float, 3>::Zero(), Vector<float, 3>::Zero());
	return aabb;
}

void SkyBox::OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& camera, RenderOrder order, uint32_t buckterFilter, uint32_t filterIgnore )
{
	if ((mFlags & filterIgnore) == 0)
	{
		if (buckterFilter & RenderQueue::BucketBackground && IsActive() )
		{
			renderQueue->AddToQueue(RenderQueueItem(mCubeBox, 0), RenderQueue::BucketBackground);			
		}
	}
}

SceneObject* SkyBox::FactoryFunc(const String& name, const NameValuePairList* params /*= 0*/)
{
	shared_ptr<TextureResource> skyTexture;

	if (params)
	{
		String groupName = "General";

		NameValuePairList::const_iterator found = params->find("ResourceGroup");
		if (found != params->end())
			groupName = found->second;

		found = params->find("Sky");
		if (found != params->end())
		{
			skyTexture = ResourceManager::GetSingleton().GetResourceByName<TextureResource>(RT_Texture, found->second, groupName);			
		}
		else
			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Texture " + found->second + " not exit!", "Entity::FactoryFunc");
	}
	else
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Create entity failed.", "Entity::FactoryFunc");

	return new SkyBox(name, skyTexture->GetTexture());
}

}