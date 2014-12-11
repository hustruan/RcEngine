Texture2D GBufferLambertain;
Texture2D GBufferGossly;
Texture2D GBufferNormal;
Texture2D GBufferDepth;
Texture2D IndirectRadiosity;

float4 ProjInfo;
float2 ClipInfo;
float2 LightBoost;

float3 LightDirection;
float3 LightColor;

#define pi (3.1415926)
#define invPi (0.318309886)
// 1.0 / (8.0 pi); used for normalizing the glossy lobe
#define inv8pi (0.0397887358)

#include "colorBoost.hlsl"

float3 reconstructCameraSpacePosition(float2 S, float z)
{
	float csz = ClipInfo.y / ( z - ClipInfo.x);
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * csz, csz);
}

/// Compute Schlick's approximation of the Fresnel coefficient.
float3 ComputeFresnel(float3 F0, float cos_i)
{
	return lerp(F0, (float3)1.0, pow(1.0 - max(0.0, cos_i), 5.0));
}

void Deferred(in float2 iTex	   : TEXCOORD0,
			  in float4 iFragCoord : SV_Position,
			 out float4 oFragColor : SV_Target0)
{
	// Screen-space point being shaded
    int3 ssC = int3(iFragCoord.xy, 0);

	// Compute lighting in camera space
	float3 N = GBufferNormal.Load(ssC).xyz * 2.0 - 1.0;
    if (dot(N, N) < 0.01) {
        // This is a background pixel, not part of an object
        discard;
    } 

	float3 csPosition = reconstructCameraSpacePosition(iFragCoord.xy, GBufferDepth.Load(ssC).r);
	
	// View vector	
	float3 V = normalize(-csPosition);
	float3 L = normalize(-LightDirection);
	float3 H = normalize(V + L);
	
	// Lambertian coefficient in BSDF
    float3 lambertianColor = GBufferLambertain.Load(ssC).rgb ;//* invPi;

    // Glossy coefficient in BSDF (this code unpacks  G3D::UniversalBSDF's encoding)
    float4 F0 = GBufferGossly.Load(ssC);
    float glossyExponent = 10.0; //unpackGlossyExponent(F0.a);

	// Schlick's approximation of the Fresnel coefficient
    float3 F = ComputeFresnel(F0.rgb, dot(V, H));

    // G = F * (s_X + 8) / (8 pi)
    float3 glossyColor = F * (glossyExponent * (1.0 / (8.0 * pi)) + invPi);

	float3 E_lambertianAmbient = 0.0;
	float3 E_glossyAmbient = 0.0;

	/*float3 E_lambertianAmbient = computeLambertianEnvironmentMapLighting(wsN);
    float3 E_glossyAmbient     = computeGlossyEnvironmentMapLighting(w_mi, (F0.a == 1.0), glossyExponent) 
        * (8.0 / (glossyExponent + 8.0));*/
  
    float3 E_lambertian = LightColor * max( 0.0, dot(N, L) ); 
	float3 E_glossy = E_lambertian * pow( max(0.0, dot(N, H)), glossyExponent );

	//E_glossy = min((float3)0, E_glossy);
	//E_lambertian = min((float3)0, E_lambertian);

	float AO = 1.0;

	// How much ambient occlusion to apply to direct illumination (sort of approximates area lights,
    // more importantly: NPR term that adds local contrast)
    const float aoInfluenceOnDirectIllumination = 0.65;
    float directAO = lerp(1.0, AO, aoInfluenceOnDirectIllumination);

    float4 indirect = IndirectRadiosity.Load(ssC);

    const float radiosityContrastCenter = 0.35;
    float radiosityConfidence = saturate(((1.0 - indirect.a) - radiosityContrastCenter) * 2.0 + radiosityContrastCenter);
    float3 E_lambertianIndirect = radiosityConfidence * indirect.rgb * colorBoost(indirect.rgb, LightBoost.x, LightBoost.y);

	float3 emissiveColor = 0.0;
	float3 result = 0.0;

#if USE_INDIRECT
    result =
            emissiveColor +
           + (E_lambertian * directAO + E_lambertianIndirect * AO   + E_lambertianAmbient * AO * lerp(1.0, 0.3, radiosityConfidence)) * lambertianColor
           + (E_glossy     * directAO                               + E_glossyAmbient     * AO) * glossyColor;

#else
    result =
            emissiveColor +
           + (E_lambertian * directAO                               + E_lambertianAmbient * AO) * lambertianColor
           + (E_glossy     * directAO                               + E_glossyAmbient     * AO) * glossyColor;
#endif

	float a = result.r;
	//result =  indirect;
	oFragColor = float4(result, a);
}