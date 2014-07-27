#ifndef LightingUtil_h__
#define LightingUtil_h__

float SpotLighting(const float3 L, const float3 lightDir, const float2 cosCone)
{
	// calc the cos angle between lightdir and litpoint dir
	float cosAlpha =  dot(-L, lightDir);
	return saturate( (cosAlpha - cosCone.y) / (cosCone.x - cosCone.y) );
}

float CalcAttenuation(const float3 lightPos, const float3 litPoint, const float3 atten)
{
	float d = distance(lightPos, litPoint);
	return 1.0 / (atten.x + d * atten.y + d * d * atten.z);
}

float CalcAttenuation(float d, const float3 atten)
{
	return 1.0 / (atten.x + d * atten.y + d * d * atten.z);
}


/**
 * @brief Computes the specular contribution of a light
 */
float CalculateSpecular(float3 N, float3 H, float specPower)
{
    return pow(saturate(dot(N, H)), specPower);
}

/**
 * @brief Calculate luminance of color
 */
 float Luminance(float3 color)
 {
	return dot(color, float3(0.2126, 0.7152, 0.0722));
 }

 /**
 * @brief Calculate the Fresnel factor using Schlick's approximation for specular highlight.
 */
float3 CalculateFresnel(in float3 specularColor, in float3 L, in float3 H) 
{
	return specularColor + (1.0 - specularColor) * pow(1.0 - saturate(dot(L, H)), 5.0);
}

/**
 * @brief Calculate an approximate Fresnel factor using N dot V instead of L dot H
 *        Which is used for environment map fresnel.
 */
float3 CalculateAmbiemtFresnel(in float3 specularColor, in float3 N, in float3 V) 
{
	return specularColor + (1.0 - specularColor) * pow(1.0 - saturate(dot(N, V)), 5.0); 
}

#endif