#ifndef OpenGLRenderWindow_h__
#define OpenGLRenderWindow_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLFrameBuffer.h"

namespace RcEngine {

class _OpenGLExport OpenGLRenderWindow : public OpenGLFrameBuffer
{
public:
	OpenGLRenderWindow(uint32_t width, uint32_t height);
	~OpenGLRenderWindow();

	void ToggleFullscreen(bool fs);
	bool Fullscreen() const;
	void SwapBuffers();

private:
	bool mFullscreen;
	std::string mDescription;

#ifdef RcWindows
	HDC  mHdc;
	HGLRC mHrc;
	HWND mHwnd;
#endif
};

}

#endif // OpenGLRenderWindow_h__
