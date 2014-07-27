#include <Graphics/Renderer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderView.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Texture.h>
#include <Graphics/CascadedShadowMap.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Light.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <GUI/UIManager.h>

namespace {

using namespace RcEngine;

shared_ptr<RenderOperation> BuildFSQuadShape()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	// Fullscreen polygon
	float vertices[6] = {  // x, y, z
		-1.0f, 1.0f, 3.0f, 1.0f, -1.0f, -3.0f, 
	};

	ElementInitData vInitData;
	vInitData.pData = vertices;
	vInitData.rowPitch = 6 * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float2,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);

	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->SetVertexRange(0, 3);

	return mRenderOperation;
}

shared_ptr<RenderOperation> BuildPointLightShape()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	const int nRings = 30;
	const int nSegments = 30;
	const float r = 1.0f;

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	int32_t vertexCount = (nRings + 1) * (nSegments+1);
	int32_t indicesCount =  6 * nRings * (nSegments + 1);

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(unsigned short);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
	unsigned short* pIndices = static_cast<unsigned short*>(indexBuffer->Map(0, -1, BA_Write_Only));

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

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	return mRenderOperation;
}

shared_ptr<RenderOperation> BuildSpotLightShape() 
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	float mRadius = 1.f;
	float mHeight = 1.f;
	uint16_t nCapSegments = 30;

	uint16_t vertexCount = nCapSegments+1;
	uint16_t indicesCount = (nCapSegments+nCapSegments-2)*3;

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(uint16_t);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
	uint16_t* pIndices = static_cast<uint16_t*>(indexBuffer->Map(0, -1, BA_Write_Only));

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
		float x0 = mRadius* cosf(i*deltaAngle);
		float z0 = mRadius * sinf(i*deltaAngle);

		*pVertex++ = x0;
		*pVertex++ = mHeight;
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

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	return mRenderOperation;
}

}

namespace RcEngine {

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
	mDevice = Context::GetSingleton().GetRenderDevicePtr();
	mSceneMan = Context::GetSingleton().GetSceneManagerPtr();
	if (mDevice == NULL || mSceneMan == NULL)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Create device and SceneManager first!", "Renderer::Init");
		return;
	}

	// Build light shape
	mPointLightShape = BuildPointLightShape();
	mFSQuadShape = BuildFSQuadShape();
	mSpotLightShape = BuildSpotLightShape();

	// Build shadow map
	mCascadedShadowMap = new CascadedShadowMap(mDevice);

}

void Renderer::CreatePrimitives()
{

}

void Renderer::SetRenderPipeline( const shared_ptr<Pipeline>& pipeline )
{
	mCurrPipeline = pipeline;
}

void Renderer::DrawRenderable( Camera* camera, const String& tech, const  String& matClass)
{

}

void Renderer::RenderScene()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	for (Pipeline::PipelineStage& stage : mCurrPipeline->mPipelineStages)
	{
		if( !stage.Enabled ) continue;

		for( const Pipeline::PipelineCommand* command : stage.Commands)
		{
			switch(command->Command)
			{
			case Pipeline::SwitchFrameBuffer:
				{
					int32_t fbIndex = command->Params[0].GetInt();
					shared_ptr<FrameBuffer> frameBuffer;

					if (fbIndex == -1) // default
						frameBuffer = device.GetScreenFrameBuffer();
					else
						frameBuffer = mCurrPipeline->GetFrameBuffer(fbIndex);
					
					device.BindFrameBuffer(frameBuffer);
				}
				break;

			case Pipeline::ClearTarget:
				{
					Attachment att = (Attachment)command->Params[0].GetInt();
					if (att == ATT_DepthStencil)
					{
						if (command->Params.size() == 2)
						{
							float depth = command->Params[1].GetFloat();
							device.GetCurrentFrameBuffer()->GetAttachedView(ATT_DepthStencil)->ClearDepth(depth);
						}
						else
						{
							float depth = command->Params[1].GetFloat();
							int32_t stencil = command->Params[2].GetInt();
							device.GetCurrentFrameBuffer()->GetAttachedView(ATT_DepthStencil)->ClearDepthStencil(depth, stencil);
						}	
					}
					else
					{
						const ColorRGBA& clearColor = command->Params[1].GetColor();
						device.GetCurrentFrameBuffer()->GetAttachedView(att)->ClearColor(clearColor);
					}
					
				}
				break;

			case Pipeline::DrawGeometry:
				{
					const String& techName = command->Params[0].GetString();
					//const String& matClass = command->Params[1].GetString();
					String matClass = "";

					RenderOrder order = RO_None;
					if (command->Params.size() > 2 )
						order = RenderOrder(command->Params[2].GetInt());

					DrawGeometry(techName, matClass, order);

				}
				break;

			case Pipeline::DrawLightShape:
				{
					int32_t matIdx = command->Params[0].GetInt();
					const String& techName = command->Params[1].GetString();

					// store for later use
					mCurrMaterial = mCurrPipeline->GetMaterial(matIdx);

					for (Pipeline::PipelineCommand* cmd : command->SubCommands)
						UpdateMaterialParameters(cmd);

					DrawLightShape(techName);

				}
				break;

			case Pipeline::DrawQuad:
				{
					int32_t matIdx = command->Params[0].GetInt();
					const String& techName = command->Params[1].GetString();

					// store for later use
					mCurrMaterial = mCurrPipeline->GetMaterial(matIdx);

					for (Pipeline::PipelineCommand* cmd : command->SubCommands)
						UpdateMaterialParameters(cmd);

					DrawFSQuad(techName);


					//const shared_ptr<Texture>& rt = mCurrPipeline->GetRenderTarget(2, 0);
					//device.GetRenderFactory()->SaveTexture2D("E:/DeferredShading.pfm", rt, 0, 0);
				}
				break;

			case Pipeline::DrawOverlays:
				{
					DrawOverlays();
				}
				break;
			}
		
		}
	}

	//device.GetScreenFrameBuffer()->SwapBuffers();
}

