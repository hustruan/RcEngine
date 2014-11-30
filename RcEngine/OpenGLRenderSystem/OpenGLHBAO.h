#ifndef OpenGLHBAO_h__
#define OpenGLHBAO_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/AmbientOcclusion.h>
#include <HBAO+/GFSDK_SSAO.h>

namespace RcEngine {

class _OpenGLExport OpenGLHBAOImpl : public AmbientOcclusion::HBAOImpl
{
public:
	OpenGLHBAOImpl(uint32_t aoWidth, uint32_t aoHeight);
	~OpenGLHBAOImpl();

	virtual void RenderSSAO(const AmbientOcclusionSettings& settings, 
		const Camera& viewCamera,
		const shared_ptr<FrameBuffer>& outAOFrameBuffer,
		const shared_ptr<Texture>& depthBuffer, 
		const shared_ptr<Texture>& normalBuffer);

private:
	GFSDK_SSAO_RenderMask         mRenderMask;
	GFSDK_SSAO_Context_GL*        mAOContext;
};







}


#endif // OpenGLHBAO_h__
