#define MAX_CASCADE_COUNT 4
	
Texture2DArray CascadeShadowTex;
SamplerState ShadowSampler;

int NumCascades;
float4x4 ShadowView;
float4 CascadeScale[MAX_CASCADE_COUNT];   // Shadow projection info
float4 CascadeOffset[MAX_CASCADE_COUNT];	
float2 BorderPaddingMinMax;				 // For map based selection, this keep pixels in valid range.
float CascadeBlendArea;  				 // Amount to overlap when blending between cascades.

/**
 * Calculate blend weight for cascade boundary pixels.
 * @param blendArea, blend width.
 * @param blendBandLocation, current pixel's location in blend band.
 * @param cascadeBlendWeight, calculated blend weight, [1, 0] -> [width, 0].
 */ 
void CalculateBlendAmountForMap(in float2 CascadeShadowTexCoord, in float blendArea, inout float blendBandLocation, out float cascadeBlendWeight)
{
	float2 distanceToOne = float2(1.0 - CascadeShadowTexCoord.x, 1.0 - CascadeShadowTexCoord.y);
    blendBandLocation = min(CascadeShadowTexCoord.x, CascadeShadowTexCoord.y);
    blendBandLocation = min( blendBandLocation, min(distanceToOne.x, distanceToOne.y) );
    cascadeBlendWeight = blendBandLocation / blendArea;
}

float ChebyshevUpperBound(float2 moments, float depth, float minVariance)
{
	float mean = moments.x;
	float measSquared = moments.y;
	
	float p = float(depth <= mean);
	
	float variance = max(minVariance, measSquared - mean*mean);
	float d = depth - mean;
	float pmax = variance / (variance + d * d);
	
	 // To combat light-bleeding, experiment with raising p_max to some power
     // (Try values from 0.1 to 100.0, if you like.)	
	return pow( max(p, pmax), 100.0 );
}

float CalculateVarianceShadow(in float4 CascadeShadowTexCoord, in float4 posShadowViewSpace, in int iCascade)
{
	float4 CascadeShadowTexCoordDDX = ddx(posShadowViewSpace) * CascadeScale[iCascade];
	float4 CascadeShadowTexCoordDDY = ddy(posShadowViewSpace) * CascadeScale[iCascade];
	
	//float2 moments = CascadeShadowTex.SampleGrad(ShadowSampler, CascadeShadowTexCoord.xyz, CascadeShadowTexCoordDDX.xy, CascadeShadowTexCoordDDY.xy).rg;

	float2 moments = CascadeShadowTex.SampleLevel(ShadowSampler, CascadeShadowTexCoord.xyz, 0).rg;

	return ChebyshevUpperBound(moments, CascadeShadowTexCoord.w, 0.0001);
}

float CalculateVarianceShadowWrong(in float4 CascadeShadowTexCoord, in float4 posShadowViewSpace, in int iCascade)
{
	float2 moments = CascadeShadowTex.Sample(ShadowSampler, CascadeShadowTexCoord.xyz).rg;
	return ChebyshevUpperBound(moments, CascadeShadowTexCoord.w, 0.0001);
}

float EvalCascadeShadow(in float4 posWorldSpace, out int selectCascade)
{
	float percentLit = 0.0;

	float4 CascadeShadowTexCoord = (float4)0.0;
	float4 CascadeShadowTexCoordBlend = (float4)0.0;
	
	// Compute view space position of shadow
	float4 posShadowVS = mul(posWorldSpace, ShadowView);
		
	int iCascadeSelected = 0;
	int cascadeFound = 0;
	for( int iCascade = 0; iCascade < NumCascades && cascadeFound == 0; ++iCascade ) 
	{
		CascadeShadowTexCoord = posShadowVS * CascadeScale[iCascade] + CascadeOffset[iCascade];		

		if ( min( CascadeShadowTexCoord.x, CascadeShadowTexCoord.y ) > BorderPaddingMinMax.x && 
             max( CascadeShadowTexCoord.x, CascadeShadowTexCoord.y ) < BorderPaddingMinMax.y )
        { 
            iCascadeSelected = iCascade;   
            cascadeFound = 1; 
        }		
	}
	
	// Store selected cascade index in z
	CascadeShadowTexCoord.w = CascadeShadowTexCoord.z;
	CascadeShadowTexCoord.z = (float)iCascadeSelected;
	
	percentLit = CalculateVarianceShadow(CascadeShadowTexCoord,  posShadowVS, iCascadeSelected);	
	
	/*
	// Blend between cascades
	int iNextCascadeIndex = min ( NumCascades - 1, iCascadeSelected + 1 ); 
	CascadeShadowTexCoordBlend = posShadowVS * CascadeScale[iNextCascadeIndex] + CascadeOffset[iNextCascadeIndex];
	CascadeShadowTexCoordBlend.xy = CascadeShadowTexCoordBlend.xy * 0.5 + 0.5; // Map [-1, 1]x[-1, 1] -> [0, 1]x[0, 1]
	CascadeShadowTexCoordBlend.w = CascadeShadowTexCoordBlend.z;
	CascadeShadowTexCoordBlend.z = float(iNextCascadeIndex);
		
	// Calculate cascade blend
	float cascadeBlendWeight = 1.0f;
	float blendBandLocation = 1.0f;
		
	CalculateBlendAmountForMap(CascadeShadowTexCoord.xy, CascadeBlendArea, blendBandLocation, cascadeBlendWeight);
	if (blendBandLocation < CascadeBlendArea)
	{
		float percentLitBlend = CalculateVarianceShadow(CascadeShadowTexCoordBlend, posShadowVS, iNextCascadeIndex);
			
		// Blend the two calculated shadows by the blend amount.
        percentLit = mix(percentLitBlend, percentLit, cascadeBlendWeight); 
	}	
	*/
	
	selectCascade = iCascadeSelected;
	return percentLit;
}