#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "D3D11Factory.h"
#include "D3D11Texture.h"
#include "D3D11RenderWindow.h"
#include "D3D11VertexDeclaration.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Shader.h"
#include <Graphics/RenderOperation.h>
#include <Graphics/Effect.h>
#include <MainApp/Application.h>
#include <Core/Exception.h>

namespace RcEngine {

D3D11Device* gD3D11Device = nullptr;

D3D11Device::D3D11Device( )
	: DeviceContextD3D11(nullptr),
	  DeviceD3D11(nullptr)
{
	gD3D11Device = this;
	mRenderFactory = new D3D11Factory();
}

D3D11Device::~D3D11Device()
{
	SAFE_RELEASE(DeviceD3D11);
	SAFE_RELEASE(DeviceContextD3D11);
}

void D3D11Device::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state )
{
	SamplerSlot samSlot(stage, unit);

	bool needUpdate = false;
	if (mCurrentSamplers.find(samSlot) != mCurrentSamplers.end())
	{
		if (mCurrentSamplers[samSlot] != state)
		{
			needUpdate = true;
			mCurrentSamplers[samSlot] = state;
		}
	}
	else
	{
		needUpdate = true;
		mCurrentSamplers[samSlot] = state;
	}

	if (needUpdate)
	{
		ID3D11SamplerState* samplerStateD3D11 = (static_cast<D3D11SamplerState*>(state.get()))->StateD3D11;
		switch (stage)
		{
		case ST_Vertex:
			DeviceContextD3D11->VSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Geomerty:
			DeviceContextD3D11->GSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Pixel:
			DeviceContextD3D11->PSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Compute:
			DeviceContextD3D11->CSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		default:
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid SamplerState", "D3D11RenderDevice::SetSamplerState");
			break;
		}
	}
}

void D3D11Device::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
{
	if (mCurrentBlendState != state || mCurrentBlendFactor != blendFactor || mCurrentSampleMask != sampleMask)
	{
		DeviceContextD3D11->OMSetBlendState( 
			(static_pointer_cast_checked<D3D11BlendState>(state))->StateD3D11,			
			blendFactor(), 
			sampleMask);

		mCurrentBlendState = state;
		mCurrentBlendFactor = blendFactor;
		mCurrentSampleMask = sampleMask;
	}
}

void D3D11Device::SetRasterizerState( const shared_ptr<RasterizerState>& state )
{
	if (mCurrentRasterizerState != state)
	{
		DeviceContextD3D11->RSSetState((static_cast<D3D11RasterizerState*>(state.get()))->StateD3D11);
		mCurrentRasterizerState = state;
	}
}

void D3D11Device::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef /*= 0*/, uint16_t backStencilRef /*= 0*/ )
{
	if (mCurrentDepthStencilState != state || frontStencilRef != mCurrentFrontStencilRef || mCurrentBackStencilRef != mCurrentBackStencilRef)
	{
		DeviceContextD3D11->OMSetDepthStencilState(
			(static_cast<D3D11DepthStencilState*>(state.get()))->StateD3D11,
			frontStencilRef);

		mCurrentDepthStencilState = state;
		mCurrentFrontStencilRef = frontStencilRef;
		mCurrentBackStencilRef = backStencilRef;
	}
}

void D3D11Device::SetViewports( const std::vector<Viewport>& vps )
{
	static CD3D11_VIEWPORT vpD3D11[4];

	size_t numViewports = vps.size();

	// Only check first one
	if (numViewports == 1)
	{
		if (vpD3D11[0].TopLeftX != vps[0].Left     || 
			vpD3D11[0].TopLeftY != vps[0].Top      || 
			vpD3D11[0].Width    != vps[0].Width    || 
			vpD3D11[0].Height   != vps[0].Height)
		{
			vpD3D11[0] = CD3D11_VIEWPORT(vps[0].Left, vps[0].Top, vps[0].Width, vps[0].Height);
			DeviceContextD3D11->RSSetViewports(numViewports, vpD3D11);
		}
	}
	else
	{
		assert(numViewports <= 4);
		for (size_t i = 0; i < vps.size(); ++i)
			vpD3D11[i] =  CD3D11_VIEWPORT(vps[i].Left, vps[i].Top, vps[i].Width, vps[i].Height);

		DeviceContextD3D11->RSSetViewports(numViewports, vpD3D11);
	}
}

