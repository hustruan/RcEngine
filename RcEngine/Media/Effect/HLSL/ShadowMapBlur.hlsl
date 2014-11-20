
Texture2DArray ShadowMap; // Texture Array for PSSM
Texture2D ShadowMapBlurX;
SamplerState ShadowSampler;
float ArraySlice;

void ShadowBlurXPS(in float2 iTex : TEXCOORD0,
				  out float2 oFragColor : SV_Target0)
{
	float2 filtered = 0.0;

    // box filter
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-2, 0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-1, 0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(0,  0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(1,  0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(2,  0)).rg;
    
	oFragColor = filtered * 0.2;
}         

        
void ShadowBlurYPS(in float2 iTex : TEXCOORD0,
				  out float2 oFragColor : SV_Target0)
{
	float2 filtered  = 0.0;
        
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0, -2)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0, -1)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0, 0)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0, 1)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0, 2)).rg;
    
	oFragColor = filtered * 0.2;
}               
