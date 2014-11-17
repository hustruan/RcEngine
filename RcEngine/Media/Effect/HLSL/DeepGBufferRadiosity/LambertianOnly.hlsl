
Texture2D GBufferLambertain;
Texture2D GBufferGossly;
Texture2D GBufferNormal;
Texture2D GBufferSSVelocity;
Texture2D GBufferDepth;

#ifdef USE_INDIRECT

	Texture2D PrevDepthBuffer;
	Texture2D PrevIndirectRadiosityBuffer;

	SamplerState RadiositySampler;

#endif 

float4x4 InvViewProj;

float3 CameraPosition; 

float3 LightDirection;
float3 LightColor;

float2 InvViewport;

float PropagationDamping;

//////////////////////////////////////////////////////////////////////////////////////
float3 ReconstructeWorldPosition(Texture2D depthBuffer, float2 fragCoord)
{
	float z = depthBuffer.Load(int3(fragCoord, 0)).r; 
	
	float2 clipPos = (fragCoord * InvViewport);
	clipPos = clipPos * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	
	float4 worldPosition = mul(float4(clipPos, z, 1.0), InvViewProj);
	return float3(worldPosition.xyz / worldPosition.w); 
}

void LambertianOnly(in float4 iFragCoord : SV_Position,
					out float3 oFragColor : SV_Target0)
{
	int3 ssp = int3(iFragCoord.xy, 0);

	float3 N = GBufferNormal.Load(ssp).xyz;
    if (dot(N, N) < 0.01) {
        // This is a background pixel, not part of an object
        discard;
    } 

	float3 worldPosition = ReconstructeWorldPosition(GBufferDepth, iFragCoord.xy);
	float3 V = normalize(CameraPosition - worldPosition);
	float3 L = normalize(-LightDirection);

	// Lambertian coefficient in BSDF
    float3 diffuseAlbedo  = (GBufferLambertain.Load(ssp).rgb); // / PI;

	// Direct light
	float3 E_lambertian = LightColor * max(0.0f, dot(N, L));

#ifdef USE_INDIRECT
	
    float2 prevFragCoord  = iFragCoord.xy - GBufferSSVelocity.Load(ssp).rg;
    float3 indirect = PrevIndirectRadiosityBuffer.Sample(RadiositySampler, prevFragCoord * InvViewport).rgb;
    
    float epsilon = 0.05;
    float3 prevWorldPosition = ReconstructeWorldPosition(PrevDepthBuffer, prevFragCoord);
    float dist = length(worldPosition - prevWorldPosition);

    float weight = 1.0 - smoothstep(epsilon * 0.8, epsilon * 1.2, dist);
    indirect *= weight;
    E_lambertian += indirect * (1.0 - PropagationDamping) /** colorBoost(indirect, unsaturatedLightBoost, saturatedLightBoost)*/;

#endif

	oFragColor = diffuseAlbedo * E_lambertian;
}
