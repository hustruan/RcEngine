#include <Scene/Light.h>
#include <Core/Exception.h>
#include <Scene/SceneNode.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Light::Light( const String& name, LightType type)
	: SceneObject(name, SOT_Light, false), 
	  mLightType(type), 
	  mLightPosition(float3::Zero()),
	  mLightDirection(float3(0, 1, 0)),
	  mLightColor(1, 1, 1),
	  mAttenuation(1.0f, 0.0f, 1.0f),
	  mSpotInnerAngle(Mathf::ToRadian(30.0f)), 
	  mSpotOuterAngle(Mathf::ToRadian(60.0f)), 
	  mSpotFalloff(1.0f),
	  mSpotNearClip(1.0f),
	  mRange(100000),
	  mDerivedPosition(float3::Zero()), 
	  mDerivedDirection(float3(0, 0, 1)),
	  mDerivedTransformDirty(false),
	  mCastShadow(false),
	  mSplitLambda(0.75f),
	  mShadowCascades(3)
{
	// Init default light intensity
	if (mLightType == LT_DirectionalLight)
		mLightIntensity = 0.5;
	else 
		mLightIntensity = 1.0;
}

Light::~Light()
{

}

void Light::SetSpotlightNearClip( float nearClip )
{
	mSpotNearClip = nearClip;
}

void Light::SetPosition( const float3& pos )
{
	mLightPosition = pos;
	mDerivedTransformDirty = true;
}

void Light::SetLightColor( const float3& color )
{
	mLightColor = color;
}

void Light::SetDirection( const float3& vec )
{
	mLightDirection = vec;
	mLightDirection = Normalize(mLightDirection);
	mDerivedTransformDirty = true;
}

void Light::SetAttenuation( float constant, float linear, float quadratic )
{
	mAttenuation = float3(constant, linear, quadratic);
}

void Light::SetRange( float range )
{
	mRange = range;
}

void Light::SetLightIntensity( float intensity )
{
	mLightIntensity = intensity;
}

void Light::SetSpotAngle( float innerAngleRadian, float outerAngleRadian, float falloff /*= 1.0*/ )
{
	if (mLightType != LT_SpotLight)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "SetSpotInnerAngle is only valid for spotlights.", "Light::SetSpotInnerAngle");
	}

	mSpotInnerAngle = innerAngleRadian;
	mSpotOuterAngle = outerAngleRadian;
	mSpotFalloff = falloff;
}

void Light::SetSpotInnerAngle( float innerAngleRadian )
{
	if (mLightType != LT_SpotLight)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "SetSpotInnerAngle is only valid for spotlights.", "Light::SetSpotInnerAngle");
	}

	mSpotInnerAngle = innerAngleRadian;
}

void Light::SetSpotOuterAngle( float outerAngleRadian )
{
	if (mLightType != LT_SpotLight)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "SetSpotOuterAngle is only valid for spotlights.", "Light::SetSpotOuterAngle");
	}

	mSpotOuterAngle = outerAngleRadian;
}

void Light::SetSpotFalloff( float exponent )
{
	if (mLightType != LT_SpotLight)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "SetSpotFalloff is only valid for spotlights.", "Light::SetSpotFalloff");
	}

	mSpotFalloff = exponent;
}


const float3& Light::GetDerivedPosition() const
{
	UpdateTransform();
	return mDerivedPosition;
}

const float3& Light::GetDerivedDirection() const
{
	UpdateTransform();
	return mDerivedDirection;
}

void Light::UpdateTransform() const
{
	if (mDerivedTransformDirty)
	{
		if (mParentNode)
		{
			// Ok, update with SceneNode we're attached to
			const Quaternionf& parentOrientation = mParentNode->GetWorldRotation();
			const float3& parentPosition = mParentNode->GetWorldPosition();
			mDerivedDirection = Transform(mLightDirection, parentOrientation);
			mDerivedPosition = Transform(mLightPosition, parentOrientation) + parentPosition;
		}
		else
		{
			mDerivedPosition = mLightPosition;
			mDerivedDirection = mLightDirection;
		}
		
		mDerivedDirection = Normalize(mDerivedDirection);
		mDerivedTransformDirty = false;
	}
}

SceneObject* Light::FactoryFunc( const String& name, const NameValuePairList* params )
{
	auto it = params->find("LightType");
	if (it != params->end())
	{
		if (it->second == "PointLight")
			return new Light(name, LT_PointLight);
		else if (it->second == "SpotLight")
			return new Light(name, LT_SpotLight);
		else if (it->second == "DirectionalLight")
			return new Light(name, LT_DirectionalLight);
	}
	
	ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Unsupported Light Type!", "Light::FactoryFunc");
}








}