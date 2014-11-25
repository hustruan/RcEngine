#ifndef OpenGLFrameBuffer_h__
#define OpenGLFrameBuffer_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/FrameBuffer.h>

namespace RcEngine {

class _OpenGLExport OpenGLFrameBuffer : public FrameBuffer
{
public:
	OpenGLFrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
	virtual ~OpenGLFrameBuffer();

	inline GLuint GetFrameBufferOGL() const { return mFrameBufferOGL; }

	virtual void OnBind();
	virtual void OnUnbind();

	virtual void SwapBuffers();
	virtual bool CheckFramebufferStatus();

protected:
	GLuint mFrameBufferOGL;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLRenderView : public RenderView
{
public:
	OpenGLRenderView(const shared_ptr<Texture>& texture);
	virtual ~OpenGLRenderView();
	
	virtual void ClearColor(const ColorRGBA& clr);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void ClearDepthStencil(float depth, uint32_t stencil);	

protected:
	virtual void OnAttach(FrameBuffer& fb, Attachment attr);
	virtual void OnDetach(FrameBuffer& fb, Attachment attr);

	void DoClear(GLbitfield flag, const ColorRGBA& clr, float depth, uint32_t stencil);

protected:
	Attachment mAttachment;
	GLuint mFrameBufferOGL;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLDepthStencilView : public OpenGLRenderView
{
public:
	OpenGLDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level);

	void ClearDepth(float depth);
	void ClearStencil(uint32_t stencil);
	void ClearDepthStencil(float depth, uint32_t stencil);

protected:
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

private:
	uint32_t mArrIndex;
	uint32_t mLevel;
};

class _OpenGLExport OpenGLScreenDepthStencilView : public OpenGLRenderView
{
public:
	OpenGLScreenDepthStencilView(PixelFormat fmt);

	void ClearDepth(float depth);
	void ClearStencil(uint32_t stencil);
	void ClearDepthStencil(float depth, uint32_t stencil);

protected:
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

private:
	PixelFormat mFormat;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLRenderTargetView2D : public OpenGLRenderView
{
public:
	OpenGLRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrIndex, uint32_t level);

	void ClearColor(const ColorRGBA& clr);

protected:
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

private:
	uint32_t mArrIndex;
	uint32_t mLevel;
};

class _OpenGLExport OpenGLRenderTargetArrayView : public OpenGLRenderView
{
public:
	OpenGLRenderTargetArrayView(const shared_ptr<Texture>& texture,  uint32_t level);

protected:
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

protected:
	uint32_t mLevel;
};
//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLScreenRenderTargetView2D :  public OpenGLRenderView
{
public:
	OpenGLScreenRenderTargetView2D();

	void ClearColor(const ColorRGBA& clr);

protected:
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);
};

}



#endif // OpenGLFrameBuffer_h__
