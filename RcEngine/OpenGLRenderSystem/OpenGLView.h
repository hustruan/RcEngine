#ifndef OpenGLShaderResourceView_h__
#define OpenGLShaderResourceView_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLShaderResourceView : public ShaderResourceView
{
public:
	virtual ~OpenGLShaderResourceView() {}

	// Bind resource to (Texture/StorageBuffer unit).
	virtual void BindSRV(GLuint unitOrBindingPoint) = 0;

protected:
	GLuint mResourceOGL;
	GLenum mTargetOGL;
};

class _OpenGLExport OpenGLTextureBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferSRV();

	void BindSRV(GLuint textureUnit);
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);

	void BindSRV(GLuint bindingPoint);

private:
	GLintptr mBufferOffset;
	GLsizeiptr mBufferSize;
};

class _OpenGLExport OpenGLTextureSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	~OpenGLTextureSRV();

	// For Internal Use
	OpenGLTextureSRV(GLuint textureOGL, GLenum targetOGL);

	void BindSRV(GLuint textureUnit);

protected:
	bool mNeedDelete;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLUnorderedAccessView : public UnorderedAccessView
{
public:
	virtual ~OpenGLUnorderedAccessView() {}

	virtual void Clear(const float4& clearData);
	virtual void Clear(const uint4& clearData);

	// Bind resource to (Image/StorageBuffer unit).
	virtual void BindUAV(GLuint unitOrBindingPoint) = 0;

protected:
	GLuint mResourceOGL;
	GLenum mTargetOGL;
};

class _OpenGLExport OpenGLTextureBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferUAV();

	void BindUAV(GLuint imageUnit);

private:
	GLenum mInternalFormat;
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);
	
	void BindUAV(GLuint bindingPoint);

private:
	GLintptr mBufferOffset;
	GLsizeiptr mBufferSize;
};

class _OpenGLExport OpenGLTextureUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureUAV(const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
    ~OpenGLTextureUAV();

	void Clear(const float4& clearData);
	void Clear(const uint4& clearData);

	void BindUAV(GLuint imageUnit);

protected:
	GLboolean mNeedDelete;

	GLenum mInternalFormat;
	GLenum mExternalFormat;
	GLenum mFormatType;

	GLint mLevel;
	GLint mFirstLayer;
	GLint mNumLayers;
};

}

#endif // OpenGLShaderResourceView_h__
