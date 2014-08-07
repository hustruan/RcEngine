#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLBuffer::OpenGLBuffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags, GLenum target, ElementInitData* initData )
	: GraphicsBuffer(bufferSize, accessHint, flags),
	  mBufferOGL(0),
	  mBufferTarget(target)
{
	assert((GL_ARRAY_BUFFER == target)			||
		   (GL_ELEMENT_ARRAY_BUFFER == target)  ||
		   (GL_UNIFORM_BUFFER == target)		||
		   (GL_SHADER_STORAGE_BUFFER == target) ||
		   (GL_TEXTURE_BUFFER == target));

	glGenBuffers(1, &mBufferOGL);
	GLenum bufferUsage = OpenGLMapping::Mapping(accessHint);

	if (GLEW_EXT_direct_state_access)
	{
		glNamedBufferDataEXT(mBufferOGL, static_cast<GLsizeiptr>(bufferSize), initData ? initData->pData : nullptr, bufferUsage);
	}
	else
	{
		glBindBuffer(mBufferTarget, mBufferOGL);
		glBufferData(mBufferTarget, static_cast<GLsizeiptr>(bufferSize), initData ? initData->pData : nullptr, bufferUsage);
		glBindBuffer(mBufferTarget, 0);
	}
	//glNamedBufferStorageEXT()

	OGL_ERROR_CHECK();
}

OpenGLBuffer::~OpenGLBuffer( )
{
	if (mBufferOGL > 0)
		glDeleteBuffers(1, &mBufferOGL);
}

void* OpenGLBuffer::Map( uint32_t offset, uint32_t length, ResourceMapAccess mapType )
{
	void* pMapBuffer = NULL;
	
	if (length == MAP_ALL_BUFFER)
		length = mBufferSize - offset;
	else
	{
		// check range
		if (offset + length > mBufferSize)
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Out of range!", "OpenGLBuffer::Map");
	}
	
	glBindBuffer(mBufferTarget, mBufferOGL);
	pMapBuffer = glMapBufferRange(mBufferTarget, offset, length, OpenGLMapping::Mapping(mapType));
	glBindBuffer(mBufferTarget, 0);

	OGL_ERROR_CHECK();
	return pMapBuffer;
}

void OpenGLBuffer::UnMap()
{
	glBindBuffer(mBufferTarget, mBufferOGL);
	glUnmapBuffer(mBufferTarget);
	glBindBuffer(mBufferTarget, 0);
}

}