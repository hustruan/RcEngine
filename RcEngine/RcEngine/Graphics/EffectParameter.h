#ifndef EffectParameter_h__
#define EffectParameter_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

typedef size_t TimeStamp;

class _ApiExport EffectConstantBuffer
{
public:
	EffectConstantBuffer(const String& name, uint32_t bufferSize);
	~EffectConstantBuffer();

	inline const String& GetName() const							{ return mName; }
	inline uint32_t GetBufferSize() const							{ return mBufferSize; }
	inline shared_ptr<GraphicsBuffer> GetBuffer() const				{ return mConstantBuffer; }
	inline uint32_t GetNumVariables() const							{ return mBufferVariable.size(); }
	inline EffectParameter* GetVariable(uint32_t index) const		{ return mBufferVariable.at(index); }
	
	inline void MakeDirty()											{ mDirty = true; }
	inline void ClearDirty()										{ mDirty = false; }
	inline uint8_t* GetRawData(uint32_t offset) const				{ return mBackingStore + offset; }

	void UpdateBuffer();
	void SetRawValue(const void* pData, uint32_t offset, uint32_t count);
	void GetRawValue(void *pData, uint32_t offset, uint32_t count);

public_internal:
	void AddVariable(EffectParameter* parameter, uint32_t offset); 

protected:
	String mName;
	uint8_t* mBackingStore;
	uint32_t mBufferSize;
	shared_ptr<GraphicsBuffer> mConstantBuffer;
	std::vector<EffectParameter*> mBufferVariable;

	bool mDirty;
};

class _ApiExport EffectParameter
{
	friend class EffectConstantBuffer;

public:
	EffectParameter(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr);
	virtual ~EffectParameter(void);

	inline uint32_t GetElementSize() const					{ return mElementSize; }
	inline const String& GetName() const					{ return mParameterName; }
	inline EffectParameterType GetParameterType() const		{ return mParameterType; }
	inline EffectParameterUsage GetParameterUsage() const	{ return mParameterUsage; }
	inline uint32_t GetOffset() const						{ return mOffset; }

	// Set and Get method, call the matched parameter type version, or will cause exception
	virtual void GetValue(bool& value) const;
	virtual void GetValue(int32_t& value) const;
	virtual void GetValue(int2& value) const;
	virtual void GetValue(int3& value) const;
	virtual void GetValue(int4& value) const;
	virtual void GetValue(uint32_t& value) const;
	virtual void GetValue(uint2& value) const;
	virtual void GetValue(uint3& value) const;
	virtual void GetValue(uint4& value) const;
	virtual void GetValue(float& value) const;
	virtual void GetValue(float2& value) const;
	virtual void GetValue(float3& value) const;
	virtual void GetValue(float4& value) const;
	virtual void GetValue(float4x4& value) const;

	virtual void GetValue(int32_t*& value) const;
	virtual void GetValue(int2*& value) const;
	virtual void GetValue(int3*& value) const;
	virtual void GetValue(int4*& value) const;
	virtual void GetValue(uint32_t*& value) const;
	virtual void GetValue(uint2*& value) const;
	virtual void GetValue(uint3*& value) const;
	virtual void GetValue(uint4*& value) const;
	virtual void GetValue(float*& value) const;
	virtual void GetValue(float2*& value) const;
	virtual void GetValue(float3*& value) const;
	virtual void GetValue(float4*& value) const;
	virtual void GetValue(float4x4*& value) const;

	virtual void GetValue(weak_ptr<ShaderResourceView>& value) const;
	virtual void GetValue(weak_ptr<UnorderedAccessView>& value) const;
	virtual	void GetValue(weak_ptr<SamplerState>& value) const;

	virtual void SetValue(const bool& value);
	virtual void SetValue(const int32_t& value);
	virtual void SetValue(const int2& value);
	virtual void SetValue(const int3& value);
	virtual void SetValue(const int4& value);
	virtual void SetValue(const uint32_t& value);	
	virtual void SetValue(const uint2& value);
	virtual void SetValue(const uint3& value);
	virtual void SetValue(const uint4& value);
	virtual void SetValue(const float& value);
	virtual void SetValue(const float2& value);
	virtual void SetValue(const float3& value);
	virtual void SetValue(const float4& value);
	virtual void SetValue(const float4x4& value);

