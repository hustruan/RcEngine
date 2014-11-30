#include "D3D11HBAO.h"
#include "D3D11Device.h"
#include "D3D11View.h"
#include "D3D11FrameBuffer.h"
#include <Graphics/Camera.h>

#pragma comment(lib, "GFSDK_SSAO.win32.lib")

namespace RcEngine {

D3D11HBAOImpl::D3D11HBAOImpl(uint32_t aoWidth, uint32_t aoHeight)
	: mAOContext(nullptr),
	  mRenderMask(GFSDK_SSAO_RENDER_AO)
{
	GFSDK_SSAO_Status status;
	status = GFSDK_SSAO_CreateContext_D3D11(gD3D11Device->DeviceD3D11, &mAOContext);
	assert(status == GFSDK_SSAO_OK);

	mViewport.TopLeftX = mViewport.TopLeftY = 0.0f;
	mViewport.Width = (float)aoWidth;
	mViewport.Height = (float)aoHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
}

D3D11HBAOImpl::~D3D11HBAOImpl()
{
	if (mAOContext)
		mAOContext->Release();
}

void D3D11HBAOImpl::RenderSSAO(const AmbientOcclusionSettings& settings, const Camera& viewCamera, const shared_ptr<FrameBuffer>& outAOFrameBuffer, const shared_ptr<Texture>& depthBuffer, const shared_ptr<Texture>& normalBuffer)
{
	const float4x4& projMatrix = viewCamera.GetProjMatrix();

	mViewport.Width = static_cast<float>(depthBuffer->GetWidth());
	mViewport.Height = static_cast<float>(depthBuffer->GetHeight());

	mAOParams.Bias = settings.Bias;
	mAOParams.Radius = settings.Radius;
	mAOParams.CoarseAO = settings.CoarseAO;
	mAOParams.DetailAO = settings.DetailAO;

	shared_ptr<ShaderResourceView> depthSRV = depthBuffer->GetShaderResourceView();

	GFSDK_SSAO_InputData_D3D11 Input;

	Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	Input.DepthData.pFullResDepthTextureSRV = (static_pointer_cast_checked<D3D11ShaderResouceView>(depthSRV))->ShaderResourceViewD3D11;
	Input.DepthData.pViewport = &mViewport;
	Input.DepthData.pProjectionMatrix = (CONST FLOAT*)&projMatrix;
	Input.DepthData.ProjectionMatrixLayout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	Input.DepthData.MetersToViewSpaceUnits = settings.MetersToViewSpaceUnits;

	if (normalBuffer)
	{
		const float4x4& viewMatrix = viewCamera.GetViewMatrix();

		shared_ptr<ShaderResourceView> normalSRV = normalBuffer->GetShaderResourceView();

		Input.NormalData.Enable = TRUE;
		Input.NormalData.pFullResNormalTextureSRV = (static_pointer_cast_checked<D3D11ShaderResouceView>(normalSRV))->ShaderResourceViewD3D11;
		Input.NormalData.pWorldToViewMatrix = (CONST FLOAT*)&viewMatrix;
		Input.NormalData.WorldToViewMatrixLayout = GFSDK_SSAO_ROW_MAJOR_ORDER;
		Input.NormalData.DecodeScale = 2.f;
		Input.NormalData.DecodeBias = -1.f;
	}

	shared_ptr<RenderView> colorRTV = outAOFrameBuffer->GetRTV(ATT_Color0);
	ID3D11RenderTargetView* d3d11ColorRTV = (static_cast_checked<D3D11TargetView*>(colorRTV.get()))->RenderTargetViewD3D11;

	GFSDK_SSAO_Status status;
	status = mAOContext->RenderAO(gD3D11Device->DeviceContextD3D11, &Input, &mAOParams, d3d11ColorRTV, mRenderMask);
	assert(status == GFSDK_SSAO_OK);
}

}
