#define MAX_CASCADE_COUNT 4
	
uniform sampler2DArray CascadeShadowTex;	
#pragma CascadeShadowTex : ShadowSampler 

uniform int NumCascades;
uniform mat4 ShadowView;						// Light View Matrix
uniform vec4 CascadeScale[MAX_CASCADE_COUNT];   // Shadow projection info
uniform vec4 CascadeOffset[MAX_CASCADE_COUNT];	
uniform vec2 BorderPaddingMinMax;				// For map based selection, this keep pixels in valid range.
uniform float CascadeBlendArea;  				// Amount to overlap when blending between cascades.

/**
 * Calculate blend weight for cascade boundary pixels.
 * @param blendArea, blend width.
 * @param blendBandLocation, current pixel's location in blend band.
 * @param cascadeBlendWeight, calculated blend weight, [1, 0] -> [width, 0].
 */ 
void CalculateBlendAmountForMap(in vec2 CascadeShadowTexCoord, in float blendArea, inout float blendBandLocation, out float cascadeBlendWeight)
{
	vec2 distanceToOne = vec2(1.0 - CascadeShadowTexCoord.x, 1.0 - CascadeShadowTexCoord.y);
    blendBandLocation = min(CascadeShadowTexCoord.x, CascadeShadowTexCoord.y);
    blendBandLocation = min( blendBandLocation, min(distanceToOne.x, distanceToOne.y) );
    cascadeBlendWeight = blendBandLocation / blendArea;
}

float ChebyshevUpperBound(vec2 moments, float depth, float minVariance)
{
	float mean = moments.x;
	float measSquared = moments.y;
	
	float p = float(depth <= mean);
	
	float variance = max(minVariance, measSquared - mean*mean);
	float d = depth - mean;
	float pmax = variance / (variance + d * d);
	
	 // To combat light-bleeding, experiment with raising p_max to some power
     // (Try values from 0.1 to 100.0, if you like.)	
	return pow( max(p, pmax), 5.0 );
}

float CalculateVarianceShadow(in vec4 CascadeShadowTexCoord, in vec4 posShadowViewSpace, in int iCascade)
{
	vec4 CascadeShadowTexCoordDDX = dFdx(posShadowViewSpace) * CascadeScale[iCascade];
	vec4 CascadeShadowTexCoordDDY = dFdy(posShadowViewSpace) * CascadeScale[iCascade];
	
	//vec2 moments = textureGrad(CascadeShadowTex, CascadeShadowTexCoord.xyz, CascadeShadowTexCoordDDX.xy, CascadeShadowTexCoordDDY.xy).rg;
	vec2 moments = textureLod(CascadeShadowTex, CascadeShadowTexCoord.xyz, 0).rg;
	return ChebyshevUpperBound(moments, CascadeShadowTexCoord.w, 0.0001);
}

float CalculateVarianceShadowWrong(in vec4 CascadeShadowTexCoord, in vec4 posShadowViewSpace, in int iCascade)
{
	vec2 moments = texture(CascadeShadowTex, CascadeShadowTexCoord.xyz).rg;
	return ChebyshevUpperBound(moments, CascadeShadowTexCoord.w, 0.0001);
}

float EvalCascadeShadow(in vec4 posWorldSpace, out int selectCascade)
{
	float percentLit = 0.0;

	vec4 CascadeShadowTexCoord = vec4(0.0);
	vec4 CascadeShadowTexCoordBlend = vec4(0.0);
	
	// Compute view space position of shadow
	vec4 posShadowVS = posWorldSpace * ShadowView;
		
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
	CascadeShadowTexCoord.z = float(iCascadeSelected);
	
	percentLit = CalculateVarianceShadow(CascadeShadowTexCoord,  posShadowVS, iCascadeSelected);	
	
	
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
	
	
	selectCascade = iCascadeSelected;
	return percentLit;
}
