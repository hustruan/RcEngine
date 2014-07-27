
#define saturate(x) clamp(x, 0.0, 1.0)	
	
//float SpotLighting(const vec3 lightPos, const vec3 lightDir, const vec2 cosCone, const vec3 litPoint)
//{
//	// calc the cos angle between lightdir and litpoint dir
//	float cosAlpha =  dot(normalize(litPoint - lightPos), lightDir);
//	return saturate( (cosAlpha - cosCone.y) / (cosCone.x - cosCone.y) );
//}

float SpotLighting(const vec3 L, const vec3 lightDir, const vec2 cosCone)
{
	// calc the cos angle between lightdir and litpoint dir
	float cosAlpha =  dot(-L, lightDir);
	return saturate( (cosAlpha - cosCone.y) / (cosCone.x - cosCone.y) );
}

float CalcAttenuation(const vec3 lightPos, const vec3 point, const vec3 atten)
{
	float d = distance(lightPos, point);
	return 1.0 / (atten.x + d * atten.y + d * d * atten.z);
}

float CalcAttenuation(float dist, vec3 atten)
{
	return 1.0 / (atten.x + dist * atten.y + dist * dist * atten.z);
}


/**
 * @brief Computes the specular contribution of a light
 */
float CalculateSpecular(vec3 N, vec3 H, float specPower)
{
    return pow(saturate(dot(N, H)), specPower);
}

/**
 * @brief Calculate luminance of color
 */
 float Luminance(vec3 color)
 {
	return dot(color, vec3(0.2126, 0.7152, 0.0722));
 }

 /**
 * @brief Calculate the Fresnel factor using Schlick's approximation for specular highlight.
 */
vec3 CalculateFresnel(in vec3 specularColor, in vec3 L, in vec3 H) 
{
	return specularColor + (1.0 - specularColor) * pow(1.0 - saturate(dot(L, H)), 5.0);
}

/**
 * @brief Calculate an approximate Fresnel factor using N dot V instead of L dot H
 *        Which is used for environment map fresnel.
 */
vec3 CalculateAmbiemtFresnel(in vec3 specularColor, in vec3 N, in vec3 V) 
{
	return specularColor + (1.0 - specularColor) * pow(1.0 - saturate(dot(N, V)), 5.0); 
}
