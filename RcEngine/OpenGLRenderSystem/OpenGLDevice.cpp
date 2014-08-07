#include "OpenGLDevice.h"
#include "OpenGLFactory.h"
#include "OpenGLBuffer.h"
#include "OpenGLRenderWindow.h"
#include "OpenGLSamplerState.h"
#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLVertexDeclaration.h"
#include "OpenGLShader.h"
#include <Graphics/RenderState.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Effect.h>
#include <MainApp/Application.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace RcEngine {

OpenGLDevice* gOpenGLDevice = NULL;

OpenGLDevice::OpenGLDevice()
	: mCurrentFBO(0)
{
	gOpenGLDevice = this;
	mBlitFBO[0] = mBlitFBO[1] = 0; 

	mRenderFactory = new OpenGLFactory();
}

OpenGLDevice::~OpenGLDevice(void)
{
	//SAFE_DELETE(mRenderFactory);

	if (mBlitFBO[0] != 0)
		glDeleteFramebuffers(2, mBlitFBO);
}

void OpenGLDevice::CreateRenderWindow()
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	mScreenFrameBuffer = std::make_shared<OpenGLRenderWindow>(appSettings.Width, appSettings.Height);

	mScreenFrameBuffer->AttachRTV(ATT_Color0, std::make_shared<OpenGLScreenRenderTargetView2D>());
	if(PixelFormatUtils::IsDepth(appSettings.DepthStencilFormat))
	{
		// Have depth buffer, attach it
		mScreenFrameBuffer->AttachRTV(ATT_DepthStencil, std::make_shared<OpenGLScreenDepthStencilView>(appSettings.DepthStencilFormat));
	}
	mScreenFrameBuffer->SetViewport(Viewport(0, 0, float(appSettings.Width), float(appSettings.Height)));
	
	// Bind as default
	BindFrameBuffer(mScreenFrameBuffer);
	
	//Create default render state
	mCurrentDepthStencilState = mRenderFactory->CreateDepthStencilState(DepthStencilStateDesc());
	mCurrentBlendState = mRenderFactory->CreateBlendState(BlendStateDesc());
	mCurrentRasterizerState = mRenderFactory->CreateRasterizerState(RasterizerStateDesc());

	// enable default render state
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);  // Disable
	glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

	/* glStencil*Separete does't work for back face on my PC.
	glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);*/

	glActiveStencilFaceEXT(GL_FRONT);
	glStencilFunc(GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glActiveStencilFaceEXT(GL_BACK);
	glStencilFunc(GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// do little hard code
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	OGL_ERROR_CHECK();
}

void OpenGLDevice::OnWindowResize( uint32_t width, uint32_t height )
{
	mScreenFrameBuffer->Resize(width, height);
	mScreenFrameBuffer->SetViewport(Viewport(0, 0, float(width), float(height)));
	BindFrameBuffer(mScreenFrameBuffer);
}

void OpenGLDevice::GetBlitFBO( GLuint& srcFBO, GLuint& dstFBO )
{
	// Create blit framebuffer
	if (mBlitFBO[0] == 0)
		glGenFramebuffers(2, mBlitFBO);

	srcFBO = mBlitFBO[0];
	dstFBO = mBlitFBO[1];
}

void OpenGLDevice::BindFBO( GLuint fbo )
{
	if (mCurrentFBO != fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		mCurrentFBO = fbo;
	}
}

void OpenGLDevice::AdjustProjectionMatrix( float4x4& pOut )
{
	//修改投影矩阵，使OpenGL适应左右坐标系
	float4x4 scale = CreateScaling(1.0f, 1.0f, 2.0f);
	float4x4 translate = CreateTranslation(0.0f, 0.0f, -1.0f);
	pOut =  pOut * scale * translate;
}

void OpenGLDevice::SetViewports( const std::vector<Viewport>& vps )
{
	size_t numViewports = vps.size();
	if (numViewports == 1)
	{
		if (mCurrentViewport != vps[0])
		{
			mCurrentViewport = vps[0];

			if (GLEW_ARB_viewport_array)
				glViewportIndexedf(0, vps[0].Left, vps[0].Top, vps[0].Width, vps[0].Height);
			else 
			{
				glViewport(
					static_cast<GLuint>(vps[0].Left),
					static_cast<GLuint>(vps[0].Top),
					static_cast<GLuint>(vps[0].Width),
					static_cast<GLuint>(vps[0].Height));
			}
		}
	}
	else 
	{
		assert(GLEW_ARB_viewport_array);
		glViewportArrayv(0, numViewports, &vps[0].Left);
	}

	OGL_ERROR_CHECK();
}


void OpenGLDevice::ToggleFullscreen( bool fs )
{

}


void OpenGLDevice::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
{
	OGL_ERROR_CHECK();

	if( mCurrentBlendState != state )
	{
		const BlendStateDesc& currDesc = mCurrentBlendState->GetDesc();
		const BlendStateDesc& stateDesc = state->GetDesc();

		if (currDesc.AlphaToCoverageEnable != stateDesc.AlphaToCoverageEnable)
		{
			if (stateDesc.AlphaToCoverageEnable)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}

		if( GLEW_EXT_draw_buffers2 ) 
		{
			for(int32_t i = 0; i < 8; i++)
			{
				if(currDesc.RenderTarget[i].BlendEnable != stateDesc.RenderTarget[i].BlendEnable)
				{
					if (stateDesc.RenderTarget[i].BlendEnable)
						glEnableIndexedEXT(GL_BLEND, i);
					else
						glDisableIndexedEXT(GL_BLEND, i);
				}

				if (currDesc.RenderTarget[i].ColorWriteMask != stateDesc.RenderTarget[i].ColorWriteMask)
				{
					glColorMaskIndexedEXT(i, (stateDesc.RenderTarget[i].ColorWriteMask & CWM_Red) != 0,
											 (stateDesc.RenderTarget[i].ColorWriteMask & CWM_Green) != 0,
										     (stateDesc.RenderTarget[i].ColorWriteMask & CWM_Blue) != 0,
											 (stateDesc.RenderTarget[i].ColorWriteMask & CWM_Alpha) != 0 );
				}

				if (currDesc.RenderTarget[i].BlendOp != stateDesc.RenderTarget[i].BlendOp)
				{
					glBlendEquationSeparatei(i, OpenGLMapping::Mapping(stateDesc.RenderTarget[i].BlendOp),
											    OpenGLMapping::Mapping(stateDesc.RenderTarget[i].BlendOpAlpha));
				}

				if ( (currDesc.RenderTarget[i].SrcBlend != stateDesc.RenderTarget[i].SrcBlend) || 
					 (currDesc.RenderTarget[i].DestBlend != stateDesc.RenderTarget[i].DestBlend)|| 
					 (currDesc.RenderTarget[i].SrcBlendAlpha != stateDesc.RenderTarget[i].SrcBlendAlpha) || 
					 (currDesc.RenderTarget[i].DestBlendAlpha != stateDesc.RenderTarget[i].DestBlendAlpha) )
				{
					glBlendFuncSeparatei(i, OpenGLMapping::Mapping(stateDesc.RenderTarget[i].SrcBlend), 
									        OpenGLMapping::Mapping(stateDesc.RenderTarget[i].DestBlend),
									        OpenGLMapping::Mapping(stateDesc.RenderTarget[i].SrcBlendAlpha), 
										    OpenGLMapping::Mapping(stateDesc.RenderTarget[i].DestBlendAlpha));
				}
			}
		}
		else
		{
			if (currDesc.RenderTarget[0].BlendEnable != stateDesc.RenderTarget[0].BlendEnable)
			{
				if (stateDesc.RenderTarget[0].BlendEnable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}

			if (currDesc.RenderTarget[0].ColorWriteMask != stateDesc.RenderTarget[0].ColorWriteMask)
			{
				glColorMask( (stateDesc.RenderTarget[0].ColorWriteMask & CWM_Red) != 0,
							 (stateDesc.RenderTarget[0].ColorWriteMask & CWM_Green) != 0,
							 (stateDesc.RenderTarget[0].ColorWriteMask & CWM_Blue) != 0,
							 (stateDesc.RenderTarget[0].ColorWriteMask & CWM_Alpha) != 0 );
			}

			if (currDesc.RenderTarget[0].BlendOp != stateDesc.RenderTarget[0].BlendOp)
			{
				glBlendEquationSeparate(OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOp),
					OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOpAlpha));
			}

			if ( (currDesc.RenderTarget[0].SrcBlend != stateDesc.RenderTarget[0].SrcBlend) || 
				 (currDesc.RenderTarget[0].DestBlend != stateDesc.RenderTarget[0].DestBlend) || 
				 (currDesc.RenderTarget[0].SrcBlendAlpha != stateDesc.RenderTarget[0].SrcBlendAlpha) || 
				 (currDesc.RenderTarget[0].DestBlendAlpha != stateDesc.RenderTarget[0].DestBlendAlpha) )
			{
				//auto src1 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlend);
				//auto dst1 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlend);
				//auto src11 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlendAlpha);
				//auto dst11 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlendAlpha);

				glBlendFuncSeparate(OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlend), 
								    OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlend),
									OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlendAlpha), 
									OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlendAlpha));
			}

		}	

		// Set Current
		mCurrentBlendState = state;	
	}

	if (mCurrentBlendFactor != blendFactor)
	{
		glBlendColor(blendFactor.R(), blendFactor.G(), blendFactor.B(), blendFactor.A());
		// Set Current
		mCurrentBlendFactor = blendFactor;
	}

	if(mCurrentSampleMask != sampleMask)
	{
		// Set Current
		mCurrentSampleMask = sampleMask;
	}

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetRasterizerState( const shared_ptr<RasterizerState>& state )
{
	OGL_ERROR_CHECK();

	if(mCurrentRasterizerState != state)
	{
		const RasterizerStateDesc& currDesc = mCurrentRasterizerState->GetDesc();
		const RasterizerStateDesc& stateDesc = state->GetDesc();

		if (currDesc.PolygonFillMode != stateDesc.PolygonFillMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, OpenGLMapping::Mapping(stateDesc.PolygonFillMode));
		}

		if(currDesc.PolygonCullMode != stateDesc.PolygonCullMode)
		{
			switch (stateDesc.PolygonCullMode)
			{
			case CM_None:
				glDisable(GL_CULL_FACE);
				break;

			case CM_Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case CM_Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			}
		}

		if(currDesc.FrontCounterClockwise != stateDesc.FrontCounterClockwise)
		{
			glFrontFace(stateDesc.FrontCounterClockwise ? GL_CW : GL_CCW);
		}

		if (currDesc.DepthBias != stateDesc.DepthBias || currDesc.SlopeScaledDepthBias != stateDesc.SlopeScaledDepthBias)
		{
			// Bias is in {0, 16}, scale the unit addition appropriately
			glPolygonOffset(stateDesc.SlopeScaledDepthBias, stateDesc.DepthBias);
		}

		if( currDesc.ScissorEnable != stateDesc.ScissorEnable )
		{
			if(stateDesc.ScissorEnable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}

		if( currDesc.MultisampleEnable != stateDesc.MultisampleEnable )
		{
			if(stateDesc.MultisampleEnable)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}

		if (currDesc.DepthClipEnable != stateDesc.DepthClipEnable)
		{
			/**
			 *  If enabled, the -wc≤zc≤wc plane equation is ignored by view volume clipping
			 * (effectively, there is no near or far plane clipping). 
			 */
			if (stateDesc.DepthClipEnable)
				glDisable(GL_DEPTH_CLAMP);
			else
				glEnable(GL_DEPTH_CLAMP);
		}


		//Set Current
		mCurrentRasterizerState = state;
	}

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef, uint16_t backStencilRef )
{
	OGL_ERROR_CHECK();

	if ( mCurrentDepthStencilState != state || mCurrentFrontStencilRef != frontStencilRef || mCurrentBackStencilRef != backStencilRef )
	{
		const DepthStencilStateDesc& currDesc = mCurrentDepthStencilState->GetDesc();
		const DepthStencilStateDesc& stateDesc = state->GetDesc();

		if(currDesc.DepthEnable != stateDesc.DepthEnable)
		{
			if (stateDesc.DepthEnable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		if (currDesc.DepthWriteMask != stateDesc.DepthWriteMask)
		{
			glDepthMask(stateDesc.DepthWriteMask);
		}

		if (currDesc.DepthFunc != stateDesc.DepthFunc)
		{
			glDepthFunc(OpenGLMapping::Mapping(stateDesc.DepthFunc));
		}

		if(currDesc.StencilEnable != stateDesc.StencilEnable)
		{
			if (stateDesc.StencilEnable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}

		if (currDesc.StencilWriteMask != stateDesc.StencilWriteMask)
		{
			glStencilMask(stateDesc.StencilWriteMask);
		}

		if (currDesc.FrontStencilFunc != stateDesc.FrontStencilFunc || 
			currDesc.StencilReadMask != stateDesc.StencilReadMask   || 
			mCurrentFrontStencilRef != frontStencilRef)
		{
			glActiveStencilFaceEXT(GL_FRONT);
			glStencilFunc(OpenGLMapping::Mapping(stateDesc.FrontStencilFunc), frontStencilRef, stateDesc.StencilReadMask);
		}

		if (currDesc.FrontStencilFailOp != stateDesc.FrontStencilFailOp || 
			currDesc.FrontStencilDepthFailOp != stateDesc.FrontStencilDepthFailOp || 
			currDesc.FrontStencilPassOp != stateDesc.FrontStencilPassOp)
		{
			glActiveStencilFaceEXT(GL_FRONT);
			glStencilOp(OpenGLMapping::Mapping(stateDesc.FrontStencilFailOp),
					    OpenGLMapping::Mapping(stateDesc.FrontStencilDepthFailOp),
						OpenGLMapping::Mapping(stateDesc.FrontStencilPassOp));
		}

		if (currDesc.BackStencilFunc != stateDesc.BackStencilFunc ||
			currDesc.StencilReadMask != stateDesc.StencilReadMask ||
			mCurrentBackStencilRef != backStencilRef)
		{
			glActiveStencilFaceEXT(GL_BACK);
			glStencilFunc(OpenGLMapping::Mapping(stateDesc.BackStencilFunc), backStencilRef, stateDesc.StencilReadMask);
		}

		if (currDesc.BackStencilFailOp != stateDesc.BackStencilFailOp ||
			currDesc.BackStencilDepthFailOp != stateDesc.BackStencilDepthFailOp ||
			currDesc.BackStencilPassOp != stateDesc.BackStencilPassOp)
		{
			glActiveStencilFaceEXT(GL_BACK);
			glStencilOp(OpenGLMapping::Mapping(stateDesc.BackStencilFailOp),
				        OpenGLMapping::Mapping(stateDesc.BackStencilDepthFailOp),
						OpenGLMapping::Mapping(stateDesc.BackStencilPassOp));
		}
	}

	mCurrentDepthStencilState = state;
	mCurrentFrontStencilRef = frontStencilRef;
	mCurrentBackStencilRef = backStencilRef;

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state )
{
	if (mCurrentSamplerStates[unit] != state)
	{
		mCurrentSamplerStates[unit] = state;

		OpenGLSamplerState* pSamplerState = static_cast_checked<OpenGLSamplerState*>(state.get());
		glBindSampler(unit, pSamplerState->GetSamplerOGL());

		OGL_ERROR_CHECK();
	}
}

void OpenGLDevice::DoBindShaderPipeline( const shared_ptr<ShaderPipeline>& pipeline )
{
	GLuint pipelineOGL = static_cast_checked<OpenGLShaderPipeline*>(pipeline.get())->GetPipelineOGL();
	glBindProgramPipeline(pipelineOGL);
}

void OpenGLDevice::DoDraw( const EffectTechnique* technique, const RenderOperation& operation )
{
	if (!operation.VertexDecl)
	{
		// Not use VertexBuffer, like FullScreenTrangle
		glBindVertexArray(0);
	}
	else
	{
		// Bind vertex buffer
		OpenGLVertexDeclaration* vertexDeclOGL = static_cast_checked<OpenGLVertexDeclaration*>(operation.VertexDecl.get());
		if (vertexDeclOGL->GetVertexArrayOGL() == 0)
		{
			// Get vertex shader of first pass, may need it to create VAO
			const Shader& vertexShader = *(technique->GetPassByIndex(0)->GetShaderPipeline()->GetShader(ST_Vertex));
			vertexDeclOGL->CreateVertexArrayOGL(operation, vertexShader);
		}
		glBindVertexArray(vertexDeclOGL->GetVertexArrayOGL());
	}

	// Bind index buffer
	if (operation.IndexBuffer)
	{
		OpenGLBuffer* bufferOGL = static_cast_checked<OpenGLBuffer*>(operation.IndexBuffer.get());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferOGL->GetBufferOGL());
	}

	// Draw primitive
	GLenum primitiveTypeOGL = OpenGLMapping::Mapping(operation.PrimitiveType);
	
	// Tessellation
	if (primitiveTypeOGL == GL_PATCHES)
	{
		assert(PT_Patch_Control_Point_1 <= operation.PrimitiveType && operation.PrimitiveType <= PT_Patch_Control_Point_32);
		GLint numControlPoints = operation.PrimitiveType - PT_Patch_Control_Point_1 + 1;
		glPatchParameteri(GL_PATCH_VERTICES, numControlPoints);
	}

	if (operation.IndexBuffer)
	{
		GLenum indexTypeOGL;
		uint32_t indexOffset;

		if(operation.IndexType == IBT_Bit16)
		{
			indexTypeOGL = GL_UNSIGNED_SHORT;
			indexOffset = operation.IndexStart * 2;
		}
		else
		{
			indexTypeOGL = GL_UNSIGNED_INT;
			indexOffset = operation.IndexStart * 4;
		}

		for (EffectPass* pass : technique->GetPasses())
		{
			pass->BeginPass();

			if (operation.NumInstances <= 1)
			{
				glDrawElementsBaseVertex(
					primitiveTypeOGL,
					operation.IndexCount, 
					indexTypeOGL,
					BUFFER_OFFSET(indexOffset),
					operation.BaseVertex);
			}
			else
			{
				glDrawElementsInstancedBaseVertex(
					primitiveTypeOGL, 
					operation.IndexCount,
					indexTypeOGL, 
					BUFFER_OFFSET(indexOffset),
					operation.NumInstances,
					operation.BaseVertex);
			}

			pass->EndPass();
		}
	}
	else
	{
		for (EffectPass* pass : technique->GetPasses())
		{
			pass->BeginPass();

			if (operation.NumInstances <= 1)
			{
				glDrawArrays(primitiveTypeOGL, operation.VertexStart, operation.VertexCount);
			}
			else
			{
				glDrawArraysInstanced(
					primitiveTypeOGL,
					operation.VertexStart,
					operation.VertexCount,
					operation.NumInstances);
			}
	
			pass->EndPass();
		}
	}
}

void OpenGLDevice::DispatchCompute( const EffectTechnique* technique, uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCounZ )
{
	for (EffectPass* pass : technique->GetPasses())
	{
		pass->BeginPass();
		glDispatchCompute(threadGroupCountX, threadGroupCountY, threadGroupCounZ);
		pass->EndPass();
	}
}

}
