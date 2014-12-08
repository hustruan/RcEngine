#ifndef Math_hlsl__
#define Math_hlsl__

/// Compute Schlick's approximation of the Fresnel coefficient.
float3 ComputeFresnel(float3 F0, float cos_i)
{
	return lerp(F0, (float3)1.0, pow(1.0 - max(0.0, cos_i), 5.0));
}




#endif //Math_hlsl__