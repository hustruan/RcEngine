#ifndef RenderView_h__
#define RenderView_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class _ApiExport RenderView
{
public: 
	RenderView();
	virtual ~RenderView();

	uint32_t GetWidth() const  { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	PixelFormat GetFormat() const  { return mFormat; }

	virtual void ClearColor(const ColorRGBA& clr) = 0;
	virtual void ClearDepth(float depth) = 0;
	virtual void ClearStencil(uint32_t stencil) = 0;
	virtual void ClearDepthStencil(float depth, uint32_t stencil) = 0;

	/**
	 * Called when a render view is attached to a frame buffer.
	 * Only called by frame buffer class;
	 */
	virtual void OnAttach(FrameBuffer& fb, Attachment attr) = 0;
	virtual void OnDetach(FrameBuffer& fb, Attachment attr) = 0;

protected:
	uint32_t mWidth, mHeight;
	PixelFormat mFormat;
};




} // Namespace RcEngine



#endif // RenderView_h__
