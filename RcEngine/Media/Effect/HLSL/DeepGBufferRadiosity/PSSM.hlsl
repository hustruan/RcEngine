#define MAX_CASCADE_COUNT 4
	
Texture2DArray CascadeShadowMap;

#if defined(PCF)
	#include "PCF.hlsl"
#elif defined(VSM)
	#include "VSM"
#elif defined(EVSM)
	#include "EVSM.hlsl"
#endif

cbuffer CSM
{
	// Light View
	float4x4 LightView;

	// Light Projection
	float4 CascadeScale[MAX_CASCADE_COUNT];  
	float4 CascadeOffset[MAX_CASCADE_COUNT];	 

	float2 BorderPaddingMinMax;				 // For map based selection, this keep pixels in valid range.
	float CascadeBlendArea;  				 // Amount to overlap when blending between cascades.
	float InvShadowMapSize;
	
	int NumCascades;
};

float EvalCascadeShadow(float4 positionWorldSpace, out int selectCascade)
{
	float percentLit = 1.0;
	float4 cascadeShadowTexCoord = 0.0;
	
	float4 positionLightSpace = mul(positionWorldSpace, LightView);
		
	int iCascadeSelected = 0;
	int cascadeFound = 0;
	for( int iCascade = 0; iCascade < NumCascades && cascadeFound == 0; ++iCascade ) 
	{
		cascadeShadowTexCoord = positionLightSpace * CascadeScale[iCascade] + CascadeOffset[iCascade];		

		if ( min( cascadeShadowTexCoord.x, cascadeShadowTexCoord.y ) > BorderPaddingMinMax.x && 
             max( cascadeShadowTexCoord.x, cascadeShadowTexCoord.y ) < BorderPaddingMinMax.y )
        { 
            iCascadeSelected = iCascade;   
            cascadeFound = 1; 
        }		
	}

	// Store selected cascade index in w
	cascadeShadowTexCoord.w = (float)iCascadeSelected;
	
#if defined(PCF)	
	{
		percentLit = EvalPossionPCF(CascadeShadowMap, cascadeShadowTexCoord, InvShadowMapSize * ShadowFilterSize);
	}
#elif defined(VSM)
	{
		percentLit = EvalVSM(CascadeShadowMap, cascadeShadowTexCoord);
	}
#elif defined(EVSM)
	{

	}
#endif

	selectCascade = iCascadeSelected;
	return percentLit;
}