void D3D11Device::CreateRenderWindow()
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	shared_ptr<D3D11RenderWindow> d3d11RenderWindow = std::make_shared<D3D11RenderWindow>(appSettings.Width, appSettings.Height);
	
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = d3d11RenderWindow->SwapChainD3D11->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	assert( SUCCEEDED(hr) );

	ID3D11RenderTargetView* pRenderTargetView = NULL;
	hr = gD3D11Device->DeviceD3D11->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
	pBackBuffer->Release();
	assert( SUCCEEDED(hr) );

	d3d11RenderWindow->AttachRTV(ATT_Color0, std::make_shared<D3D11RenderTargetView2D>(pRenderTargetView) );
	if(PixelFormatUtils::IsDepth(appSettings.DepthStencilFormat))
	{
		// Have depth buffer, attach it
		RenderFactory* factory = gD3D11Device->GetRenderFactory();
		shared_ptr<Texture> depthStencilTexture( new D3D11Texture2D(appSettings.DepthStencilFormat,
					appSettings.Width,  appSettings.Height,  appSettings.SampleCount, appSettings.SampleQuality));

		d3d11RenderWindow->AttachRTV(ATT_DepthStencil, factory->CreateDepthStencilView(depthStencilTexture, 0, 0));
	}

	d3d11RenderWindow->SetViewport(Viewport(0.f, 0.f, float(appSettings.Width), float(appSettings.Height)));
	BindFrameBuffer(d3d11RenderWindow);

	mScreenFrameBuffer = d3d11RenderWindow;
}

void D3D11Device::OnWindowResize( uint32_t width, uint32_t height )
{
	if (mScreenFrameBuffer)
	{
		const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

		shared_ptr<D3D11RenderWindow> d3d11RenderWindow = static_pointer_cast_checked<D3D11RenderWindow>(mScreenFrameBuffer);
		
		d3d11RenderWindow->DetachAll();

		DeviceContextD3D11->OMSetRenderTargets(0, 0, 0);

		HRESULT hr;
		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		hr = d3d11RenderWindow->SwapChainD3D11->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* pBackBuffer;
		hr = d3d11RenderWindow->SwapChainD3D11->GetBuffer(0, __uuidof( ID3D11Texture2D), (void**) &pBackBuffer );
		assert( SUCCEEDED(hr) );

		ID3D11RenderTargetView* pRenderTargetView = NULL;
		hr = gD3D11Device->DeviceD3D11->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
		pBackBuffer->Release();
		assert( SUCCEEDED(hr) );

		d3d11RenderWindow->AttachRTV(ATT_Color0, std::make_shared<D3D11RenderTargetView2D>(pRenderTargetView) );
		
		if(PixelFormatUtils::IsDepth(appSettings.DepthStencilFormat))
		{
			// Have depth buffer, attach it
			RenderFactory* factory = gD3D11Device->GetRenderFactory();
			shared_ptr<Texture> depthStencilTexture( new D3D11Texture2D(appSettings.DepthStencilFormat,
				appSettings.Width,  appSettings.Height,  appSettings.SampleCount, appSettings.SampleQuality));

			d3d11RenderWindow->AttachRTV(ATT_DepthStencil, factory->CreateDepthStencilView(depthStencilTexture, 0, 0));
		}

		d3d11RenderWindow->Resize(width, height);
		d3d11RenderWindow->SetViewport(Viewport(0, 0, float(width), float(height)));
		BindFrameBuffer(d3d11RenderWindow);
	}
}

void D3D11Device::ToggleFullscreen( bool fs )
{

}

void D3D11Device::AdjustProjectionMatrix( float4x4& pOut )
{

}

