#include <Graphics/Pipeline.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderView.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Texture.h>
#include <Graphics/Material.h>
#include <Resource/ResourceManager.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Core/Context.h>
#include <Core/Exception.h>

namespace {

using namespace RcEngine;

Attachment GetAttachment(const String& name)
{
	if (name == "Depth")
	{
		return ATT_DepthStencil;
	}
	else if (name == "Color0")
	{
		return ATT_Color0;
	}
	else if (name == "Color1")
	{
		return ATT_Color1;
	}
	else if (name == "Color2")
	{
		return ATT_Color2;
	}
	else if (name == "Color3")
	{
		return ATT_Color3;
	}
	else if (name == "Color4")
	{
		return ATT_Color4;
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Attachment Error.", "GetAttachment");
	}
}

Pipeline::CommandFlag GetCommandFlag(const String& name) 
{
	if (name == "SwitchFrameBuffer")
		return Pipeline::SwitchFrameBuffer;
	else if (name == "ClearTarget")
		return Pipeline::ClearTarget;
	else if (name == "DrawGeometry")
		return Pipeline::DrawGeometry;
	else if (name == "BindBuffer")
		return Pipeline::BindBuffer;
	else if (name == "DrawQuad")
		return Pipeline::DrawQuad;
	else if (name == "DrawLightShape")
		return Pipeline::DrawLightShape;
	else if (name == "DrawOverlays")
		return Pipeline::DrawOverlays;
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Attachment Error.", "GetAttachment");
	}
}

}


namespace RcEngine {

shared_ptr<Resource> Pipeline::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Pipeline>(creator, handle, name, group);
}

Pipeline::Pipeline(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Pipeline, creator, handle, name, group)
{

}

Pipeline::~Pipeline()
{

}

void Pipeline::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	Window* appWindow = Context::GetSingleton().GetApplication().GetMainWindow();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	shared_ptr<Stream> matStream = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *matStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	mPipelineName = root->AttributeString("name", "");

	XMLNodePtr setupNode = root->FirstNode("Setup");
	ParseSetup(setupNode);

	// Create frame buffer and render targets
	CreateFrameBuffers();

	XMLNodePtr cmdQueueNode = root->FirstNode("CommandQueue"); 
	ParseStage(cmdQueueNode);	
}

void Pipeline::UnloadImpl()
{

}

void Pipeline::CreateFrameBuffers()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		PipeFrameBuffer& fbDesc = mFrameBuffers[i];

		fbDesc.FrameBuffer = factory.CreateFrameBuffer(fbDesc.Width, fbDesc.Height);
		
		// bind as current, so we can add render target
		device.BindFrameBuffer(fbDesc.FrameBuffer);
		for (size_t j = 0; j < fbDesc.RenderTargets.size(); ++j)
		{
			RenderTarget& rtDesc = fbDesc.RenderTargets[j];

			shared_ptr<RenderView> view;

			uint32_t accessHint = EAH_CPU_Read | EAH_GPU_Write | EAH_GPU_Read;

			if (rtDesc.Attach == ATT_DepthStencil)
			{
				rtDesc.RenderTexture = factory.CreateTexture2D(fbDesc.Width, fbDesc.Height, rtDesc.Format, 1, 1, rtDesc.SampleCount, rtDesc.SampleQuality, accessHint, nullptr);
				view = factory.CreateDepthStencilView(rtDesc.RenderTexture, 0, 0);
			}
			else
			{
				rtDesc.RenderTexture = factory.CreateTexture2D(fbDesc.Width, fbDesc.Height, rtDesc.Format, 1, 1, rtDesc.SampleCount, rtDesc.SampleQuality, accessHint, nullptr);
				view = factory.CreateRenderTargetView2D(rtDesc.RenderTexture, 0, 0);
			}

			fbDesc.FrameBuffer->Attach(rtDesc.Attach, view);
		}

		// set default camera to game camera
		fbDesc.FrameBuffer->SetCamera( device.GetScreenFrameBuffer()->GetCamera() );
	}
	
	// bind default screen buffer
	device.BindFrameBuffer(device.GetScreenFrameBuffer());
}

int32_t Pipeline::GetFrameBufferIndex( const String& name ) const
{
	if (name == "Screen" || name == "screen")
		return -1;  // for default screen frame buffer

	for (int32_t i = 0; i < (int32_t)mFrameBuffers.size(); ++i)
	{
		if (mFrameBuffers[i].Name == name)
		{
			return i;
		}
	}

	ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "FrameBuffer" + name + " not exits", "Pipeline::GetFrameBufferIndex");
	return -1;
}