	virtual void SetValue(const int32_t* value, uint32_t count);
	virtual void SetValue(const int2* value, uint32_t count);
	virtual void SetValue(const int3* value, uint32_t count);
	virtual void SetValue(const int4* value, uint32_t count);
	virtual void SetValue(const uint32_t* value, uint32_t count);
	virtual void SetValue(const uint2* value, uint32_t count);
	virtual void SetValue(const uint3* value, uint32_t count);
	virtual void SetValue(const uint4* value, uint32_t count);
	virtual void SetValue(const float* value, uint32_t count);
	virtual void SetValue(const float2* value, uint32_t count);
	virtual void SetValue(const float3* value, uint32_t count);
	virtual void SetValue(const float4* value, uint32_t count);
	virtual void SetValue(const float4x4* value, uint32_t count);
	
	virtual void SetValue(const shared_ptr<ShaderResourceView>& value);
	virtual void SetValue(const shared_ptr<UnorderedAccessView>& value);  // Need to consider D3D11 Atomic Counter 
	virtual void SetValue(const shared_ptr<SamplerState>& value);

public_internal:
	// Make constant buffer dirty
	inline TimeStamp GetTimeStamp() const					{ return mLastModifiedTime; }
	
	void MakeDirty();
	void SetConstantBuffer(EffectConstantBuffer* cbuffer, uint32_t offset);	

	virtual void SetArrayStride(uint32_t stride);
	virtual void SetMatrixStride(uint32_t matStride);

protected:
	void IncrementTimeStamp();

protected:
	String mParameterName;

	EffectParameterType mParameterType;
	EffectParameterUsage mParameterUsage;   // Used for shader parameter auto binding

	TimeStamp mLastModifiedTime;

	uint32_t mElementSize;					// For non-array type, always 0.
	uint32_t mOffset;						// Offset in parent constant buffer
	EffectConstantBuffer* mUniformBuffer;   // Constant buffer this variable belong to

	friend class Effect;
};

template< typename T>
class _ApiExport EffectParameterNumberic : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) {}

	void GetValue(T& value) const { value = mValue; }
	void SetValue(const T& value)
	{
		if (value != mValue)
		{
			mValue = value;
			
			// Update data in uniform buffer
			if (mUniformBuffer)
			{
				*(reinterpret_cast<T*>(mUniformBuffer->GetRawData(mOffset))) = mValue;
			}

			MakeDirty();
		}
	}

protected:
	T mValue;
};

// For boolean type
template<>
class _ApiExport EffectParameterNumberic<bool> : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) {}

	void GetValue(bool& value) const { value = mValue; }
	void SetValue(const bool& value)
	{
		if (value != mValue)
		{
			mValue = value;

			// Update data in uniform buffer
			if (mUniformBuffer)
			{
				*(reinterpret_cast<int*>(mUniformBuffer->GetRawData(mOffset))) = mValue ? 1 : 0;
			}

			MakeDirty();
		}
	}

protected:
	bool mValue;
};

template< typename T >
class _ApiExport EffectParameterNumbericArray : public EffectParameter
{
public:
	EffectParameterNumbericArray(const String& name, EffectParameterType type, uint32_t arrSize, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB)
	{  
		mElementSize = arrSize;
		mValue = new T[arrSize];
	}

	~EffectParameterNumbericArray()
	{ 
		delete[] mValue;
	}

	void SetArrayStride(uint32_t stride)
	{ 
		mArrayStrides = stride; 
	}

	void GetValue(T*& value) const	
	{ 
		value = mValue;
	}

