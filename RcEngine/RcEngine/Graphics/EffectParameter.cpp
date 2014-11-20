#include <Graphics/EffectParameter.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderFactory.h>
#include <Core/Environment.h>
#include <Core/Exception.h>

namespace RcEngine {

#if defined(_DEBUG) && !defined(_M_X64)

// This variable indicates how many ticks to go before rolling over
// all of the timer variables in the FX system.
// It is read from the system registry in debug builds; in retail the high bit is simply tested.

_declspec(selectany) unsigned int g_TimerRolloverCount = 0x80000000;

#endif // _DEBUG && !_M_X64

EffectParameter::EffectParameter( const String& name, EffectParameterType type, EffectConstantBuffer* pCB)
	: mParameterType(type), 
	  mParameterName(name), 
	  mConstantBuffer(pCB),
	  mElementSize(0),
	  mOffset(0),
	  mLastModifiedTime(0),
	  mParameterUsage(EPU_Unknown)
{

}

EffectParameter::~EffectParameter( void )
{

}


#define UnimplentedGetMethod(type) \
	void EffectParameter::GetValue(type& value ) const \
	{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

	UnimplentedGetMethod(bool)
	UnimplentedGetMethod(int32_t)
	UnimplentedGetMethod(int2)
	UnimplentedGetMethod(int3)
	UnimplentedGetMethod(int4)
	UnimplentedGetMethod(uint32_t)
	UnimplentedGetMethod(uint2)
	UnimplentedGetMethod(uint3)
	UnimplentedGetMethod(uint4)
	UnimplentedGetMethod(float)
	UnimplentedGetMethod(float2)
	UnimplentedGetMethod(float3)
	UnimplentedGetMethod(float4)
	UnimplentedGetMethod(int32_t*)
	UnimplentedGetMethod(int2*)
	UnimplentedGetMethod(int3*)
	UnimplentedGetMethod(int4*)
	UnimplentedGetMethod(uint32_t*)
	UnimplentedGetMethod(uint2*)
	UnimplentedGetMethod(uint3*)
	UnimplentedGetMethod(uint4*)
	UnimplentedGetMethod(float*)
	UnimplentedGetMethod(float2*)
	UnimplentedGetMethod(float3*)
	UnimplentedGetMethod(float4*)
	UnimplentedGetMethod(float4x4)
	UnimplentedGetMethod(float4x4*)
	UnimplentedGetMethod(weak_ptr<ShaderResourceView>)
	UnimplentedGetMethod(weak_ptr<UnorderedAccessView>)
	UnimplentedGetMethod(weak_ptr<SamplerState>)
#undef UnimplentedGetMethod

#define UnimplentedSetMethod(type) \
	void EffectParameter::SetValue( const type& value ) \
	{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

	UnimplentedSetMethod(bool)
	UnimplentedSetMethod(int32_t)
	UnimplentedSetMethod(int2)
	UnimplentedSetMethod(int3)
	UnimplentedSetMethod(int4)
	UnimplentedSetMethod(uint32_t)
	UnimplentedSetMethod(uint2)
	UnimplentedSetMethod(uint3)
	UnimplentedSetMethod(uint4)
	UnimplentedSetMethod(float)
	UnimplentedSetMethod(float2)
	UnimplentedSetMethod(float3)
	UnimplentedSetMethod(float4)
	UnimplentedSetMethod(float4x4)
	UnimplentedSetMethod(shared_ptr<UnorderedAccessView>)
	UnimplentedSetMethod(shared_ptr<ShaderResourceView>)
	UnimplentedSetMethod(shared_ptr<SamplerState>)
#undef UnimplentedSetMethod

#define UnimplentedSetArrayMethod(type) \
	void EffectParameter::SetValue( const type* value, uint32_t count) \
	{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

	UnimplentedSetArrayMethod(int32_t)
	UnimplentedSetArrayMethod(int2)
	UnimplentedSetArrayMethod(int3)
	UnimplentedSetArrayMethod(int4)
	UnimplentedSetArrayMethod(uint32_t)
	UnimplentedSetArrayMethod(uint2)
	UnimplentedSetArrayMethod(uint3)
	UnimplentedSetArrayMethod(uint4)
	UnimplentedSetArrayMethod(float)
	UnimplentedSetArrayMethod(float2)
	UnimplentedSetArrayMethod(float3)
	UnimplentedSetArrayMethod(float4)
	UnimplentedSetArrayMethod(float4x4)
#undef UnimplentedSetArrayMethod

void EffectParameter::SetArrayStride( uint32_t stride )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetArrayStride"); }

void EffectParameter::SetMatrixStride( uint32_t matStride )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetMatrixStride"); }

void EffectParameter::IncrementTimeStamp()
{
	mLastModifiedTime++;

#ifndef _M_X64
#if _DEBUG
	if (mLastModifiedTime > g_TimerRolloverCount)
	{
		printf("Rolling over timer (current time: %d, rollover cap: %d).", mLastModifiedTime, g_TimerRolloverCount);
#else
	if (mLastModifiedTime >= 0x80000000) // check to see if we've exceeded ~2 billion
	{
#endif
		// Rollover
		mLastModifiedTime = 1;
	}
#endif // _M_X64
}

void EffectParameter::SetConstantBuffer( EffectConstantBuffer* cbuffer, uint32_t offset )
{
	assert(mConstantBuffer == nullptr);
	mConstantBuffer = cbuffer;
	mOffset = offset;
}

void EffectParameter::MakeDirty()
{
	IncrementTimeStamp();
	if (mConstantBuffer)
		mConstantBuffer->MakeDirty();
}

//----------------------------------------------------------------------------------------
EffectConstantBuffer::EffectConstantBuffer( const String& name, uint32_t bufferSize )
	: mName(name),
	  mBufferSize(bufferSize),
	  mDirty(true)
{
	mBackingStore = new uint8_t[bufferSize];

	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
	mConstantBuffer = factory->CreateConstantBuffer(bufferSize, EAH_CPU_Write | EAH_GPU_Read, BufferCreate_Constant, NULL);
}

EffectConstantBuffer::~EffectConstantBuffer()
{
	delete[] mBackingStore;
}

void EffectConstantBuffer::AddVariable( EffectParameter* parameter, uint32_t offset )
{
	std::vector<EffectParameter*>::iterator it;
	it = std::find_if(mBufferVariable.begin(), mBufferVariable.end(), [&](const EffectParameter* param) {
						return param == parameter; });
	
	if (it == mBufferVariable.end())
	{
		parameter->SetConstantBuffer(this, offset);
		mBufferVariable.push_back(parameter);
	}
}

void EffectConstantBuffer::UpdateBuffer()
{
	if (mDirty)
	{
		void* pBuffer = mConstantBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard); 
		memcpy(pBuffer, mBackingStore, mBufferSize);
		mConstantBuffer->UnMap();
		mDirty = false;
	}
}

void EffectConstantBuffer::SetRawValue( const void* pData, uint32_t offset, uint32_t count )
{
#ifdef _DEBUG
	if ((offset + count < offset) || (offset + (uint8_t*)pData < (uint8_t*)pData) ||
		((offset + count) > mBufferSize))
	{
		// overflow of some kind
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid range specified", "EffectConstantBuffer::SetRawValue");
	}
#endif

	memcpy(mBackingStore + offset, pData, count);
	mDirty = true;
}

void EffectConstantBuffer::GetRawValue( void *pData, uint32_t offset, uint32_t count )
{
#ifdef _DEBUG
	if ((offset + count < offset) ||
		(count + (uint8_t*)pData < (uint8_t*)pData) ||
		((offset + count) > mBufferSize))
	{
		// overflow of some kind
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid range specified", "EffectConstantBuffer::GetRawValue");
	}
#endif

	memcpy(pData, mBackingStore + offset, count);
}

void EffectConstantBuffer::SetBuffer( const shared_ptr<GraphicsBuffer>& buffer )
{
	if (buffer != mConstantBuffer)
	{
		if (buffer->GetBufferSize() != mConstantBuffer->GetBufferSize() &&
			buffer->GetCreateFlags() != mConstantBuffer->GetCreateFlags() )
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Constant Buffer Does't Match!", "EffectConstantBuffer::SetBuffer");

		mConstantBuffer = buffer;
		mDirty = false;
	}
}

} // Namespace RcEngine