int32_t Pipeline::GetRenderTargetIndex( const String& fbName, Attachment attach ) const
{
	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		if (mFrameBuffers[i].Name == fbName)
		{
			for (int32_t j = 0; j < (int32_t)mFrameBuffers[i].RenderTargets.size(); ++j)
			{
				if (mFrameBuffers[i].RenderTargets[j].Attach == attach)
				{
					return j;
				}
			}
		}
	}

	ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "RenderTexture not exits", "Pipeline::GetRenderTextureIndex");
	return -1;
}

int32_t Pipeline::LoadMaterial( const String& resName, const String& resGroup )
{
	for (int32_t i = 0; i < (int32_t)mMaterials.size(); ++i)
	{
		if (mMaterials[i]->GetResourceName() == resName && mMaterials[i]->GetResourceGroup() == resGroup)
			return i;
	}

	// if not exits, load new
	ResourceManager& resMan = ResourceManager::GetSingleton();
	shared_ptr<Material> material = std::static_pointer_cast<Material>(
		resMan.GetResourceByName(RT_Material, resName, resGroup) ) ;
	mMaterials.push_back(material);

	return (int32_t)mMaterials.size() - 1;
}

void Pipeline::ParseSetup( const XMLNodePtr& setupNode )
{
	if (setupNode)
	{
		Window* appWindow = Context::GetSingleton().GetApplication().GetMainWindow();

		for (XMLNodePtr frameBufferNode = setupNode->FirstNode("FrameBuffer"); frameBufferNode; frameBufferNode = frameBufferNode->NextSibling("FrameBuffer"))
		{
			PipeFrameBuffer fbDesc;

			float sizeScale = frameBufferNode->AttributeFloat("scale", 1.0);

			fbDesc.Name =  frameBufferNode->AttributeString("name", "");
			fbDesc.Width = static_cast<uint32_t>(appWindow->GetWidth() * sizeScale);
			fbDesc.Height = static_cast<uint32_t>(appWindow->GetHeight() * sizeScale);

			for (XMLNodePtr targetNode = frameBufferNode->FirstNode("Target"); targetNode; targetNode = targetNode->NextSibling("Target"))
			{
				RenderTarget rtDesc; 

				String attachment = targetNode->AttributeString("attach", "");
				String targetFormat = targetNode->AttributeString("format", "");	

				rtDesc.Attach = GetAttachment(attachment);
				rtDesc.Format = PixelFormatUtils::GetPixelFormat(targetFormat);
				rtDesc.SampleCount = targetNode->AttributeUInt("sampleCount", 1);
				rtDesc.SampleQuality = targetNode->AttributeUInt("sampleQuality", 0);

				fbDesc.RenderTargets.push_back(rtDesc);	
			}

			mFrameBuffers.push_back(fbDesc);
		}
	}
}

