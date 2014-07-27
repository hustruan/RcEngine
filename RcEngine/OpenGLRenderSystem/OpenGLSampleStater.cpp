#include "OpenGLSamplerState.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine {

OpenGLSamplerState::OpenGLSamplerState( const SamplerStateDesc& desc )
	: SamplerState(desc),
	  mSamplerOGL(0)
{
	glGenSamplers(1, &mSamplerOGL);

	glSamplerParameteri(mSamplerOGL, GL_TEXTURE_WRAP_S, OpenGLMapping::Mapping(desc.AddressU));
	glSamplerParameteri(mSamplerOGL, GL_TEXTURE_WRAP_T, OpenGLMapping::Mapping(desc.AddressV));
	glSamplerParameteri(mSamplerOGL, GL_TEXTURE_WRAP_R, OpenGLMapping::Mapping(desc.AddressW));

	GLenum min, mag;
	OpenGLMapping::Mapping( min, mag, desc.Filter );
	glSamplerParameteri(mSamplerOGL, GL_TEXTURE_MAG_FILTER, mag);
	glSamplerParameteri(mSamplerOGL, GL_TEXTURE_MIN_FILTER, min);

	glSamplerParameterf(mSamplerOGL, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.MaxAnisotropy);

	glSamplerParameterf(mSamplerOGL, GL_TEXTURE_MIN_LOD, desc.MinLOD);
	glSamplerParameterf(mSamplerOGL, GL_TEXTURE_MAX_LOD, desc.MaxLOD);
	glSamplerParameterf(mSamplerOGL, GL_TEXTURE_LOD_BIAS , desc.MipLODBias);

	glSamplerParameterfv(mSamplerOGL, GL_TEXTURE_BORDER_COLOR, desc.BorderColor());

	if (desc.CompareSampler)
	{
		// Use GL_COMPARE_REF_TO_TEXTURE
		glSamplerParameteri(mSamplerOGL, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glSamplerParameteri(mSamplerOGL, GL_TEXTURE_COMPARE_FUNC, OpenGLMapping::Mapping(desc.ComparisonFunc));
	}

	OGL_ERROR_CHECK();
}

OpenGLSamplerState::~OpenGLSamplerState()
{
	if (mSamplerOGL)
	{
		glDeleteSamplers(1, &mSamplerOGL);
		mSamplerOGL = 0;
	}
}

}