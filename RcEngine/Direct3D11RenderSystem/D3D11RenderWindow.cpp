#include "D3D11RenderWindow.h"
#include "D3D11Device.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Factory.h"
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Exception.h>

namespace RcEngine {

D3D11RenderWindow::D3D11RenderWindow( uint32_t width, uint32_t height )
	: D3D11FrameBuffer(width, height),
	  SwapChainD3D11(nullptr),
	  FeatureLevelD3D11(D3D_FEATURE_LEVEL_11_0)
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	mSyncInterval = appSettings.SyncInterval;
	assert(DXGI_FORMAT_R8G8B8A8_UNORM == D3D11Mapping::Mapping(appSettings.ColorFormat));

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );

	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = appSettings.SampleCount;
	sd.SampleDesc.Quality = appSettings.SampleQuality;
	sd.Windowed = appSettings.Fullscreen ? FALSE : TRUE;
	sd.OutputWindow = Window::msWindow->GetHwnd();;

	HRESULT hr = S_OK;
	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, &sd, &SwapChainD3D11, &gD3D11Device->DeviceD3D11, &FeatureLevelD3D11, &gD3D11Device->DeviceContextD3D11 );
	
	if( FAILED( hr ) )
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unable to create d3d11 Device and SwapChain", "D3D11RenderWindow::D3D11RenderWindow");
	}
}

D3D11RenderWindow::~D3D11RenderWindow()
{
	 if( SwapChainD3D11 )
		 SwapChainD3D11->Release();
}

void D3D11RenderWindow::ToggleFullscreen( bool fs )
{

}

void D3D11RenderWindow::SwapBuffers()
{
	SwapChainD3D11->Present(mSyncInterval, 0);
}

}
