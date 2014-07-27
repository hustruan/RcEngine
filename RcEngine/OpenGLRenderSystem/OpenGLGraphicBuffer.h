#pragma once
#ifndef OpenGLGraphicBuffer_h__
#define OpenGLGraphicBuffer_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/GraphicBuffer.h>

namespace RcEngine {

class _OpenGLExport OpenGLGraphicsBuffer : public GraphicsBuffer
{
public:
	OpenGLGraphicsBuffer(BufferUsage usage, uint32_t accessHint, GLenum target, ElementInitData* initData);
	~OpenGLGraphicsBuffer(void); 

	GLenum GetTarget() const;
	GLuint GetBufferID() const;

	void ResizeBuffer(uint32_t sizeInByte);
	void* Map(uint32_t offset, uint32_t length, BufferAccess options);
	void UnMap();

private:
	GLuint mBufferID;
	GLenum mTarget;
};

}

#endif // OpenGLGraphicBuffer_h__