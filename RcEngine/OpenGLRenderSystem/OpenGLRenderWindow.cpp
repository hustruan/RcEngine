#include "OpenGLRenderWindow.h"
#include "OpenGLDevice.h"
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <Core/Environment.h>

namespace RcEngine {

OpenGLRenderWindow::OpenGLRenderWindow( uint32_t width, uint32_t height )
	: OpenGLFrameBuffer(width, height, false)
{
	const ApplicationSettings& appSettings = Environment::GetSingleton().GetApplication()->GetAppSettings();

	mFullscreen = appSettings.Fullscreen;

	uint32_t colorDepth, depthBits, stencilBits;
	colorDepth = PixelFormatUtils::GetNumElemBytes(appSettings.ColorFormat) * 8;
	PixelFormatUtils::GetNumDepthStencilBits(appSettings.DepthStencilFormat, depthBits, stencilBits);

#ifdef RcWindows
	mHwnd = Window::msWindow->GetHwnd();
	mHdc = GetDC(mHwnd);

	// Todo: Add fullscreen support
	assert(mFullscreen == false);
	if (mFullscreen)												
	{
		DEVMODE dmScreenSettings;								
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	

		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);	
		dmScreenSettings.dmPelsWidth	= width;				
		dmScreenSettings.dmPelsHeight	= height;				
		dmScreenSettings.dmBitsPerPel	= colorDepth;					
		dmScreenSettings.dmFields       = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			assert(false);
		}

	}
	else
	{
		colorDepth = ::GetDeviceCaps(mHdc, BITSPIXEL);
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize		 = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	 = 1;
	pfd.dwFlags		 = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	 = PFD_TYPE_RGBA;
	pfd.cColorBits	 = static_cast<BYTE>(colorDepth);
	pfd.cDepthBits	 = static_cast<BYTE>(depthBits);
	pfd.cStencilBits = static_cast<BYTE>(stencilBits);
	pfd.iLayerType	 = PFD_MAIN_PLANE;

	GLuint pixelFormat = ::ChoosePixelFormat(mHdc,&pfd);
	assert(pixelFormat != 0);

	SetPixelFormat(mHdc,pixelFormat,&pfd);

	mHrc = wglCreateContext(mHdc);
	wglMakeCurrent(mHdc, mHrc);

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		String errMsg = reinterpret_cast<char const *>(glewGetErrorString(err));
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, errMsg, "OpenGLRenderDevice::InitGlew");
	}

	if (WGLEW_EXT_swap_control)
		wglSwapIntervalEXT(appSettings.SyncInterval);
#endif

	uint32_t sampleCount = appSettings.SampleCount;
	if(sampleCount > 1)
	{
		
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::ostringstream oss;
	oss << reinterpret_cast<char const *>(glGetString(GL_VENDOR)) << " "
		<< reinterpret_cast<char const *>(glGetString(GL_RENDERER)) << " " 
		<< reinterpret_cast<char const *>(glGetString(GL_VERSION));

	if (sampleCount > 1)
	{
		oss << " (" << sampleCount << "x AA)";
	}

	mDescription = oss.str();
}

OpenGLRenderWindow::~OpenGLRenderWindow()
{
	
}

void OpenGLRenderWindow::ToggleFullscreen( bool fs )
{

}

bool OpenGLRenderWindow::Fullscreen() const
{
	return false;
}

void OpenGLRenderWindow::SwapBuffers()
{
#ifdef RcWindows
	::SwapBuffers(mHdc);
#endif
}

}
