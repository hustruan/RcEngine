#ifndef RHFrameBuffer_h__
#define RHFrameBuffer_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

struct _ApiExport Viewport
{
	Viewport() {}
	Viewport(float left, float top, float width, float height)
		: Left(left), Top(top), Width(width), Height(height) {}
	
	friend bool operator != ( const Viewport& lhs, const Viewport& rhs )
	{
		return (lhs.Left != rhs.Left) || (lhs.Top != rhs.Top) || 
			   (lhs.Width != rhs.Width) || (lhs.Height != rhs.Height);
	}

	float Left, Top, Width, Height;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RenderView
{
public:
	friend class FrameBuffer;

public: 
	RenderView(const shared_ptr<Texture>& texture);
	virtual ~RenderView() {}
	
	virtual void ClearColor(const ColorRGBA& clr) = 0;
	virtual void ClearDepth(float depth) = 0;
	virtual void ClearStencil(uint32_t stencil) = 0;
	virtual void ClearDepthStencil(float depth, uint32_t stencil) = 0;

protected:
	/**
	 * Called when a render view is attached to a frame buffer.
	 * Only called by frame buffer class;
	 */
	virtual void OnAttach(FrameBuffer& fb, Attachment attr) = 0;
	virtual void OnDetach(FrameBuffer& fb, Attachment attr) = 0;

protected:
	shared_ptr<Texture> mTexture;  // Keep a reference of texture to keep it alive 
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height);
	virtual ~FrameBuffer();

	inline uint32_t GetWidth() const { return mWidth; }
	inline uint32_t GetHeight() const { return mHeight; }

	inline shared_ptr<Camera> GetCamera() const     { return mCamera; }
	void SetCamera(const shared_ptr<Camera>& cam)   { mCamera = cam; }

	void SetViewport(const Viewport& vp);
	void SetViewport(uint32_t index, const Viewport& vp); 
	inline const vector<Viewport>& GetViewports() const { return mViewports; }

	shared_ptr<RenderView> GetRTV(Attachment att) const;
	shared_ptr<UnorderedAccessView> GetUAV(uint32_t index) const;

	void AttachRTV(Attachment attchment, const shared_ptr<RenderView>& rtv);
	void AttachUAV(uint32_t index, const shared_ptr<UnorderedAccessView>& uav);
	
	void DetachRTV(Attachment att);
	void DetachUAV(uint32_t index);

	void DetachAll();

	void Resize(uint32_t width, uint32_t height);

	/**
	 * Clear all render target of the frame buffer.
	 * Note that before do clear, you need to bind the frame buffer as current device 
	 * frame buffer first, it is user's responsibility.
	 */
	void Clear(uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil);
	
	virtual void SwapBuffers() = 0;

protected:
	//Called when this frame buffer is binded
	virtual void OnBind() = 0;
	virtual void OnUnbind() = 0;

protected:

	uint32_t mWidth, mHeight;

	vector<Viewport> mViewports;

	vector<shared_ptr<RenderView> > mColorViews;
	shared_ptr<RenderView> mDepthStencilView;

	// Unordered access view, used in per-pixel list method, like OIT.
	vector<shared_ptr<UnorderedAccessView>> mUnorderedAccessViews;

	shared_ptr<Camera> mCamera;

	friend class RenderDevice;
};

}

#endif // RHFrameBuffer_h__
