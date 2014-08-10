
Texture2DArray ShadowMap; // Texture Array for PSSM
Texture2D ShadowMapBlurX;
SamplerState ShadowSampler;
int ArraySlice;

void ShadowBlurXPS(in float2 iTex : TEXCOORD0,
				     out float2 oFragColor : SV_Target0)
{
	float2 filtered  = 0.0;
        
    // box filter
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-2, 0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-1, 0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(0,  0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(1,  0)).rg;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(2,  0)).rg;
    oFragColor = filtered * 0.2;
        
        
    //filtered  += textureLodOffset(InputTex, vec3(oTex, ArraySlice), 0, ivec2(-3, 0)).rg;
    //filtered  += textureLodOffset(InputTex, vec3(oTex, ArraySlice), 0, ivec2(3, 0)).rg;
    //gl_FragColor = vec4(filtered * (1.0 / 7.0), 0.0, 1.0);
    
    
    /*
    // 5 tap gaussian filter
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-2, 0)).rg * 0.0097576614950975;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(-1, 0)).rg * 0.20584891912478;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(0,  0)).rg * 0.56878683876025;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(1,  0)).rg * 0.20584891912478;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, float3(iTex, ArraySlice), 0, int2(2,  0)).rg * 0.012439754406416;
    oFragColor = filtered;*/
}         

        
void ShadowBlurYPS(in float2 iTex : TEXCOORD0,
				     out float2 oFragColor : SV_Target0)
{
	float2 filtered  = 0.0;
        
    // box filter
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(-2, 0)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(-1, 0)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(0,  0)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(1,  0)).rg;
    filtered  += ShadowMapBlurX.SampleLevel(ShadowSampler, iTex, 0, int2(2,  0)).rg;
    oFragColor = filtered * 0.2;
        
        
    //filtered  += textureLodOffset(InputTex, vec3(oTex, ArraySlice), 0, ivec2(-3, 0)).rg;
    //filtered  += textureLodOffset(InputTex, vec3(oTex, ArraySlice), 0, ivec2(3, 0)).rg;
    //gl_FragColor = vec4(filtered * (1.0 / 7.0), 0.0, 1.0);
    
    
    /*
    // 5 tap gaussian filter
    filtered  += ShadowMap.SampleLevel(ShadowSampler, iTex, 0, int2(-2, 0)).rg * 0.0097576614950975;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, iTex, 0, int2(-1, 0)).rg * 0.20584891912478;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, iTex, 0, int2(0,  0)).rg * 0.56878683876025;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, iTex, 0, int2(1,  0)).rg * 0.20584891912478;
    filtered  += ShadowMap.SampleLevel(ShadowSampler, iTex, 0, int2(2,  0)).rg * 0.012439754406416;
    oFragColor = filtered;*/
}               
