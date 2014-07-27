#ifndef AppSettings_h__
#define AppSettings_h__

#include <Graphics/PixelFormat.h>

namespace RcEngine {

enum RenderDeviceType
{
	RD_OpenGL = 0,
	RD_OpenGL_ES,
	RD_Direct3D11,
	RD_Count
};

struct ApplicationSettings
{
	String AppTitle;
	
	int32_t Left, Top;
	uint32_t Width, Height;
	
	bool Fullscreen;

	RenderDeviceType RHDeviceType;

	PixelFormat ColorFormat;
	PixelFormat DepthStencilFormat;

	uint32_t SyncInterval;
	uint32_t SampleCount, SampleQuality;
};

} // Namespace RcEngine


#endif // AppSettings_h__
