#include "LightingUtil.hlsl"

float4x4 WorldViewProj;
float4x4 InvViewProj;

cbuffer PerLight{

	float4 LightPos;		// w dimension is spot light inner cone cos angle
	float4 LightDir;		// w dimension is spot light outer cone cos angle
	float3 LightColor;
	float3 LightFalloff; 
};

float3 CameraOrigin;

Texture2D GBuffer0;	
Texture2D GBuffer1;	
Texture2D<float> DepthBuffer;	
Texture2D LightAccumulateBuffer;

//-------------------------------------------------------------------------------------
// Help function
void GetNormalAndShininess(in int3 texelPos, out float3 oNormal, out float oShininess)
{
	float4 tap = GBuffer0.Load(texelPos);

	// RGBA16F 
	//oNormal = normalize(tap.xyz * 2.0 - 1.0);	 // World Space Normal
	oNormal = normalize(tap.xyz);
	oShininess = tap.w;
}

void GetDiffuseAndSpecular(in int3 texelPos, out float3 oDiffuse, out float3 oSpecular)
{
	float4 tap = GBuffer1.Load(texelPos);
	oDiffuse = tap.rgb;
	oSpecular = tap.aaa;
}

float3 ReconstructWorldPosition(in int3 texelPos, in float4 clipPos)
{
	float zBuffer = DepthBuffer.Load(texelPos);
	float4 worldPositionH = mul( float4(clipPos.xy / clipPos.w, zBuffer, 1.0), InvViewProj );
	return float3(worldPositionH.xyz / worldPositionH.w);
}


//---------------------------------------------------------------
// Directional light
void DirectionalVSMain(uint iVertexID        : SV_VertexID,
					   out float4 oPosCS    : TEXCOORD0,
					   out float4 oPosCSD3D : SV_POSITION) 
{
	float2 grid = float2((iVertexID << 1) & 2, iVertexID & 2);
	float2 ndcXY = grid * float2(2.0, -2.0) + float2(-1.0, 1.0);

	oPosCSD3D = oPosCS = float4(ndcXY, 0.0, 1.0);
}

//-----------------------------------------------------------------
// Spot or Point light
void LightVolumeVSMain(in float3 iPos       : POSITION,
					   out float4 oPosCS    : TEXCOORD0,
					   out float4 oPosCSD3D : SV_POSITION) 
{
	oPosCSD3D = oPosCS = mul( float4(iPos, 1.0), WorldViewProj );
}


//---------------------------------------------------------------------
// Deferred Directional Light
void DirectionalPSMain(
	in float4 iPosCS	  : TEXCOORD0,
	in float4 iFragCoord  : SV_POSITION,
	out float4 oFragColor : SV_Target0 )
{
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	float3 worldPosition = ReconstructWorldPosition(sampleIndex, iPosCS);

	// Decode normal and shininess from GBuffer
	float3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);

	// Light accumulate in HDR Buffer
	oFragColor = 0;

	float3 L = normalize(-LightDir.xyz);
	float nDotl = dot(L, N);
	if (nDotl > 0.0)
	{
		float3 V = normalize(CameraOrigin - worldPosition);
		float3 H = normalize(V + L);
		
		float3 diffuse = LightColor * nDotl;
		
		// Frensel in moved to calculate in shading pass
		float3 specular = CalculateSpecular(N, H, shininess) * LightColor * nDotl;
		
		oFragColor = float4(diffuse, Luminance(specular));
	}
}

//---------------------------------------------------------------------
// Deferred Point Light
void PointLightingPSMain(
	in float4 iPosCS	  : TEXCOORD0,
	in float4 iFragCoord  : SV_POSITION,
	out float4 oFragColor : SV_Target0 )
{
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	float3 worldPosition = ReconstructWorldPosition(sampleIndex, iPosCS);
	float3 L = normalize(LightPos.xyz - worldPosition);
	
	// Decode normal and shininess from GBuffer
	float3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);
	
	// Light accumulate in HDR Buffer
	oFragColor = 0;

	float NdotL = dot(L, N);
	if (NdotL > 0.0)
	{
		float3 V = normalize(CameraOrigin - worldPosition);
		float3 H = normalize(V + L);
		
		// calculate attenuation
		float attenuation = CalcAttenuation(LightPos.xyz, worldPosition, LightFalloff);
		
		float3 diffuseLight = LightColor * NdotL * attenuation;	
		float3 specularLight = CalculateSpecular(N, H, shininess) * diffuseLight; // Frensel in moved to calculate in shading pass
		
		oFragColor = float4(diffuseLight, Luminance(specularLight));
	}
}


//---------------------------------------------------------------------
// Deferred Spot Light
void SpotLightingPSMain(
	in float4 iPosCS	  : TEXCOORD0,
	in float4 iFragCoord  : SV_POSITION,
	out float4 oFragColor : SV_Target0 )
{
	// Light accumulate in HDR Buffer
	oFragColor = 0;

	int3 sampleIndex = int3(iFragCoord.xy, 0);

	float3 worldPosition = ReconstructWorldPosition(sampleIndex, iPosCS);
	float3 L = normalize(LightPos.xyz - worldPosition);

	// Decode normal and shininess from GBuffer
	float3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);

	float spot = SpotLighting(L, LightDir.xyz, float2(LightPos.w, LightDir.w));

	if(spot > 0.0)
	{
		float NdotL = dot(L, N);
		if (NdotL > 0.0)
		{
			float3 V = normalize(CameraOrigin - worldPosition);
			float3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = spot * CalcAttenuation(LightPos.xyz, worldPosition, LightFalloff);
		
			float3 diffuse = LightColor * NdotL * attenuation;
		
			// Frensel in moved to calculate in shading pass
			float3 specular = CalculateSpecular(N, H, shininess) * LightColor * NdotL * attenuation;
		
			oFragColor = float4(diffuse, Luminance(specular));
		}
	}
}

//--------------------------------------------------------
// Final Shading Pass
void DeferredShadingPSMain(
	in float3 iViewRay	  : TEXCOORD0,
	in float4 iFragCoord  : SV_POSITION,
	out float4 oFragColor : SV_Target0 )
{
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	float3 V = normalize(-iViewRay);

	// Decode normal and shininess from GBuffer
	float3 N, diffuseAlbedo, specularAlbedo;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);
	GetDiffuseAndSpecular(sampleIndex, diffuseAlbedo, specularAlbedo);
	
	float4 lightColor = LightAccumulateBuffer.Load(sampleIndex);
	                      
	float3 diffueLight = lightColor.rgb;
	float3 specularLight = lightColor.a / (Luminance(diffueLight) + 1e-6) * diffueLight;

	// Approximate fresnel by N and V
	float3 fresnelTerm = CalculateAmbiemtFresnel(specularAlbedo, N, V);
	
	float3 final = 0;

	final =  diffueLight * diffuseAlbedo + ((shininess + 2.0) / 8.0) * fresnelTerm * specularLight;
	final += float3(0.1, 0.1, 0.1) * diffuseAlbedo;

	oFragColor = float4(final, 1.0);
}

//-------------------------------------------------------------------------------------------
// Copy Depth Buffer
float CopyDepthPSMain(in float4 iFragCoord  : SV_POSITION) : SV_Depth
{
	return DepthBuffer.Load(int3(iFragCoord.xy, 0));
}