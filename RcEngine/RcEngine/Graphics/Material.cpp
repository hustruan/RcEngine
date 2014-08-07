#include <Graphics/Material.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/Effect.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/TextureResource.h>
#include <Graphics/RenderState.h>
#include <Graphics/RenderQueue.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <Core/XMLDom.h>
#include <Core/Utility.h>
#include <Core/Exception.h>
#include <IO/FileSystem.h>
#include <IO/PathUtil.h>
#include <Resource/ResourceManager.h>
#include <Graphics/GraphicsScriptLoader.h>

namespace {

using namespace RcEngine;

uint32_t GetRenderQueueBucket(const String& str) 
{
	if (str == "Overlay")
		return RenderQueue::BucketOverlay;
	else if (str == "Background")
		return RenderQueue::BucketBackground;
	else if (str == "Opaque")
		return RenderQueue::BucketOpaque;
	else if (str == "Transparent")
		return RenderQueue::BucketTransparent;
	else if (str == "Translucent")
		return RenderQueue::BucketTranslucent;
	else if (str == "Opaque")
		return RenderQueue::BucketTransparent;
	else 
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Undefined Queue Bucket", "GetQueueBucket");
}

}

namespace RcEngine {

Material::Material( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Material, creator, handle, name, group)
{
}

Material::~Material(void)
{
	
}

shared_ptr<Resource> Material::Clone()
{
	printf("Clone material: %s\n", mMaterialName.c_str());

	shared_ptr<Material> retVal = std::make_shared<Material>(mCreator, mResourceHandle, mResourceName, mGroup);

	retVal->mMaterialName = mMaterialName;
	
	retVal->mAmbient = mEmissive;
	retVal->mDiffuse = mDiffuse;
	retVal->mSpecular = mSpecular;
	retVal->mEmissive = mEmissive;
	retVal->mPower = mPower;

	retVal->mEffect = mEffect;
	retVal->mMaterialTextureCopys = mMaterialTextureCopys;
	retVal->mTextureSRVs = mTextureSRVs;
	retVal->mAutoBindings = mAutoBindings;

	retVal->SetLoadState(Resource::Loaded);

	return retVal;
}

void Material::SetTexture( const String& name, const shared_ptr<ShaderResourceView>& textureSRV )
{
	mTextureSRVs[name] = textureSRV;

	EffectParameter* effectParam = mEffect->GetParameterByName(name);
	if (effectParam == nullptr)
	{
		String errMsg = "Texture " + name + " not exit in Effect" + mEffect->GetResourceName();
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, errMsg, "Material::SetTexture");
	}

	effectParam->SetValue(textureSRV);
}

void Material::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	shared_ptr<Stream> matStream = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *matStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	// material name 
	mMaterialName = root->AttributeString("name", "");
	mMaterialName = mResourceName;

	// effect first
	XMLNodePtr effectNode = root->FirstNode("Effect");
	String effecFile =  effectNode->AttributeString("name", "");		// file name
	
	String parentDir = PathUtil::GetPath(mResourceName);
	String effectResGroup;

	// Test if a effect exits in the same group as material
	if( fileSystem.Exits(parentDir + effecFile, mGroup) )
	{
		effectResGroup = mGroup;
		effecFile = parentDir + effecFile;  // Add material resource directory
	}
	else
		effectResGroup = "General";
	
	/* Effect name is unique resource ID, but with the effect shader macro, we can define different effect
	 * with the same file. So the full effect name is the effect file string + shader macro. By this way, 
	 * we can distinction effects.
	 */
	String effectName = effecFile;
	for (XMLNodePtr effectFlagNode = effectNode->FirstNode("Flag"); effectFlagNode; effectFlagNode = effectFlagNode->NextSibling("Flag"))
	{
		String flag = effectFlagNode->AttributeString("name", "");
		effectName += " " + flag;
	}
	
	// load effect
	mEffect = std::static_pointer_cast<Effect>( resMan.GetResourceByName(RT_Effect, effectName, effectResGroup) );

	for (XMLNodePtr paramNode = root->FirstNode("Parameter"); paramNode; paramNode = paramNode->NextSibling("Parameter"))
	{
		if (XMLAttributePtr sematicAttrib = paramNode->FirstAttribute("semantic"))
		{
			String semantic = sematicAttrib->ValueString();
			EffectParameter* effectParam = mEffect->GetParameterByUsage(Internal::EffectParamsUsageDefs::GetInstance().GetUsageType(semantic));
			
			if (effectParam == nullptr)
				continue;

			// texture type
			if (EPT_Texture1D <= effectParam->GetParameterType() && effectParam->GetParameterType() <= EPT_TextureCubeArray)
			{	
				String texFile = paramNode->AttributeString("value", "");
				if (!texFile.empty())
				{
					String resGroup = mGroup;
					String texturePath = parentDir + texFile;
					
					if (fileSystem.Exits(texturePath, mGroup) == false)
						resGroup = "General";

					shared_ptr<TextureResource> textureRes = resMan.GetResourceByName<TextureResource>(RT_Texture, texturePath, mGroup);
					mMaterialTextureCopys.push_back(textureRes->GetTexture());
					SetTexture(effectParam->GetName(), textureRes->GetTexture()->GetShaderResourceView());		
				}
			}

			// Material Color
			switch (effectParam->GetParameterUsage())
			{
			case EPU_Material_Ambient_Color:
				{
					String value = paramNode->AttributeString("value", "");
				    mAmbient = Internal::StringToFloat3(value);	
				}
				break;
			case EPU_Material_Diffuse_Color:
				{
					String value = paramNode->AttributeString("value", "");
					mDiffuse = Internal::StringToFloat3(value);		
				}
				break;
			case EPU_Material_Specular_Color:
				{
					String value = paramNode->AttributeString("value", "");
					mSpecular = Internal::StringToFloat3(value);	
				}
				break;
			case EPU_Material_Emissive_Color:
				{
					String value = paramNode->AttributeString("value", "");
					mEmissive = Internal::StringToFloat3(value);	
				}
				break;
			case EPU_Material_Power:
				{
					String value = paramNode->AttributeString("value", "");
					std::sscanf(value.c_str(), "%f", &mPower);		
				}
				break;
			default:
				break;
			}
		}
	}

	// Capture all auto-binding shader parameter
	for (auto& kv : mEffect->GetParameters())
	{
		EffectParameter* effectParam = kv.second;
		if (effectParam->GetParameterUsage() != EPU_Unknown)
			mAutoBindings.push_back(effectParam);
	}

	// Parse render queue bucket
	if (root->FirstNode("Queue"))
	{
		String bucket = root->FirstNode("Queue")->AttributeString("name", "");
		mQueueBucket = GetRenderQueueBucket(bucket);
	}
}

