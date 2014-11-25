#ifndef D3D11SSAO_h__
#define D3D11SSAO_h__

#include "D3D11Prerequisites.h"
#include <Graphics/AmbientOcclusion.h>
#include <HBAO+/GFSDK_SSAO.h>

namespace RcEngine {

class _D3D11Export D3D11HBAOImpl : public AmbientOcclusion::HBAOImpl
{
public:
	D3D11HBAOImpl(uint32_t aoWidth, uint32_t aoHeight);
	~D3D11HBAOImpl();

	virtual void RenderSSAO(const AmbientOcclusionSettings& settings, 
		const Camera& viewCamera,
		const shared_ptr<FrameBuffer>& outAOFrameBuffer,
		const shared_ptr<Texture>& depthBuffer, 
		const shared_ptr<Texture>& normalBuffer);

private:
	D3D11_VIEWPORT mViewport;

	GFSDK_SSAO_RenderMask         mRenderMask;
	GFSDK_SSAO_Parameters_D3D11   mAOParams;
	GFSDK_SSAO_Context_D3D11*     mAOContext;
};


}




#endif // D3D11SSAO_h__