	void SetValue(const T* value, uint32_t count)
	{
		assert(count <= mElementSize);
		
		//if (memcmp(value, mValue, sizeof(T) * count) != 0)
		{
			memcpy(mValue, value, sizeof(T) * count);
			
			// Update data in constant buffer
			if (mUniformBuffer)
			{
				uint8_t* pData = mUniformBuffer->GetRawData(mOffset);

				if (mArrayStrides == sizeof(T))
					memcpy(pData, mValue, sizeof(T) * count);
				else 
				{
					for (uint32_t i = 0; i < count; ++i)
						memcpy(pData + i * mArrayStrides, mValue + i, sizeof(T));
				}
			}

			MakeDirty();
		}
	}

protected:
	T* mValue;

	// Array strides in parent constant buffer
	uint32_t mArrayStrides;
};

// Matrix type need more attention
template<>
class _ApiExport EffectParameterNumberic<float4x4> : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) {}

	virtual void SetMatrixStride(uint32_t stride)	 { mMatrixStride = stride; }
	virtual void GetValue(float4x4& value) const	 { value = mValue; }
	virtual void SetValue(const float4x4& value)
	{
		if (value != mValue)
		{
			mValue = value;

			// Update data in constant buffer
			if (mUniformBuffer)
			{
				// Column-major matrix in shader 
				*(reinterpret_cast<float4x4*>(mUniformBuffer->GetRawData(mOffset))) = mValue.Transpose();		
			}

			MakeDirty();
		}
	}

protected:
	float4x4 mValue;
	uint32_t mMatrixStride;
};

template<>
class _ApiExport EffectParameterNumbericArray<float4x4> : public EffectParameter
{
public:
	EffectParameterNumbericArray(const String& name, EffectParameterType type, uint32_t arrSize, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB)
	{ 
		mElementSize = arrSize;
		mValue = new float4x4[arrSize];
	}

	~EffectParameterNumbericArray()
	{ 
		delete[] mValue;
	}

	virtual void SetMatrixStride(uint32_t stride) { mMatrixStride = stride; }
	virtual void SetArrayStride(uint32_t stride)  { mArrayStrides = stride; }
	virtual void GetValue(float4x4*& value) const { value = mValue; }

	virtual void SetValue(const float4x4* value, uint32_t count)
	{
		assert(count <= mElementSize);

		if (mUniformBuffer)
		{
			assert(mMatrixStride == sizeof(float4));
			assert(count <= mElementSize);
			/**
			 * Both GLSL and HLSL matrix use column-major as default pack ordering. So there has
			 * two choice. Transpose every matrix before sending to ConstantBuffer, or use row_major 
			 * qualifier in shader language.
			 */
			float4x4* pMatrix = reinterpret_cast<float4x4*>( mUniformBuffer->GetRawData(mOffset) );
			for (uint32_t i = 0; i < count; ++i)
				pMatrix[i] = value[i].Transpose();		
		}
		else
		{
			// OpenGL global uniform, OpenGLAPI will handle it
			memcpy(mValue, value, sizeof(float4x4) * count);
		}

		MakeDirty();


		//if (memcmp(value, mValue, sizeof(float4x4) * count) != 0)
		//{
		//	// OpenGL global uniform, OpenGLAPI will handle it
		//	memcpy(mValue, value, sizeof(float4x4) * count);

		//	// Update data in constant buffer
		//	if (mUniformBuffer)
		//	{
		//		assert(mMatrixStride == sizeof(float4));
		//		assert(count <= mElementSize);
		//		/**
		//		 * Both GLSL and HLSL matrix use column-major as default pack ordering. So there has
		//		 * two choice. Transpose every matrix before sending to ConstantBuffer, or use row_major 
		//		 * qualifier in shader language.
		//		 */
		//		float4x4* pMatrix = reinterpret_cast<float4x4*>( mUniformBuffer->GetRawData(mOffset) );
		//		for (uint32_t i = 0; i < count; ++i)
		//			pMatrix[i] = mValue[i].Transpose();		
		//	}

		//	MakeDirty();
		//}
	}

protected:
	float4x4* mValue;

	// Array strides in parent constant buffer
	uint32_t mArrayStrides;