void Pipeline::ParseStage( const XMLNodePtr& node )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	for (XMLNodePtr stageNode = node->FirstNode("Stage"); stageNode; stageNode = stageNode->NextSibling("Stage"))
	{
		// add new stage
		mPipelineStages.resize(mPipelineStages.size() + 1);

		PipelineStage& stage = mPipelineStages.back();

		stage.Name = stageNode->AttributeString("name", "");
		stage.Enabled = true;

		for (XMLNodePtr cmdNode = stageNode->FirstNode(); cmdNode; cmdNode = cmdNode->NextSibling())
		{
			CommandFlag cmdFlag = GetCommandFlag(cmdNode->NodeName());

			switch (cmdFlag)
			{
			case SwitchFrameBuffer:
				{
					String frameBufferName = cmdNode->AttributeString("name", "");
					int32_t index = GetFrameBufferIndex(frameBufferName);

					PipelineCommand* cmd = new PipelineCommand(SwitchFrameBuffer);

					PipelineCmdParam fbIndexParam;
					fbIndexParam.SetInt(index);
					cmd->Params.push_back(fbIndexParam);

					stage.Commands.push_back(cmd);
				}
				break;
			case ClearTarget:
				{
					PipelineCommand* cmd = new PipelineCommand(ClearTarget);
					Attachment attach = GetAttachment(cmdNode->AttributeString("attach", ""));
					if (attach == ATT_DepthStencil)
					{
						float depth = cmdNode->AttributeFloat("depth", 1.0f);

						PipelineCmdParam attachParam;
						attachParam.SetInt((int32_t)ATT_DepthStencil);
						cmd->Params.push_back(attachParam);

						PipelineCmdParam depthParam;
						depthParam.SetFloat(depth);
						cmd->Params.push_back(depthParam);

						XMLAttributePtr stencil = cmdNode->Attribute("stencil");
						if (stencil)
						{
							PipelineCmdParam stencilParam;
							stencilParam.SetInt(cmdNode->AttributeInt("stencil", 0));
							cmd->Params.push_back(stencilParam);
						}
					}
					else
					{
						float r = cmdNode->AttributeFloat("r", 0.0f);
						float g = cmdNode->AttributeFloat("g", 0.0f);
						float b = cmdNode->AttributeFloat("b", 0.0f);
						float a = cmdNode->AttributeFloat("a", 0.0f);

						PipelineCmdParam attachParam;
						attachParam.SetInt((int32_t)attach);
						cmd->Params.push_back(attachParam);

						PipelineCmdParam colorParam;
						colorParam.SetColor(ColorRGBA(r, g, b, a));		
						cmd->Params.push_back(colorParam);						
					}

					stage.Commands.push_back(cmd);
				}
				break;
			case DrawOverlays:
				{
					PipelineCommand* cmd = new PipelineCommand(DrawOverlays);
					stage.Commands.push_back(cmd);
				}
				break;
			case DrawGeometry:
			case DrawQuad:
			case DrawLightShape:
				{
					PipelineCommand* cmd = new PipelineCommand(cmdFlag);

					if (cmdNode->Attribute("material"))
					{		
						String materialName = cmdNode->AttributeString("material", "");
						int32_t materialIdx = LoadMaterial(materialName, "General");
						
						// Material index
						PipelineCmdParam materialParam;
						materialParam.SetInt(materialIdx);
						cmd->Params.push_back(materialParam);
					}

					PipelineCmdParam techParam;
					techParam.SetString( cmdNode->AttributeString("technique", "") );
					cmd->Params.push_back(techParam);
	
					for (XMLNodePtr node = cmdNode->FirstNode(); node; node = node->NextSibling())
					{
						String name = node->NodeName();
						if (name == "BindBuffer")
						{
							String fbName = node->AttributeString("source", "");
							Attachment attach = GetAttachment(node->AttributeString("attach", ""));
							String targetName = node->AttributeString("name", "");

							PipelineCommand* subCmd = new PipelineCommand(BindBuffer);
							
							PipelineCmdParam targetParam;
							targetParam.SetString(targetName);
							subCmd->Params.push_back(targetParam);

							PipelineCmdParam fbIdxParam;
							fbIdxParam.SetInt(GetFrameBufferIndex(fbName));
							subCmd->Params.push_back(fbIdxParam);

							PipelineCmdParam rtIdxParam;
							rtIdxParam.SetInt(GetRenderTargetIndex(fbName, attach));
							subCmd->Params.push_back(rtIdxParam);

							cmd->SubCommands.push_back(subCmd);
						}
						else if (name == "SetUniform")
						{

						}
					}

					stage.Commands.push_back(cmd);
				}
				break;

			default:
				break;
			}
		}
	}

}

void Pipeline::Resize( uint32_t width, uint32_t height )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory = *device.GetRenderFactory();

	for (PipeFrameBuffer& fb : mFrameBuffers)
	{
		device.BindFrameBuffer(fb.FrameBuffer);

		fb.FrameBuffer->DetachAll();
		fb.FrameBuffer->Resize(width, height);

		for (RenderTarget& rt : fb.RenderTargets)
		{
			PixelFormat fmt = rt.RenderTexture->GetTextureFormat();
			uint32_t arrSize = rt.RenderTexture->GetTextureArraySize();
			uint32_t numLevels = rt.RenderTexture->GetMipLevels();
			uint32_t sampleCount = rt.RenderTexture->GetSampleCount();
			uint32_t sampleQuality = rt.RenderTexture->GetSampleQuality();

			rt.RenderTexture = factory.CreateTexture2D(width, height, fmt, arrSize, numLevels, sampleCount, sampleQuality, 0, nullptr);
			
			if (rt.Attach == ATT_DepthStencil)
				fb.FrameBuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(rt.RenderTexture, 0, 0));
			else		
				fb.FrameBuffer->Attach(rt.Attach, factory.CreateRenderTargetView2D(rt.RenderTexture, 0, 0));
		}
	}

	device.BindFrameBuffer(device.GetScreenFrameBuffer());
}



}
