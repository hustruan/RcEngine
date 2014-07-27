#ifndef OpenGLSamplerState_h__
#define OpenGLSamplerState_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RenderState.h>

namespace RcEngine {

class _OpenGLExport OpenGLSamplerState : public SamplerState
{
public:
	OpenGLSamplerState(const SamplerStateDesc& desc);
	~OpenGLSamplerState();

	inline GLuint GetSamplerOGL() const { return mSamplerOGL; }

private:
	GLuint mSamplerOGL;
};

}

#endif // OpenGLSamplerState_h__