void Renderer::UpdateMaterialParameters( Pipeline::PipelineCommand* cmd )
{
	if (cmd->Command == Pipeline::BindBuffer)
	{
		const String& targetName = cmd->Params[0].GetString();
		int32_t pipeFrameBufferIdx = cmd->Params[1].GetInt();
		int32_t pipeTextureIndex = cmd->Params[2].GetInt();

		const shared_ptr<Texture>& rt = mCurrPipeline->GetRenderTarget(pipeFrameBufferIdx, pipeTextureIndex);
		mCurrMaterial->SetTexture(targetName, rt);
	}
	else if (cmd->Command == Pipeline::SetUniform)
	{
		
	}
}

void Renderer::DrawGeometry( const String& tech, const  String& matClass, RenderOrder order)
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	const Camera& camera = *(device.GetCurrentFrameBuffer()->GetCamera());	
	sceneMan.UpdateRenderQueue(camera, order);

	RenderBucket& opaquerBucket = sceneMan.GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	
	if (opaquerBucket.size())
	{
		for (const RenderQueueItem& renderItem : opaquerBucket) 
		{
			renderItem.Renderable->GetMaterial()->SetCurrentTechnique(tech);
			renderItem.Renderable->Render();
		}
	}
	//const shared_ptr<Texture>& rt0 = mCurrPipeline->GetRenderTarget(0, 1);
	//const shared_ptr<Texture>& rt1 = mCurrPipeline->GetRenderTarget(0, 2);
	//device.GetRenderFactory()->SaveTexture2D("E:/Normal.tga", rt0, 0, 0);
	//device.GetRenderFactory()->SaveTexture2D("E:/GBuffer1.tga", rt1, 0, 0);
}

void Renderer::DrawLightShape( const String& tech )
{
	// Fetch all lights affect current view frustum

	for (Light* light : mSceneMan->GetSceneLights())
	{
		switch (light->GetLightType())
		{
		case LT_DirectionalLight:
			DrawDirectionalLightShape(light, tech);
			break;
		case LT_PointLight:
			DrawPointLightShape(light, tech);
			break;
		case LT_SpotLight:
			DrawSpotLightShape(light, tech);
			break;
		default:
			break;
		}
	}

	//RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	//device.GetRenderFactory()->SaveTexture2D("E:/Light1.pfm", mCurrPipeline->GetRenderTarget(1, 1), 0, 0);
}

void Renderer::DrawDirectionalLightShape( Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());
	
	bool bCastShadow = light->GetCastShadow();
	
	if (bCastShadow)
	{
		// Generate shadow map
		mCascadedShadowMap->MakeCascadedShadowMap(*light);
	}

	const float3& lightColor = light->GetLightColor();
	mCurrMaterial->GetCustomParameter(EPU_Light_Color)->SetValue(lightColor);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mCurrMaterial->GetEffect()->GetParameterByName("CameraNearFar")->SetValue(camNearFar);

	const float3& worldDirection = light->GetDerivedDirection();
	float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
	lightDir = lightDir * currCamera.GetViewMatrix();

	mCurrMaterial->GetCustomParameter(EPU_Light_Dir)->SetValue(lightDir);

	shared_ptr<Effect> effect = mCurrMaterial->GetEffect();
	effect->GetParameterByName("ShadowEnabled")->SetValue(bCastShadow);
	if (bCastShadow)
	{	
		mCurrMaterial->SetTexture("ShadowTex", mCascadedShadowMap->mShadowTexture);
		effect->GetParameterByName("ShadowView")->SetValue(mCascadedShadowMap->mShadowView);
		effect->GetParameterByName("NumCascades")->SetValue((int)light->GetShadowCascades());
		effect->GetParameterByName("BorderPaddingMinMax")->SetValue(mCascadedShadowMap->mBorderPaddingMinMax);
		effect->GetParameterByName("CascadeScale")->SetValue(&(mCascadedShadowMap->mShadowCascadeScale[0]), MAX_CASCADES);
		effect->GetParameterByName("CascadeOffset")->SetValue(&(mCascadedShadowMap->mShadowCascadeOffset[0]), MAX_CASCADES); 
		//effect->GetParameterByName("CascadeBlendArea")->SetValue(mCascadedShadowMap->mCascadeBlendArea);
	}

	String techName = "Directional" + tech;
	DrawFSQuad(techName);
}

