#include "OpenGLGraphicBuffer.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLGraphicsBuffer::OpenGLGraphicsBuffer(BufferUsage usage, uint32_t accessHint, GLenum target, ElementInitData* initData)
	: GraphicsBuffer(usage, accessHint), mTarget(target)
{
	assert((GL_ARRAY_BUFFER == target) || (GL_ELEMENT_ARRAY_BUFFER == target) ||
		(GL_UNIFORM_BUFFER == target) || (GL_SHADER_STORAGE_BUFFER == target) );

	glGenBuffers(1, &mBufferID);
	if (initData != NULL)
	{
		mSizeInBytes = initData->rowPitch;
		glBindBuffer(mTarget, mBufferID);
		glBufferData(mTarget, static_cast<GLsizeiptr>(mSizeInBytes),
			initData->pData, (BU_Static == mBufferUsage) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	glBindBuffer(mTarget, 0);
	
	OGL_ERROR_CHECK();
}


OpenGLGraphicsBuffer::~OpenGLGraphicsBuffer(void)
{
	glDeleteBuffers(1, &mBufferID);
}

GLenum OpenGLGraphicsBuffer::GetTarget() const
{
	return mTarget;
}

GLuint OpenGLGraphicsBuffer::GetBufferID() const
{
	return mBufferID;
}

void* OpenGLGraphicsBuffer::Map( uint32_t offset, uint32_t length, BufferAccess options )
{
	/*GLenum mapFlag;
	void* pMapBuffer = NULL;

	switch(options)
	{
	case BA_Read_Only:
	mapFlag = GL_READ_ONLY;
	break;
	case BA_Write_Only:
	mapFlag = GL_WRITE_ONLY;
	break;
	case BA_Read_Write:
	mapFlag = GL_READ_WRITE;
	break;
	}
	glBindBuffer(mTarget, mBufferID);
	pMapBuffer = glMapBuffer(mTarget, mapFlag);
	glBindBuffer(mTarget, 0);*/

	// Switch to use glMapBufferRange

	GLbitfield access;
	void* pMapBuffer = NULL;

	switch(options)
	{
	case BA_Read_Only:
		access = GL_MAP_READ_BIT;
		break;
	case BA_Write_Only:
		access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
		break;
	case BA_Read_Write:
		access = GL_MAP_READ_BIT | BA_Read_Write;
		break;
	}

	// Map all rest buffer 
	if (length == MAP_ALL_BUFFER)
		length = mSizeInBytes - offset;

	if (offset < 0 ||  offset + length > mSizeInBytes)
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Out of range!", "GraphicsBuffer::Map");

	glBindBuffer(mTarget, mBufferID);
	pMapBuffer = glMapBufferRange(mTarget, offset, length, access);
	glBindBuffer(mTarget, 0);

	OGL_ERROR_CHECK();

	return pMapBuffer;
}

void OpenGLGraphicsBuffer::UnMap()
{
	glBindBuffer(mTarget, mBufferID);
	glUnmapBuffer(mTarget);
	glBindBuffer(mTarget, 0);

	OGL_ERROR_CHECK();
}

void OpenGLGraphicsBuffer::ResizeBuffer( uint32_t sizeInByte )
{
	if (mSizeInBytes != sizeInByte)
	{
		glBindBuffer(mTarget, mBufferID);
		glBufferData(mTarget, static_cast<GLsizeiptr>(sizeInByte), NULL, 
			(BU_Static == mBufferUsage) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

		mSizeInBytes = sizeInByte;

		OGL_ERROR_CHECK();
	}
}

}