void Material::UnloadImpl()
{

}

shared_ptr<Resource> Material::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Material>(creator, handle, name, group);
}

EffectTechnique* Material::GetCurrentTechnique() const
{
	return mEffect->GetCurrentTechnique();
}

void Material::SetCurrentTechnique( const String& techName )
{
	mEffect->SetCurrentTechnique(techName);
}

void Material::SetCurrentTechnique( uint32_t index )
{
	mEffect->SetCurrentTechnique(index);
}

void Material::ApplyMaterial( const float4x4& world )
{
	RenderDevice* renderDevice = Environment::GetSingleton().GetRenderDevice();
	const shared_ptr<Camera> camera = renderDevice->GetCurrentFrameBuffer()->GetCamera();

	for (auto effectParam : mAutoBindings)
	{
		switch (effectParam->GetParameterUsage())
		{
		case EPU_WorldMatrix:
			{
				effectParam->SetValue(world);
			}		
			break;
		case EPU_ViewMatrix:
			{
				effectParam->SetValue(camera->GetViewMatrix());
			}			
			break;
		case EPU_ProjectionMatrix:
			{
				effectParam->SetValue(camera->GetEngineProjMatrix());
			}	
			break;
		case EPU_WorldViewMatrix:
			{
				effectParam->SetValue(world * camera->GetViewMatrix());
			}		
			break;
		case EPU_ViewProjectionMatrix:
			{
				effectParam->SetValue(camera->GetEngineViewProjMatrix());
			}			
			break;	
		case EPU_WorldViewProjection:
			{
				effectParam->SetValue(world * camera->GetEngineViewProjMatrix());
			}			
			break;
		case EPU_WorldInverseTranspose:
			{
				effectParam->SetValue(world.Inverse().Transpose());
			}
			break;
		case EPU_WorldMatrixInverse:
			{
				effectParam->SetValue(world.Inverse());
			}
			break;
		/*case EPU_ViewMatrixInverse:
			{
				effectParam->SetValue(camera->GetInvViewMatrix());
			}
			break;
		case EPU_ProjectionMatrixInverse:
			{
				effectParam->SetValue(camera->GetInvProjMatrix());
			}
			break;*/
		case EPU_Material_Ambient_Color:
			{
				effectParam->SetValue(mAmbient);
			}
			break;
		case EPU_Material_Diffuse_Color:
			{
				effectParam->SetValue(mDiffuse);
			}
			break;
		case EPU_Material_Specular_Color:
			{
				effectParam->SetValue(mSpecular);
			}
			break;
		case EPU_Material_Power:
			{
				effectParam->SetValue(mPower);
			}
			break;
		case EPU_Material_DiffuseMap:
		case EPU_Material_SpecularMap:
		case EPU_Material_NormalMap:
			{
				effectParam->SetValue(mTextureSRVs[effectParam->GetName()]);
			}
			break;
		case EPU_Camera_Position:
			{
				effectParam->SetValue(camera->GetPosition());
			}
			break;
		default:
			{

			}
		}
	}
}



} // Namespace RcEngine