void D3D11Device::DoBindShaderPipeline( const shared_ptr<ShaderPipeline>& pipeline )
{
	shared_ptr<Shader> shaderStage;
	
	D3D11ShaderPipeline* pipelineD3D11 = static_cast_checked<D3D11ShaderPipeline*>(pipeline.get());

	if ( shaderStage = pipelineD3D11->GetShader(ST_Vertex) )
	{
		ID3D11VertexShader* shaderD3D11 = (static_cast<D3D11VertexShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->VSSetShader(shaderD3D11, nullptr, 0);
	}

	if ( shaderStage = pipelineD3D11->GetShader(ST_TessControl) )
	{
		ID3D11HullShader* shaderD3D11 = (static_cast<D3D11HullShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->HSSetShader(shaderD3D11, nullptr, 0);
	}

	if ( shaderStage = pipelineD3D11->GetShader(ST_TessEval) )
	{
		ID3D11DomainShader* shaderD3D11 = (static_cast<D3D11DomainShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->DSSetShader(shaderD3D11, nullptr, 0);
	}

	if ( shaderStage = pipelineD3D11->GetShader(ST_Geomerty) )
	{
		ID3D11GeometryShader* shaderD3D11 = (static_cast<D3D11GeometryShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->GSSetShader(shaderD3D11, nullptr, 0);
	}

	if ( shaderStage = pipelineD3D11->GetShader(ST_Pixel) )
	{
		ID3D11PixelShader* shaderD3D11 = (static_cast<D3D11PixelShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->PSSetShader(shaderD3D11, nullptr, 0);
	}

	if ( shaderStage = pipelineD3D11->GetShader(ST_Compute) )
	{
		ID3D11ComputeShader* shaderD3D11 = (static_cast<D3D11ComputeShader*>(shaderStage.get()))->ShaderD3D11;
		DeviceContextD3D11->CSSetShader(shaderD3D11, nullptr, 0);
	}
}

void D3D11Device::DoDraw( const EffectTechnique* technique, const RenderOperation& operation )
{
	// Set up input layout
	if (!operation.VertexDecl)
	{
		// Not use input layout
		DeviceContextD3D11->IASetInputLayout(nullptr);
	}
	else
	{
		D3D11VertexDeclaration* vertexDeclD3D11 = static_cast_checked<D3D11VertexDeclaration*>(operation.VertexDecl.get());
		if (vertexDeclD3D11->InputLayoutD3D11 == nullptr)
		{
			const Shader& vertexShader = *(technique->GetPassByIndex(0)->GetShaderPipeline()->GetShader(ST_Vertex));
			vertexDeclD3D11->CreateInputLayout(operation, vertexShader);
		}
		DeviceContextD3D11->IASetInputLayout(vertexDeclD3D11->InputLayoutD3D11);
	}
	
	// Set up vertex stream
	if (operation.VertexStreams.size())
	{
		static ID3D11Buffer* vertexStreamsD3D11[8];
		static uint32_t streamStride[8];
		static uint32_t streamOffset[8] = {0};

		for (size_t i = 0; i < operation.VertexStreams.size(); ++i)
		{
			vertexStreamsD3D11[i] = static_cast_checked<D3D11Buffer*>(operation.VertexStreams[i].get())->BufferD3D11;
			streamStride[i] = operation.VertexDecl->GetStreamStride(i);
		}

		DeviceContextD3D11->IASetVertexBuffers(0, operation.VertexStreams.size(), vertexStreamsD3D11, streamStride, streamOffset);
	}

	// Set draw primitive mode
	DeviceContextD3D11->IASetPrimitiveTopology( D3D11Mapping::Mapping(operation.PrimitiveType) );
	if (operation.IndexBuffer)
	{
		ID3D11Buffer* indexBufferD3D11 = static_cast_checked<D3D11Buffer*>(operation.IndexBuffer.get())->BufferD3D11;

		/**
		 * StartIndexLocation and Index Buffer Offset is almost the same thing. 
		 * The difference is that the offset is in bytes so you have to take the 
		 * index size (16 bit vs 32 bit) into account. 
		 */

		//uint32_t indexOffset;
		//DXGI_FORMAT indexFormatD3D11;
		//if(operation.IndexType == IBT_Bit16)
		//{
		//	indexFormatD3D11 = DXGI_FORMAT_R16_UINT;
		//	indexOffset += operation.IndexStart * 2;
		//}
		//else
		//{
		//	indexFormatD3D11 = DXGI_FORMAT_R32_UINT;
		//	indexOffset += operation.IndexStart * 4;
		//}
		//DeviceContextD3D11->IASetIndexBuffer(indexBufferD3D11, indexFormatD3D11, indexOffset);

		DXGI_FORMAT indexFormatD3D11 = (operation.IndexType == IBT_Bit16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		DeviceContextD3D11->IASetIndexBuffer(indexBufferD3D11, indexFormatD3D11, 0);

		for (EffectPass* pass : technique->GetPasses())
		{
			pass->BeginPass();

			if (operation.NumInstances <= 1)
			{
				DeviceContextD3D11->DrawIndexed(
					operation.IndexCount, 
					operation.IndexStart,
					operation.BaseVertex);
			}
			else
			{
				DeviceContextD3D11->DrawIndexedInstanced(
					operation.IndexCount, 
					operation.NumInstances,
					operation.IndexStart,
					operation.BaseVertex,
					0);
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
				DeviceContextD3D11->Draw(
					operation.VertexCount,
					operation.VertexStart);
			}
			else
			{
				DeviceContextD3D11->DrawInstanced(
					operation.VertexCount,
					operation.NumInstances,
					operation.VertexStart,
					0);
			}

			pass->EndPass();
		}
	}
}

void D3D11Device::DispatchCompute( const EffectTechnique* technique, uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCounZ )
{
   /**
    * No need frame buffer need for Compute Shader.
    *
    * From MSDN
    * If an overlapping resource view is already bound to an output slot,
    * such as a render target, then the method will fill the destination 
    * shader resource slot with NULL.
    */
	BindFrameBuffer(nullptr);

	static ID3D11UnorderedAccessView* NullUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT] = { nullptr };

	for (EffectPass* pass : technique->GetPasses())
	{
		pass->BeginPass();
		
		DeviceContextD3D11->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCounZ);
		
		// Hack: bind all Compute UAVs to null, because it may use as SRV in next pass.
		DeviceContextD3D11->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, NullUAVs, nullptr);
	
		pass->EndPass();
	}
}

}