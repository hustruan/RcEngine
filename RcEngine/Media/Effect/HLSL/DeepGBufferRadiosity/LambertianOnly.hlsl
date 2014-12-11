#include "ColorBoost.hlsl"

Texture2D GBufferLambertain;
Texture2D GBufferGossly;
Texture2D GBufferNormal;
Texture2D GBufferSSVelocity;
Texture2D GBufferDepth;

#if USE_INDIRECT
	SamplerState RadiositySampler;
	Texture2D PrevDepthBuffer;
	Texture2D PrevIndirectRadiosityBuffer;
#endif 

float4x4 InvView;
float4x4 PrevoiusInvView;

float4 ProjInfo;
float2 ClipInfo;
float2 InvViewport;
float3 LightDirection;
float3 LightColor;
float PropagationDamping;
float2 LightBoost;

#define PI 3.141592653589

//////////////////////////////////////////////////////////////////////////////////////
float3 reconstructCameraSpacePosition(float2 S, float z)
{
	float csz = ClipInfo.y / ( z - ClipInfo.x);
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * csz, csz);
}

void LambertianOnly(in float2 iTex	      : TEXCOORD0,
					in float4 iFragCoord  : SV_Position,
					out float3 oFragColor : SV_Target0)
{
	int3 ssP = int3(iFragCoord.xy, 0);

	float3 csN = GBufferNormal.Load(ssP).xyz * 2.0 - 1.0;
    if (dot(csN, csN) < 0.01) {
        // This is a background pixel, not part of an object
        discard;
    } 

	// Compute direct lighting in camera space
	float3 csPosition = reconstructCameraSpacePosition(iFragCoord.xy, GBufferDepth.Load(ssP).r);
	float3 V = normalize(-csPosition);
	float3 L = normalize(-LightDirection);

	// Lambertian coefficient in BSDF
    float3 diffuseAlbedo  = (GBufferLambertain.Load(ssP).rgb) / PI;

	// Direct light
	float3 E_lambertian = LightColor * max(0.0, dot(csN, L));

#if USE_INDIRECT
	
    float2 prevFragCoord  = iFragCoord.xy - GBufferSSVelocity.Load(ssP).rg;
    float3 indirect = PrevIndirectRadiosityBuffer.Sample(RadiositySampler, prevFragCoord * InvViewport).rgb;
    
    float epsilon = 0.05;
    float3 csPrevPosition = reconstructCameraSpacePosition(prevFragCoord, PrevDepthBuffer.Load(int3(prevFragCoord, 0)).r);
    
	// Calculate motion in world space
	float4 wsPosition = mul( float4(csPosition, 1.0), InvView );
	float4 wsPrevPosition = mul( float4(csPrevPosition, 1.0), PrevoiusInvView );
	float dist = length(wsPosition.xyz - wsPrevPosition.xyz);

    float weight = 1.0 - smoothstep(epsilon * 0.8, epsilon * 1.2, dist);
    indirect *= weight;
    E_lambertian += indirect * (1.0 - PropagationDamping) * colorBoost(indirect, LightBoost.x, LightBoost.y);

#endif

	oFragColor = diffuseAlbedo * E_lambertian;
}