void Renderer::DrawPointLightShape( Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());

	float lightRadius = light->GetRange();

	const float3& lightColor = light->GetLightColor();
	mCurrMaterial->GetCustomParameter(EPU_Light_Color)->SetValue(lightColor);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mCurrMaterial->GetEffect()->GetParameterByName("CameraNearFar")->SetValue(camNearFar);

	const float3& worldPos = light->GetDerivedPosition();
	float4 lightPos(worldPos[0], worldPos[1], worldPos[2], 1.0f);
	lightPos = lightPos * currCamera.GetViewMatrix();
	mCurrMaterial->GetCustomParameter(EPU_Light_Position)->SetValue(lightPos);

	mCurrMaterial->GetCustomParameter(EPU_Light_Attenuation)->SetValue(light->GetAttenuation());

	float4x4 worldMatrix(lightRadius,  0.0f,		  0.0f,		    0.0f,
						 0.0f,         lightRadius,   0.0f,		    0.0f,
						 0.0f,         0.0f,          lightRadius,  0.0f,
						 worldPos.X(), worldPos.Y(),  worldPos.Z(), 1.0f);

	mCurrMaterial->ApplyMaterial(worldMatrix);
	mCurrMaterial->SetCurrentTechnique("Point" + tech);
	mDevice->Render(*mCurrMaterial->GetCurrentTechnique(), *mPointLightShape);
}

void Renderer::DrawSpotLightShape( Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());

	bool bCastShadow = light->GetCastShadow();
	if (bCastShadow)
	{
		// Generate shadow map
		mCascadedShadowMap->MakeCascadedShadowMap(*light);
	}

	const float3& lightColor = light->GetLightColor();
	mCurrMaterial->GetCustomParameter(EPU_Light_Color)->SetValue(lightColor);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mCurrMaterial->GetEffect()->GetParameterByName("CameraNearFar")->SetValue(camNearFar);

	const float3& worldPos = light->GetDerivedPosition();
	float4 lightPos(worldPos[0], worldPos[1], worldPos[2], 1.0f);
	lightPos = lightPos * currCamera.GetViewMatrix();

	const float3& worldDirection = light->GetDerivedDirection();
	float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
	lightDir = lightDir * currCamera.GetViewMatrix();

	float spotInnerAngle = light->GetSpotInnerAngle();
	float spotOuterAngle = light->GetSpotOuterAngle();
	lightPos[3] = cosf(spotInnerAngle);
	lightDir[3] = cosf(spotOuterAngle);
	mCurrMaterial->GetCustomParameter(EPU_Light_Dir)->SetValue(lightDir);
	mCurrMaterial->GetCustomParameter(EPU_Light_Position)->SetValue(lightPos);

	mCurrMaterial->GetCustomParameter(EPU_Light_Attenuation)->SetValue(light->GetAttenuation());

	float scaleHeight = light->GetRange();
	float scaleBase = scaleHeight * tanf(spotOuterAngle * 0.5f);

	float3 rotAxis = Cross(float3(0, 1, 0), worldDirection);
	//float rotAngle = acosf(Dot(float3(0, 1, 0), worldDirection));
	float4x4 rotation = CreateRotationAxis(rotAxis, acosf(worldDirection.Y()));

	float4x4 worldMatrix = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
		CreateTranslation(worldPos);

	String techName = "Spot" + tech;
	mCurrMaterial->ApplyMaterial(worldMatrix);
	mCurrMaterial->SetCurrentTechnique("Spot" + tech);
	mDevice->Render(*mCurrMaterial->GetCurrentTechnique(), *mSpotLightShape);
}

void Renderer::DrawFSQuad( const String& tech )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	mCurrMaterial->ApplyMaterial();
	mCurrMaterial->SetCurrentTechnique(tech);
	device.Render(*mCurrMaterial->GetCurrentTechnique(), *mFSQuadShape);
}

void Renderer::DrawOverlays()
{
	// Update overlays
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	UIManager& uiMan = UIManager::GetSingleton();

	uiMan.Render();
	sceneMan.UpdateOverlayQueue();

	RenderBucket& guiBucket = sceneMan.GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay);	
	if (guiBucket.size())
	{
		for (const RenderQueueItem& renderItem : guiBucket) 
			renderItem.Renderable->Render();
	}

}

void Renderer::UpdateCascadeShadowMap( Light* light )
{

}







}