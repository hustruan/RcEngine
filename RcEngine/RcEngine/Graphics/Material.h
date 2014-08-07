#ifndef Material_h__
#define Material_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>
#include <Graphics/GraphicsCommon.h>
#include <Resource/Resource.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderQueue.h>


namespace RcEngine {
	
static const int32_t MaxMaterialTextures = 16;

struct _ApiExport MaterialParameter
{
	String Name;
	bool IsSemantic;
	EffectParameterType Type;
	EffectParameterUsage Usage;
	EffectParameter* EffectParam;
};

class _ApiExport Material : public Resource
{
public:
	Material(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	virtual ~Material(void);

	const String& GetName() const						{ return mMaterialName; }
	uint32_t GetQueueBucket() const						{ return mQueueBucket; }

	shared_ptr<Effect> GetEffect() const				{ return mEffect; }
	
	EffectTechnique* GetCurrentTechnique() const;
	void SetCurrentTechnique(const String& techName);
	void SetCurrentTechnique(uint32_t index);

	void SetAmbientColor(const float3& ambient)		{ mAmbient = ambient; }
	void SetDiffuseColor(const float3& diffuse)		{ mDiffuse = diffuse; }
	void SetSpecularColor(const float3& specular)	{ mSpecular = specular; }
	void SetSpecularPower(float power)				{ mPower = power; }

	void SetTexture(const String& name, const shared_ptr<ShaderResourceView>& textureSRV);

	// Apply shader parameter before rendering, called by renderable
	void ApplyMaterial(const float4x4& world = float4x4::Identity());

	shared_ptr<Resource> Clone();

protected:

	void LoadImpl();
    void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:	
	
	String mMaterialName;
	shared_ptr<Effect> mEffect;
		
	uint32_t mQueueBucket;

	float3 mAmbient;
	float3 mDiffuse;
	float3 mSpecular;
	float3 mEmissive;
	float mPower;
	
	vector<shared_ptr<Texture> > mMaterialTextureCopys;
	unordered_map<String, shared_ptr<ShaderResourceView> > mTextureSRVs;		

	vector<EffectParameter*> mAutoBindings;
};


} // Namespace RcEngine

#endif // Material_h__

