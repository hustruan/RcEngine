#ifndef FrameBuffer_h__
#define FrameBuffer_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

struct _ApiExport Viewport
{
public:
	Viewport() {}
	Viewport(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
		: Left(left), Top(top), Width(width), Height(height) {} 
	
	bool operator==( const Viewport& rhs ) const
	{
		return (Left == rhs.Left && Top == rhs.Top &&
			    Width == rhs.Width && Height == rhs.Height);
	}

	bool operator!=( const Viewport& rhs ) const
	{
		return !((*this) == rhs);
	}

	uint32_t Left, Top, Width, Height;
};

class _ApiExport FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
	virtual ~FrameBuffer(void);

	bool IsActice() const					{ return mActice; }
	void SetActice(bool state)				{ mActice = state; }

	uint32_t GetWidth() const				{ return mWidth; }
	uint32_t GetHeight() const				{ return mHeight; }
	
	void SetViewport(const Viewport& vp);
	const Viewport&	GetViewport() const		{ return mViewport; }

	PixelFormat GetColorFormat() const		{ return mColorFormat; }
	uint32_t GetColorDepth() const			{ return mColorDepth; }
	uint32_t GetDepthBits() const			{ return mDepthBits; }
	uint32_t GetStencilBits() const			{ return mStencilBits;}

	bool IsDepthBuffered() const			{ return mIsDepthBuffered; }
	bool IsDirty() const					{ return mDirty; }

	const shared_ptr<Camera>& GetCamera() const		{ return mCamera; }
	void SetCamera(const shared_ptr<Camera>& cam)	{ mCamera = cam; }

	shared_ptr<RenderView> GetAttachedView(Attachment att);

	/**
	 * Clear all render target of the frame buffer.
	 * note that before do clear, you need to bind the frame buffer as current device 
	 * frame buffer first, it is user's responsibity.
	 */
	void Clear(uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil);

	virtual void SwapBuffers() = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual bool CheckFramebufferStatus();
	
	void Attach(Attachment att, const shared_ptr<RHRenderView>& view);
	void Detach(Attachment att);
	void DetachAll();

	//Called when this frame buffer is binded
	void OnBind();
	void OnUnbind();

protected:
	virtual void DoBind() = 0;
	virtual void DoUnbind() = 0;

protected:
	bool mActice;

	uint32_t mWidth, mHeight;

	PixelFormat mColorFormat;
	bool mIsDepthBuffered;
	uint32_t mDepthBits, mStencilBits;
	uint32_t mColorDepth;

	vector<shared_ptr<RenderView> > mColorViews;
	shared_ptr<RenderView> mDepthStencilView;

	Viewport mViewport;
	shared_ptr<Camera> mCamera;

	bool mOffscreen;
	bool mDirty;
};


} // Namespace RcEngine

#endif // FrameBuffer_h__
