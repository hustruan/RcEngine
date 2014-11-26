#ifndef Effect_h__
#define Effect_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/ColorRGBA.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {	

class EffectConstantBuffer;

class _ApiExport Effect : public Resource
{
public:
	Effect(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Effect();

	void SetCurrentTechnique(uint32_t index);
	void SetCurrentTechnique(const String& techName);					
	
	inline const String& GetEffectName() const								{ return mEffectName; }
	inline EffectTechnique* GetCurrentTechnique() const 					{ return mCurrTechnique; }
	inline uint32_t GetNumTechniques() const								{ return mTechniques.size(); }
						
	EffectTechnique* GetTechniqueByName(const String& techName) const;
	EffectTechnique* GetTechniqueByIndex(uint32_t index) const;

	EffectParameter* GetParameterByName(const String& paraName) const;
	EffectParameter* GetParameterByUsage(EffectParameterUsage usage) const;
	const std::map<String, EffectParameter*>& GetParameters() const			{ return mParameters; }

	EffectConstantBuffer* GetConstantBuffer(const String& name) const;
		
protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

public_internal:

	// Get shader parameter, if not exits, create one
	EffectParameter* FetchSRVParameter(const String& name, EffectParameterType effectType);
	EffectParameter* FetchUAVParameter(const String& name, EffectParameterType effectType);
	EffectParameter* FetchSamplerParameter(const String& name);
	EffectParameter* FetchUniformParameter(const String& name, EffectParameterType effectType, uint32_t elementSize);
	EffectConstantBuffer* FetchConstantBuffer(const String& name, uint32_t bufferSize);
	
	// Create a new uniform buffer parameter 
	EffectConstantBuffer* CreateConstantBuffer(const String& name, uint32_t bufferSize);

	inline EffectConstantBuffer* GetConstantBuffer(uint32_t index) const      { return mConstantBuffers[index]; }
	inline uint32_t GetNumConstantBuffers() const							{ return mConstantBuffers.size(); }

protected:
	String mEffectName;

	EffectTechnique* mCurrTechnique;
	vector<EffectTechnique*> mTechniques;
	
	std::vector<EffectConstantBuffer*> mConstantBuffers;
	std::map<String, EffectParameter*> mParameters;

	std::map<String, shared_ptr<SamplerState> > mSamplerStates;
};

class _ApiExport EffectTechnique 
{
	friend class Effect;

public:
	EffectTechnique(Effect& effect);
	~EffectTechnique();

	inline const String& GetTechniqueName() const				{ return mName; }
	inline Effect& GetEffect()									{ return mEffect; }

	inline vector<EffectPass*>& GetPasses()					    { return mPasses; }
	inline const vector<EffectPass*>& GetPasses() const		    { return mPasses; }

	EffectPass* GetPassByName(const String& name) const;
	EffectPass* GetPassByIndex(uint32_t index) const;

protected:
	Effect& mEffect;

	String mName;
	bool mValid;
	vector<EffectPass*> mPasses;
};

class _ApiExport EffectPass
{
	friend class Effect;

public:
	EffectPass();

	inline const String& GetPassName() const									{ return mName; }
	inline const shared_ptr<ShaderPipeline>& GetShaderPipeline() const		{ return mShaderPipeline; }

	void BeginPass();
	void EndPass();

protected:
	String mName;
	uint16_t mFrontStencilRef, mBackStencilRef;
	ColorRGBA mBlendColor;
	uint32_t mSampleMask;
	shared_ptr<BlendState> mBlendState;
	shared_ptr<DepthStencilState> mDepthStencilState;
	shared_ptr<RasterizerState> mRasterizerState;
	shared_ptr<ShaderPipeline> mShaderPipeline;
};


} // Namespace RcEngine
#endif // Effect_h__