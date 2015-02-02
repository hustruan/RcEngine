#ifndef LIGHT_COMMON_HLSL__
#define LIGHT_COMMON_HLSL__

#include "BRDF.hlsl"

float3 SimplePointLightDiffuse( float3 DiffuseColor )
{
	return Diffuse_Lambert(DiffuseColor);
}

float3 SimplePointLightSpecular(float3 SpecularColor, float Roughness, float3 L, float3 V, half3 N )
{
	float3 H = normalize(V + L);
	float NoL = saturate( dot(N, L) );
	float NoV = saturate( dot(N, V) );
	float NoH = saturate( dot(N, H) );
	float VoH = saturate( dot(V, H) );

	// Generalized microfacet specular
	float  D = Distribution( Roughness, NoH ) / PI;
	float  G = GeometricVisibility( Roughness, NoV, NoL, VoH, L, V );
	float3 F = Fresnel( SpecularColor, VoH );

	return (D * G) * F;
}

/** 
 * Calculates lighting for a given position, normal, etc with a simple lighting model designed for speed. 
 * All lights rendered through this method are unshadowed point lights with no shadowing or light function or IES.
 * A cheap Blinn specular is used instead of the more correct area specular, no fresnel.
 */
float3 GetSimpleDynamicLighting(float3 WorldPosition, float3 CameraVector, FScreenSpaceData ScreenSpaceData, FSimpleDeferredLightData LightData)
{
	float3 N = ScreenSpaceData.GBuffer.WorldNormal;
	float3 L = LightData.LightPositionAndInvRadius.xyz - WorldPosition;
	float3 UnitL = normalize(L);
	float NoL = BiasedNDotL( dot( N, UnitL ) );
	float DistanceAttenuation = 1;

	if (LightData.bInverseSquared)
	{
		float DistanceSqr = dot( L, L );

		// Sphere falloff (technically just 1/d2 but this avoids inf)
		DistanceAttenuation = 1 / ( DistanceSqr + 1 );

		// Correction for lumen units
		DistanceAttenuation *= 16;
	
		float LightRadiusMask = Square( saturate( 1 - Square( DistanceSqr * Square(LightData.LightPositionAndInvRadius.w) ) ) );
		DistanceAttenuation *= LightRadiusMask;
	}
	else
	{
		DistanceAttenuation = RadialAttenuation(L * LightData.LightPositionAndInvRadius.w, LightData.LightColorAndFalloffExponent.w);
	}

	float3 OutLighting = 0;

	BRANCH
	if (DistanceAttenuation > 0)
	{
		float3 DiffuseLighting = SimplePointLightDiffuse(ScreenSpaceData);
		float3 SpecularLighting = SimplePointLightSpecular(ScreenSpaceData, UnitL, -CameraVector, N);
		float3 CombinedAttenuation = DistanceAttenuation * LightData.LightColorAndFalloffExponent.rgb;
		// Apply SSAO to the direct lighting since we're not going to have any other shadowing
		CombinedAttenuation *= ScreenSpaceData.AmbientOcclusion;

		OutLighting += NoL * (DiffuseLighting + SpecularLighting) * CombinedAttenuation;
	}

	return OutLighting;
}


#endif //LIGHT_COMMON_HLSL__