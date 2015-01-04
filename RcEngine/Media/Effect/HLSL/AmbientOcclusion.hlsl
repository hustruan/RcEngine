
// Total number of direct samples to take at each pixel
#define NUM_SAMPLES (9)

// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower 
// miplevel to maintain reasonable spatial locality in the cache
// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
#define LOG_MAX_OFFSET 3

// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
#define MAX_MIP_LEVEL 5

/** Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling. 
    This need not match the real far plane*/
#define FAR_PLANE_Z (300.0)

// This is the number of turns around the circle that the spiral pattern makes.  This should be prime to prevent
// taps from lining up.  This particular choice was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS (7)

// Mipmaped camera space zbuffer
Texture2D CameraSpaceZBuffer;

cbuffer AOParams
{
	// (2.0 / (width * P[0][0]), -2.0 / (height * P[1][1]), -1.0 / P[0][0], 1.0 / P[1][1])
	float4 ProjInfo ;

	float ProjScale ;

	// World space sample radius
	float Radius	;
	float Radius2   ;
	float InvRadius2 ;

	/** Bias to avoid AO in smooth corners, e.g., 0.01m */
	float Bias	    ;

	/** Darkending factor, e.g., 1.0 */
	float IntensityDivR6 ;
};

float3 reconstructCameraSpacePosition(float2 S, float z)
{
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * z, z);
}

float3 getPosition(int2 ssp)
{
	float z = CameraSpaceZBuffer.Load(int3(ssp, 0)).r;	
	return reconstructCameraSpacePosition(float2(ssp) + float2(0.5, 0.5), z);
}

float fallOffFunction(float vv, float vn, float epsilon)
{
    // A: From the HPG12 paper
    // Note large epsilon to avoid overdarkening within cracks
    return float(vv < Radius2) * max((vn - Bias) / (epsilon + vv), 0.0) * Radius2 * 0.6;

    // B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
    //float f = Radius2 - vv; return f * f * f * max((vn - Bias) / (epsilon + vv), 0.0);

    // C: Medium contrast (which looks better at high radii), no division.  Note that the 
    // contribution still falls off with Radius2^2, but we've adjusted the rate in a way that is
    // more computationally efficient and happens to be aesthetically pleasing.
    // return 4.0 * max(1.0 - vv * InvRadius2, 0.0) * max(vn - Bias, 0.0);

    // D: Low contrast, no division operation
    // return 2.0 * float(vv < Radius * Radius) * max(vn - Bias, 0.0);
}


/** Compute the occlusion due to sample point \a Q about camera-space point \a C with unit normal \a n_C */
float aoValueFromPositionsAndNormal(float3 C, float3 n_C, float3 Q)
{
    float3 v = Q - C;
    float vv = dot(v, v);
    float vn = dot(v, n_C);
    const float epsilon = 0.001;
   
	return fallOffFunction(vv, vn, epsilon);
}

/** Returns a unit vector and a screen-space radius for the tap on a unit disk (the caller should scale by the actual disk radius) */
float2 tapLocation(int sampleNumber, float spinAngle, out float ssR)
{
    // Radius relative to ssR
    float alpha = float(sampleNumber + 0.5) * (1.0 / NUM_SAMPLES);
    float angle = alpha * (NUM_SPIRAL_TURNS * 6.28) + spinAngle;

    ssR = alpha;
    return float2(cos(angle), sin(angle));
}

/** Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.  Assumes length(unitOffset) == 1 */
float3 getOffsetPosition(int2 ssC, float2 unitOffset, float ssR) 
{
	// Derivation:
	//  mipLevel = floor(log(ssR / MAX_OFFSET));

	int mipLevel = clamp(int(floor(log2(ssR))) - LOG_MAX_OFFSET, 0, MAX_MIP_LEVEL);
	int2 ssP = int2(ssR*unitOffset) + ssC;

	float3 P;

	// Divide coordinate by 2^mipLevel
	P.z = CameraSpaceZBuffer.Load(int3(ssP >> mipLevel, mipLevel)).r;

	// Offset to pixel center
	P = reconstructCameraSpacePosition(float2(ssP) + float2(0.5, 0.5), P.z);

	return P;
}


/** Compute the occlusion due to sample with index \a i about the pixel at \a ssC that corresponds
    to camera-space point \a C with unit normal \a n_C, using maximum screen-space sampling radius \a ssDiskRadius 

    Note that units of H() in the HPG12 paper are meters, not
    unitless.  The whole falloff/sampling function is therefore
    unitless.  In this implementation, we factor out (9 / radius).

    Four versions of the falloff function are implemented below

    When sampling from the peeled depth buffer, make sure ssDiskRadius has been premultiplied by cszBufferScale
*/
float sampleAO(int2 ssC, float3 C, float3 n_C, float ssDiskRadius, int tapIndex, float randomPatternRotationAngle)
{
    // Offset on the unit disk, spun for this pixel
    float ssR;
    float2 unitOffset = tapLocation(tapIndex, randomPatternRotationAngle, ssR);
    ssR *= ssDiskRadius;

	// The occluding point in camera space
    float3 Q = getOffsetPosition(ssC, unitOffset, ssR);
    return aoValueFromPositionsAndNormal(C, n_C, Q);
}

void AlchemyAmbientObsurance(in float2 iTex			: TEXCOORD0,
							 in float4 iFragCoord   : SV_Position,
							 out float oVisibility  : SV_Target0)
{
	int2 ssC = int2(iFragCoord.xy);

	float3 C = getPosition(ssC);
	float3 n_C = cross(ddx(C), ddy(C));
	// Since n_C is computed from the cross product of cmaera-space edge vectors from points at adjacent pixels, its magnitude will be proportional to the square of distance from the camera
    if (dot(n_C, n_C) > (pow(C.z*C.z * 0.00006, 2.0)))
	{ 
		// if the threshold # is too big you will see black dots where we used a bad normal at edges, too small -> white
        // The normals from depth should be very small values before normalization,
        // except at depth discontinuities, where they will be large and lead
        // to 1-pixel false occlusions because they are not reliable
		oVisibility = 1.0; 
        return;
    }
	else 
	{
		n_C = normalize(n_C);
	}

	// Hash function used in the HPG12 AlchemyAO paper
    float randomPatternRotationAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;
    
    // Choose the screen-space sample radius
    // proportional to the projected area of the sphere
    float ssDiskRadius = ProjScale * Radius / C.z;
	
	float sum = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i)
		sum += sampleAO(ssC, C, n_C, ssDiskRadius, i, randomPatternRotationAngle);

    	
	float A = max(0.0, 1.0 - sum * IntensityDivR6 * (5.0 / NUM_SAMPLES));

    // Anti-tone map to reduce contrast and drag dark region farther
    // (x^0.2 + 1.2 * x^4)/2.2
    A = (pow(A, 0.2) + 1.2 * A*A*A*A) / 2.2;

    // See random spin distribution
    //A = randomPatternRotationAngle / (2 * 3.141592653589);
    //A = mod(A, 1.0);

	oVisibility = A;
}
