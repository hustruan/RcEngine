#ifndef D3D11Buffer_h__
#define D3D11Buffer_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _D3D11Export D3D11Buffer : public GraphicsBuffer
{
public:
	D3D11Buffer(uint32_t bufferSize, uint32_t accessHint, uint32_t flags, uint32_t structSize, ElementInitData* initData);
	~D3D11Buffer(void);

	void ResizeBuffer(uint32_t size);
	void* Map(uint32_t offset, uint32_t length, ResourceMapAccess options);
	void UnMap();

public:
	ID3D11Buffer* BufferD3D11;

private:
	ID3D11Buffer* mStagingBufferD3D11;
	ResourceMapAccess mBufferMapAccess;
	D3D11_USAGE mBufferUsage;
};

}
#endif // D3D11Buffer_h__