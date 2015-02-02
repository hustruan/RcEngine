// Total number of direct samples to take at each pixel
#define NUM_SAMPLES 50

// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower 
// miplevel to maintain reasonable spatial locality in the cache
// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
#define LOG_MAX_OFFSET 4

// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
#define MIN_MIP_LEVEL 2

// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
#define MAX_MIP_LEVEL 5

// This is the number of turns around the circle that the spiral pattern makes.  This should be prime to prevent
// taps from lining up.  This particular choice was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS 19

#define PI 3.141592653589

Texture2D CSZBuffer;
Texture2D NormalBuffer;
Texture2D BounceBuffer;

#if USE_DEPTH_PEEL
    Texture2D PeeledBounceBuffer;
	#if USE_OCT16 
		#include "oct.hlsl"
	#else
		Texture2D PeeledNormalBuffer;
	#endif
#endif

float4 ProjInfo;
int4 TextureBound; // (0, 0, W-1, H-1)
float Radius;
float Radius2;
float ProjScale;
float GameTime;

#define USE_MIPMAPS 1

float3 reconstructCameraSpacePosition(float2 S, float z)
{
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * z, z);
}

float3 sampleNormal(Texture2D normalBuffer, int2 ssC, int mipLevel)
{
#if USE_OCT16
    return decode16(normalBuffer.Load(int3(ssC, mipLevel)).xy * 2.0 - 1.0);
#else
    return normalBuffer.Load(int3(ssC, mipLevel)).xyz * 2.0 - 1.0;
#endif
}

float3 getPosition(int2 ssp, int2 texel, int mipLevel)
{
	float z = CSZBuffer.Load(int3(texel, mipLevel)).r;	
	return reconstructCameraSpacePosition(float2(ssp) + float2(0.5, 0.5), z);
}

/** Returns a unit vector and a screen-space radius for the tap on a unit disk (the caller should scale by the actual disk radius) */
float2 tapLocation(int sampleNumber, float spinAngle, float radialJitter, out float ssR)
{
    // Radius relative to ssR
    float alpha = float(sampleNumber + radialJitter) * (1.0 / NUM_SAMPLES);
    float angle = alpha * (NUM_SPIRAL_TURNS * 6.28) + spinAngle;

    ssR = alpha;
    return float2(cos(angle), sin(angle));
}

void computeMipInfo(float ssR, int2 ssP, out int mipLevel, out int2 mipP)
{
    // Derivation:
    //  mipLevel = floor(log(ssR / MAX_OFFSET));
	mipLevel = clamp(int(floor(log2(ssR))) - LOG_MAX_OFFSET, MIN_MIP_LEVEL, MAX_MIP_LEVEL);

    // We need to divide by 2^mipLevel to read the appropriately scaled coordinate from a MIP-map.  
    // Manually clamp to the texture size because texelFetch bypasses the texture unit
    mipP = ssP >> mipLevel;//clamp(ssP >> mipLevel, ivec2(0), textureSize(CS_Z_buffer, mipLevel) - ivec2(1));
}

void iiValueFromPositionsAndNormalsAndLambertian(int2 ssP, float3 X, float3 n_X, float3 Y, float3 n_Y, float3 radiosity_Y, out float3 E, out float weight_Y) 
{
   
    float3 YminusX = Y - X;
    float3 w_i = normalize(YminusX);
    weight_Y = ((dot(w_i, n_X) > 0.0) && (dot(-w_i, n_Y) > 0.01)) ? 1.0 : 0.0; // Backface check

    // E = radiosity_Y * dot(w_i, n_X) * weight_Y * float(dot(YminusX, YminusX) < radius2);    
    if ((dot(YminusX, YminusX) < Radius2) && (weight_Y > 0)) { 
        E = radiosity_Y * dot(w_i, n_X);
    } else {
        E = (float3)0.0;
	}
}

// Get normal and lambertain of sample
void getOffsetPositionNormalAndLambertian(int2 ssP, float ssR, out float3 Q, out float3 N, out float3 lambertian)
{
	int mipLevel;
	int2 texel;

#if USE_MIPMAPS
	computeMipInfo(ssR, ssP, mipLevel, texel);
#else 
	mipLevel = 0;
	texel = ssP;
#endif

	Q = getPosition(ssP, texel, mipLevel); 
    N = sampleNormal(NormalBuffer, texel, mipLevel);
    lambertian = BounceBuffer.Load(int3(texel, mipLevel)).rgb;
}

