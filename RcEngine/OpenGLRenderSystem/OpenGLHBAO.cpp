#include "OpenGLHBAO.h"
#include "OpenGLTexture.h"
#include "OpenGLFrameBuffer.h"
#include <Graphics/Camera.h>

#pragma comment(lib, "GFSDK_SSAO.win32.lib")

namespace RcEngine {

OpenGLHBAOImpl::OpenGLHBAOImpl(uint32_t aoWidth, uint32_t aoHeight)
	: mAOContext(nullptr),
	  mRenderMask(GFSDK_SSAO_RENDER_AO) 
{
	GFSDK_SSAO_GLFunctions GL;
	GFSDK_SSAO_INIT_GL_FUNCTIONS(GL);

	GFSDK_SSAO_CustomHeap CustomHeap;
	CustomHeap.new_ = ::operator new;
	CustomHeap.delete_ = ::operator delete;

	GFSDK_SSAO_Status status;
	status = GFSDK_SSAO_CreateContext_GL(&mAOContext, &GL, &CustomHeap, GFSDK_SSAO_Version());
	assert(status == GFSDK_SSAO_OK);

	GFSDK_SSAO_Version Version = mAOContext->GetVersion();
	//LOGI("GL HBAO+ %d.%d.%d.%d", Version.Major, Version.Minor, Version.Branch, Version.Revision)
}

OpenGLHBAOImpl::~OpenGLHBAOImpl()
{
	if (mAOContext)
		mAOContext->Release();
}

void OpenGLHBAOImpl::RenderSSAO(const AmbientOcclusionSettings& settings, const Camera& viewCamera, const shared_ptr<FrameBuffer>& outAOFrameBuffer, const shared_ptr<Texture>& depthBuffer, const shared_ptr<Texture>& normalBuffer)
{
	GFSDK_SSAO_Parameters_GL AOParams;
	AOParams.Radius = settings.Radius;
	AOParams.Bias = settings.Bias;
	AOParams.DetailAO = settings.DetailAO;
	AOParams.CoarseAO = settings.CoarseAO;

	const float4x4& proj = viewCamera.GetProjMatrix();
	GLuint depthOGL = (static_cast_checked<OpenGLTexture*>(depthBuffer.get()))->GetTextureOGL();

	GFSDK_SSAO_InputData_GL Input;
	Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	Input.DepthData.FullResDepthTexture = GFSDK_SSAO_Texture_GL(GL_TEXTURE_2D, depthOGL);
	Input.DepthData.pProjectionMatrix = (const GFSDK_SSAO_FLOAT*)&proj;
	Input.DepthData.ProjectionMatrixLayout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	Input.DepthData.MetersToViewSpaceUnits = 0.005f;

	if (normalBuffer)
	{
		const float4x4& view = viewCamera.GetViewMatrix();
		GLuint normalOGL = (static_cast_checked<OpenGLTexture*>(normalBuffer.get()))->GetTextureOGL();

		Input.NormalData.Enable = true;
		Input.NormalData.FullResNormalTexture = GFSDK_SSAO_Texture_GL(GL_TEXTURE_2D, normalOGL);
		Input.NormalData.pWorldToViewMatrix = (const GFSDK_SSAO_FLOAT*)&view;
		Input.NormalData.WorldToViewMatrixLayout = GFSDK_SSAO_ROW_MAJOR_ORDER;
		Input.NormalData.DecodeScale = 2.f;
		Input.NormalData.DecodeBias = -1.f;
	}

	OpenGLFrameBuffer* aoFrameBufferOGL = static_cast_checked<OpenGLFrameBuffer*>(outAOFrameBuffer.get());

	GFSDK_SSAO_Status status;
	status = mAOContext->RenderAO(&Input, &AOParams, aoFrameBufferOGL->GetFrameBufferOGL(), mRenderMask);
	assert(status == GFSDK_SSAO_OK);
}

}
