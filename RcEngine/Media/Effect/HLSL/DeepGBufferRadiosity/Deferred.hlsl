Texture2D GBufferLambertain;
Texture2D GBufferGossly;
Texture2D GBufferNormal;
Texture2D GBufferDepth;
Texture2D IndirectRadiosity;
Texture2D AmbientOcclusion;

TextureCube EnvironmentMap;
SamplerState CubeMapSampler;

float4x4 InvView;

float4 ProjInfo;
float2 ClipInfo;
float2 LightBoost;

float3 LightDirection;
float  EnvGlossyMIPConstant;

float3 LightColor;
bool   EnableSSAO;


#define pi (3.1415926)
#define invPi (0.318309886)
// 1.0 / (8.0 pi); used for normalizing the glossy lobe
#define inv8pi (0.0397887358)

#include "colorBoost.hlsl"

#define PCF
#include "PSSM.hlsl"

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

float3 ComputeLambertianEnvironmentMapLighting(float3 wsN) 
{
	// Sample the highest MIP-level to approximate Lambertian integration over the hemisphere
	const float MAXMIP = 20;
	return EnvironmentMap.SampleLevel(CubeMapSampler, wsN, MAXMIP).rgb * pi;
}

float3 ComputeGlossyEnvironmentMapLighting(float3 wsR, float glossyExponent)
{
	// We compute MIP levels based on the glossy exponent for non-mirror surfaces
	float MIPshift = -0.5 * log2(glossyExponent + 1.0);
	float MIPlevel = EnvGlossyMIPConstant + MIPshift;
	MIPlevel = max(MIPlevel, EnvironmentMap.CalculateLevelOfDetail(CubeMapSampler, wsR));
	return EnvironmentMap.SampleLevel(CubeMapSampler, wsR, MIPlevel).rgb /** pi*/;
}

void Deferred(in float2 iTex	   : TEXCOORD0,
			  in float4 iFragCoord : SV_Position,
			 out float4 oFragColor : SV_Target0)
{
	// Screen-space point being shaded
    int3 ssC = int3(iFragCoord.xy, 0);

	// Compute lighting in world space
	float3 N = GBufferNormal.Load(ssC).xyz;
    if (dot(N, N) < 0.1) {
        // This is a background pixel, not part of an object
		 discard;
    } 
	else {
		N = mul( normalize(N * 2.0 - 1.0), (float3x3)InvView );
	}

	float3 csPosition = reconstructCameraSpacePosition(iFragCoord.xy, GBufferDepth.Load(ssC).r);
	float4 wsPosition = mul( float4(csPosition, 1.0), InvView );
	
	// View vector	
	float3 V = normalize( InvView[3].xyz - wsPosition.xyz );
	float3 L = normalize(-LightDirection);
	float3 H = normalize(V + L);
	float3 R = reflect(-V, N);

	// Lambertian coefficient in BSDF
    float3 lambertianColor = GBufferLambertain.Load(ssC).rgb * invPi;

    // Glossy coefficient in BSDF (this code unpacks  G3D::UniversalBSDF's encoding)
    float4 F0 = GBufferGossly.Load(ssC);
    float glossyExponent = 10.0; //unpackGlossyExponent(F0.a);

	// Schlick's approximation of the Fresnel coefficient
	float3 F = ComputeFresnel(F0.rgb, dot(V, H));

	// G = F * (s_X + 8) / (8 pi)
	float3 glossyColor = F * (glossyExponent * (1.0 / (8.0 * pi)) + invPi);

	float3 E_lambertianAmbient = ComputeLambertianEnvironmentMapLighting(N);
	float3 E_glossyAmbient = ComputeGlossyEnvironmentMapLighting(R, glossyExponent) * (8.0 / (glossyExponent + 8.0));

	// Compute shadow
	int iCascadeSelected = 0;
	float percentLit = EvalCascadeShadow(wsPosition, iCascadeSelected);

    float3 E_lambertian = LightColor * max( 0.0, dot(N, L) ) * percentLit; 
	float3 E_glossy = E_lambertian * pow( max(0.0, dot(N, H)), glossyExponent );

	float AO = 1.0;
	if (EnableSSAO) {
		AO = 0.95 * AmbientOcclusion.Load(ssC).r + 0.05;
	}

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

	//E_lambertian = min((float3)0.0, E_lambertian);
	//E_glossy = min((float3)0.0, E_glossy);
	//E_lambertianIndirect = min((float3)0.0, E_lambertianIndirect);
	//E_lambertianAmbient = min((float3)0.0, E_lambertianAmbient);
	//E_glossyAmbient = min((float3)0.0, E_glossyAmbient);

#if USE_INDIRECT
    result =
            emissiveColor +
           + (E_lambertian * directAO + E_lambertianIndirect * AO   + E_lambertianAmbient * AO * lerp(0.7, 0.3, radiosityConfidence)) * lambertianColor
           + (E_glossy     * directAO                               + E_glossyAmbient     * AO) * glossyColor;

#else
    result =
            emissiveColor +
           + (E_lambertian * directAO                               + E_lambertianAmbient * AO) * lambertianColor
           + (E_glossy     * directAO                               + E_glossyAmbient     * AO) * glossyColor;
#endif

	//oFragColor = float4(E_lambertian, result.r);
	oFragColor = float4(result, 1.0);
}