#if USE_DEPTH_PEEL
// Get normal and lambertain of sample / peeled sample	
void getOffsetPositionsNormalsAndLambertians(int2 ssP, float ssR, 
											out float3 Q0, out float3 N0, out float3 lambertian0, 
											out float3 Q1, out float3 N1, out float3 lambertian1)
{
	int mipLevel;
	int2 texel;

#if USE_MIPMAPS
	computeMipInfo(ssR, ssP, mipLevel, texel);
#else 
	mipLevel = 0;
	texel = ssP;
#endif

	// Get sample normals
#if USE_OCT16
	float4 encodedNormals = NormalBuffer.Load(int3(texel, mipLevel)) * 2.0 - 1.0;
	N0 = decode16(encodedNormals.xy);
	N1 = decode16(encodedNormals.zw);
#else
	N0 = sampleNormal(NormalBuffer, texel, mipLevel);
	N1 = sampleNormal(PeeledNormalBuffer, texel, mipLevel);
#endif
		
	lambertian0 = BounceBuffer.Load(int3(texel, mipLevel)).rgb;
	lambertian1 = PeeledBounceBuffer.Load(int3(texel, mipLevel)).rgb;

	// Get sample camera space position
	float2 Zs = CSZBuffer.Load(int3(texel, mipLevel)).rg;	
	Q0 = reconstructCameraSpacePosition((float2(ssP) + float2(0.5, 0.5)), Zs.x);
	Q1 = reconstructCameraSpacePosition((float2(ssP) + float2(0.5, 0.5)), Zs.y);
}
#endif 

void sampleIndirectLight(int2 ssC, float3 C, float3 n_C, float ssDiskRadius, int tapIndex,
						 float randomPatternRotationAngle, float radialJitter, 
						 inout float3 irradianceSum, inout float numSamplesUsed)
{
	 // Offset on the unit disk, spun for this pixel
    float ssR;
    float2 unitOffset = tapLocation(tapIndex, randomPatternRotationAngle, radialJitter, ssR);
    ssR *= ssDiskRadius;
    int2 ssP = int2(ssR * unitOffset) + ssC;

#   if USE_DEPTH_PEEL
        float3 E, ii_tap0, ii_tap1;
        float weight, weight0, weight1;

        // The occluding point in camera space
        float3 Q0, lambertian_tap0, n_tap0, Q1, lambertian_tap1, n_tap1;
		getOffsetPositionsNormalsAndLambertians(ssP, ssR, Q0, n_tap0, lambertian_tap0, Q1, n_tap1, lambertian_tap1);

		iiValueFromPositionsAndNormalsAndLambertian(ssP, C, n_C, Q0, n_tap0, lambertian_tap0, ii_tap0, weight0);
        float adjustedWeight0 = weight0 * dot(ii_tap0, ii_tap0) + weight0;

        iiValueFromPositionsAndNormalsAndLambertian(ssP, C, n_C, Q1, n_tap1, lambertian_tap1, ii_tap1, weight1);
        float adjustedWeight1 = weight1 * dot(ii_tap1, ii_tap1) + weight1;

        weight = (adjustedWeight0 > adjustedWeight1) ? weight0 : weight1;
        E = (adjustedWeight0 > adjustedWeight1) ? ii_tap0 : ii_tap1;

		//weight = weight0 + min(0, weight1);
		//E = ii_tap0 + min((float3)0, ii_tap1);
        numSamplesUsed += weight;

#   else

        float3 E;
        float weight_Y;
        // The occluding point in camera space
        float3 Q, lambertian_tap, n_tap;
        getOffsetPositionNormalAndLambertian(ssP, ssR, Q, lambertian_tap, n_tap);
        iiValueFromPositionsAndNormalsAndLambertian(ssP, C, n_C, Q, n_tap, lambertian_tap, E, weight_Y);
        numSamplesUsed += weight_Y;

#   endif

    irradianceSum           += E;
}

void DeepGBufferRadiosity(in float2 iTex	   : TEXCOORD0,
						  in float4 iFragCoord : SV_Position,
						  out float4 oFragColor : SV_Target0)
{
	int2 ssC = int2(iFragCoord.xy);
	
    float3 C = getPosition(ssC, ssC, 0);
	float3 n_C = sampleNormal(NormalBuffer, ssC, 0);

	// Choose the screen-space sample radius
    // proportional to the projected area of the sphere
    float ssDiskRadius = ProjScale * Radius / C.z;

	// Hash function used in the HPG12 AlchemyAO paper
    float randomPatternRotationAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;
	float radialJitter = frac(sin(iFragCoord.x * 1e2 + iFragCoord.y) * 1e5 + sin(iFragCoord.y * 1e3) * 1e3) * 0.8 + 0.1;

	float numSamplesUsed = 0.0;
    float3 irradianceSum = 0.0; 
    for (int i = 0; i < NUM_SAMPLES; ++i)
        sampleIndirectLight(ssC, C, n_C, ssDiskRadius, i, randomPatternRotationAngle, radialJitter, irradianceSum, numSamplesUsed);

	const float solidAngleHemisphere = 2 * PI;
    float3 E_X = irradianceSum * solidAngleHemisphere / (numSamplesUsed + 0.00001);

    // What is the ambient visibility of this location
    float visibility = 1 - numSamplesUsed / float(NUM_SAMPLES);

	//oFragColor = float4(E_X, visibility);
	oFragColor = float4(E_X.rg, visibility, visibility);
}