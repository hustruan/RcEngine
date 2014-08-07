#ifndef OpenGLBuffer_h__
#define OpenGLBuffer_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLBuffer : public GraphicsBuffer
{
public:
	OpenGLBuffer(uint32_t bufferSize, uint32_t accessHint, uint32_t flags, GLenum target, ElementInitData* initData);
	~OpenGLBuffer(void);

	inline GLuint GetBufferOGL() const		{ return mBufferOGL; }
	inline GLuint GetBufferTarget() const   { return mBufferTarget; }

	virtual void* Map(uint32_t offset, uint32_t length, ResourceMapAccess mapType);
	virtual void UnMap() ;

private:
	GLenum mBufferTarget;
	GLenum mBufferOGL;
};


}


#endif // OpenGLBuffer_h__
