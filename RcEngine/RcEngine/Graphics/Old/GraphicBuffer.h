#pragma once
#ifndef GraphicBuffer_h__
#define GraphicBuffer_h__

#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

#define MAP_ALL_BUFFER 0xFFFFFFFF

class _ApiExport GraphicsBuffer
{
public:
	GraphicsBuffer(BufferUsage usage, uint32_t accessHint, uint32_t flags);
	virtual ~GraphicsBuffer(void);

	inline BufferUsage		GetBufferUsage() const { return mBufferUsage; }
	inline uint32_t			GetAccessHint() const  { return mAccessHint; }
	inline uint32_t			GetBufferSize() const  { return mSizeInBytes; }
	inline uint32_t         GetFlags() const       { return mFlags; }

	virtual void ResizeBuffer(uint32_t sizeInByte) = 0;
	virtual void* Map(uint32_t offset, uint32_t length, BufferAccess options) = 0;
	virtual void UnMap() = 0;

protected:
	uint32_t mSizeInBytes;
	BufferUsage mBufferUsage;
	uint32_t mAccessHint;		// CPU/GPU access flags 
	uint32_t mFlags;            // Buffer create flags
};


} // Namespace RcEngine

#endif // GraphicBuffer_h__