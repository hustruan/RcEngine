
/** Increase to make depth edges crisper. Decrease to reduce flicker. */
#define EDGE_SHARPNESS     (1.0)

/** Step in 2-pixel intervals since we already blurred against neighbors in the
    first AO pass.  This constant can be increased while R decreases to improve
    performance at the expense of some dithering artifacts. 
    
    Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
    unobjectionable after shading was applied but eliminated most temporal incoherence
    from using small numbers of sample taps.
    */
#define SCALE               (1)

/** Filter radius in pixels. This will be multiplied by SCALE. */
#define R                   (5)

/** Uniforms */
Texture2D CSZBuffer;
Texture2D SourceBuffer;

#if USE_NORMAL
	Texture2D NormalBuffer;
#endif

float4 ProjInfo;
int2 Axis;

/** Camera far plane */
#define FAR_PLANE_Z (150.0)

/** Type of data to read from source.  This macro allows
    the same blur shader to be used on different kinds of input data. */
#define VALUE_TYPE        float4

/** Swizzle to use to extract the channels of source. This macro allows
    the same blur shader to be used on different kinds of input data. */
#define VALUE_COMPONENTS   rgba


float getKey(int2 ssP)
{	
	float key = CSZBuffer.Load(int3(ssP, 0)).r;
    key = clamp(key * (1.0 / FAR_PLANE_Z), 0.0, 1.0);
    return key;
}

#if USE_NORMAL
	float3 sampleNormal(int2 ssP)  
	{
		return normalize( NormalBuffer.Load(int3(ssP, 0)).rgb * 2.0 - 1.0 );
	}
#endif

float3 reconstructCameraSpacePosition(float2 S, float z)
{
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * z, z);
}

float3 positionFromKey(float key, int2 ssC)
{
    float z = key * FAR_PLANE_Z;
    return reconstructCameraSpacePosition(float2(ssC) + float2(0.5, 0.5), z);    
}

float calculateBilateralWeight(float key, float tapKey, int2 tapLoc, float3 n_C, float3 C) 
{
    // range domain (the "bilateral" weight). As depth difference increases, decrease weight.
    float depthWeight = max(0.0, 1.0
            - (EDGE_SHARPNESS * 2000.0) * abs(tapKey - key)
            );

    float k_normal = 40.0;
    float k_plane = 0.5;

    // Prevents blending over creases. 
    float normalWeight = 1000.0;
    float planeWeight = 1.0;

#if USE_NORMAL
        float3 tapN_C = sampleNormal(tapLoc);
        depthWeight = 1.0;

        float normalError = (1.0 - dot(tapN_C, n_C)) * k_normal;
        normalWeight = max(1.0 - EDGE_SHARPNESS*normalError, 0.00);
        
        float lowDistanceThreshold2 = 0.01;

        float3 tapC = positionFromKey(tapKey, tapLoc);

        // Change in position in camera space
        float3 dq = C - tapC;
                
        // How far away is this point from the original sample
        // in camera space? (Max value is unbounded)
        float distance2 = dot(dq, dq);
                
        // How far off the expected plane (on the perpendicular) is this point?  Max value is unbounded.
        float planeError = max(abs(dot(dq, tapN_C)), abs(dot(dq, n_C)));

        planeWeight = (distance2 < lowDistanceThreshold2) ? 1.0 :
                         pow(max(0.0, 1.0 - EDGE_SHARPNESS * 2.0 * k_plane * planeError / sqrt(distance2)), 2.0);

#   endif

    return depthWeight * normalWeight * planeWeight;
}

void DeepGBufferBlur(in float2 iTex	   : TEXCOORD0,
				     in float4 iFragCoord : SV_Position,
				     out float4 oFragColor : SV_Target0)
{
	    float kernel[R + 1];
//      if R == 0, we never call this shader
#       if R == 1 
            kernel[0] = 0.5; kernel[1] = 0.25;
#       elif R == 2 
            kernel[0] = 0.153170; kernel[1] = 0.144893; kernel[2] = 0.122649;
#       elif R == 3 
            kernel[0] = 0.153170; kernel[1] = 0.144893; kernel[2] = 0.122649; kernel[3] = 0.092902;
#       elif R == 4 
            kernel[0] = 0.153170; kernel[1] = 0.144893; kernel[2] = 0.122649; kernel[3] = 0.092902; kernel[4] = 0.062970;
#       elif R == 5 
            kernel[0] = 0.111220; kernel[1] = 0.107798; kernel[2] = 0.098151; kernel[3] = 0.083953; kernel[4] = 0.067458; kernel[5] = 0.050920;
#       elif R == 6
            kernel[0] = 0.111220; kernel[1] = 0.107798; kernel[2] = 0.098151; kernel[3] = 0.083953; kernel[4] = 0.067458; kernel[5] = 0.050920; kernel[6] = 0.036108;
#       endif

	int2 ssC = int2(iFragCoord.xy);

    float key = getKey(ssC);
	float4 temp = SourceBuffer.Load(int3(ssC, 0));
    
	VALUE_TYPE sum = temp.VALUE_COMPONENTS;

	if (key == 1.0) { 
        // Sky pixel (if you aren't using depth keying, disable this test)
        oFragColor = sum;
        return;
    }

	// Base weight for falloff.  Increase this for more blurriness,
    // decrease it for better edge discrimination
    float BASE = kernel[0];
    float totalWeight = BASE;
    sum *= totalWeight;

	float3 n_C;
#if USE_NORMAL
    n_C = sampleNormal(ssC);
#endif

	float3 C = positionFromKey(key, ssC);

	for (int r = -R; r <= R; ++r) 
	{
		// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
		// so the IF statement has no runtime cost
		if (r != 0) 
		{
			float2 tapLoc = ssC + Axis * (r * SCALE);
			temp = SourceBuffer.Load(int3(tapLoc, 0)); 
            
			float tapKey = getKey(tapLoc);
			VALUE_TYPE value = temp.VALUE_COMPONENTS;
            
			// spatial domain: offset kernel tap
			float weight = 0.3 + kernel[abs(r)];   
			float bilateralWeight = calculateBilateralWeight(key, tapKey, tapLoc, n_C, C);

			weight *= bilateralWeight;
			sum += value * weight;
			totalWeight += weight;
		}
	}

	const float epsilon = 0.0001;
    oFragColor = sum / (totalWeight + epsilon);	
}