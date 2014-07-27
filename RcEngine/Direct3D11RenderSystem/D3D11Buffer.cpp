#include "D3D11Buffer.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Buffer::D3D11Buffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags, uint32_t structSize, ElementInitData* initData )
	: GraphicsBuffer(bufferSize, accessHint, flags),
	  BufferD3D11(nullptr),
	  mStagingBufferD3D11(nullptr)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.ByteWidth = bufferSize;
	D3D11Mapping::Mapping(accessHint, desc.Usage, desc.CPUAccessFlags);

	if (mCreateFlags & BufferCreate_Uniform)		desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	if (mCreateFlags & BufferCreate_Index)			desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	if (mCreateFlags & BufferCreate_Vertex)			desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	if (mCreateFlags & BufferCreate_StreamOutput)	desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	if (mCreateFlags & BufferCreate_Structured) 
	{
		desc.StructureByteStride = structSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}
	if (mCreateFlags & BufferCreate_ShaderResource)	desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (mCreateFlags & BufferCreate_UAV)			
	{
		assert(desc.Usage == D3D11_USAGE_DEFAULT);
		desc.BindFlags |=  D3D11_BIND_UNORDERED_ACCESS;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	if (initData)
	{
		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = initData->pData;
		//subData.SysMemPitch = initData->rowPitch;
		D3D11_VERRY( deviceD3D11->CreateBuffer( &desc, &subData, &BufferD3D11 ) );
	}
	else 
	{
		D3D11_VERRY( deviceD3D11->CreateBuffer( &desc, NULL, &BufferD3D11 ) );
	}

	// Keep track of usage
	mBufferUsage = desc.Usage;
}


D3D11Buffer::~D3D11Buffer(void)
{
	SAFE_RELEASE(BufferD3D11);
	SAFE_RELEASE(mStagingBufferD3D11);
}

void* D3D11Buffer::Map( uint32_t offset, uint32_t length, ResourceMapAccess mapType )
{
	ID3D11Buffer* mappedResourceD3D11;
	
	bool hadReadMap = (mapType == RMA_Read_Only || mapType == RMA_Read_Write);
	if ( mBufferUsage != D3D11_USAGE_DYNAMIC || hadReadMap )
	{
		if (!mStagingBufferD3D11)
		{
			// Create staging texture if not created.
			D3D11_BUFFER_DESC bufferDesc;
			BufferD3D11->GetDesc(&bufferDesc);

			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			bufferDesc.BindFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_STAGING;

			ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
			D3D11_VERRY( deviceD3D11->CreateBuffer( &bufferDesc, NULL, &mStagingBufferD3D11) );
		}

		if (hadReadMap)
		{
			ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
			deviceContextD3D11->CopyResource(mStagingBufferD3D11, BufferD3D11);
		}

		mappedResourceD3D11 = mStagingBufferD3D11;
	}
	else
	{
		mappedResourceD3D11 = BufferD3D11;
	}
	
	if (length == MAP_ALL_BUFFER)
	{
		assert(offset < mBufferSize);
		length = mBufferSize - offset;
	}
	else
	{
		if (offset + length > mBufferSize)
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Out of range!", "D3D11Buffer::Map");
	}

	D3D11_MAPPED_SUBRESOURCE mappedD3D11;
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	D3D11_VERRY( deviceContextD3D11->Map(mappedResourceD3D11, 0, D3D11Mapping::Mapping(mapType), 0, &mappedD3D11) );
	
	mBufferMapAccess = mapType;	
	return (uint8_t*)mappedD3D11.pData + offset;
}

void D3D11Buffer::UnMap()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mBufferMapAccess == RMA_Read_Only)
	{
		deviceContextD3D11->Unmap(mStagingBufferD3D11, 0);
	}
	else if (mBufferMapAccess == RMA_Read_Write || mBufferUsage != D3D11_USAGE_DYNAMIC)
	{
		deviceContextD3D11->Unmap(mStagingBufferD3D11, 0);
		deviceContextD3D11->CopyResource(BufferD3D11, mStagingBufferD3D11);
	}
	else
	{
		deviceContextD3D11->Unmap(BufferD3D11, 0);
	}
}

void D3D11Buffer::ResizeBuffer( uint32_t size )
{

}

}