	// Matrix row or column stride in constant buffer
	uint32_t mMatrixStride;
};


typedef EffectParameterNumberic< bool >								EffectParameterBool;
typedef EffectParameterNumberic< uint32_t >							EffectParameterUInt;
typedef EffectParameterNumberic< uint2 >							EffectParameterUInt2;
typedef EffectParameterNumberic< uint3 >							EffectParameterUInt3;
typedef EffectParameterNumberic< uint4 >							EffectParameterUInt4;
typedef EffectParameterNumberic< int32_t >							EffectParameterInt;
typedef EffectParameterNumberic< int2 >								EffectParameterInt2;
typedef EffectParameterNumberic< int3 >								EffectParameterInt3;
typedef EffectParameterNumberic< int4 >								EffectParameterInt4;
typedef EffectParameterNumberic< float >							EffectParameterFloat;
typedef EffectParameterNumberic< float2 >							EffectParameterVector2;
typedef EffectParameterNumberic< float3 >							EffectParameterVector3;
typedef EffectParameterNumberic< float4 >							EffectParameterVector4;
typedef EffectParameterNumberic< float4x4 >							EffectParameterMatrix;

typedef EffectParameterNumbericArray< uint32_t >					EffectParameterUIntArray;
typedef EffectParameterNumbericArray< int32_t>						EffectParameterIntArray;
typedef EffectParameterNumbericArray< uint2 >						EffectParameterUInt2Array;
typedef EffectParameterNumbericArray< uint3 >						EffectParameterUInt3Array;
typedef EffectParameterNumbericArray< uint4 >						EffectParameterUInt4Array;
typedef EffectParameterNumbericArray< int2 >						EffectParameterInt2Array;
typedef EffectParameterNumbericArray< int3 >						EffectParameterInt3Array;
typedef EffectParameterNumbericArray< int4 >						EffectParameterInt4Array;
typedef EffectParameterNumbericArray< float >						EffectParameterFloatArray;
typedef EffectParameterNumbericArray< float2 >						EffectParameterVector2Array;
typedef EffectParameterNumbericArray< float3 >						EffectParameterVector3Array;
typedef EffectParameterNumbericArray< float4 >						EffectParameterVector4Array;
typedef EffectParameterNumbericArray< float4x4 >					EffectParameterMatrixArray;


class _ApiExport EffectSRVParameter : public EffectParameter
{
public:
	EffectSRVParameter(const String& name, EffectParameterType type)
		: EffectParameter(name, type) {}

	void GetValue(weak_ptr<ShaderResourceView>& value) const
	{
		value = mSRV;
	}

	void SetValue(const shared_ptr<ShaderResourceView>& value)
	{
		if (mSRV.lock() != value)
		{
			mSRV = value;
			MakeDirty();
		}
	}

private:
	weak_ptr<ShaderResourceView> mSRV;
};

class _ApiExport EffectUAVParameter : public EffectParameter
{
public:
	EffectUAVParameter(const String& name, EffectParameterType type)
		: EffectParameter(name, type) {}

	void GetValue(weak_ptr<UnorderedAccessView>& value) const
	{
		value = mUAV;
	}

	void SetValue(const shared_ptr<UnorderedAccessView>& value)
	{
		if (mUAV.lock() != value)
		{
			mUAV = value;
			MakeDirty();
		}
	}

private:
	weak_ptr<UnorderedAccessView> mUAV;
};

class _ApiExport EffectSamplerParameter : public EffectParameter
{
public:
	EffectSamplerParameter(const String& name) : EffectParameter(name, EPT_Sampler) {}

	void GetValue(weak_ptr<SamplerState>& value) const
	{
		value = mSamplerState;
	}

	void SetValue(const shared_ptr<SamplerState>& value)
	{
		if (mSamplerState.lock() != value)
		{
			mSamplerState = value;
			MakeDirty();
		}
	}

private:
	weak_ptr<SamplerState> mSamplerState;
};

} // Namespace RcEngine


#endif // EffectParameter_h__
