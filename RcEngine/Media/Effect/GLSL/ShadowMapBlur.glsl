[[Fragment=ShadowBlurXPS]]
        
uniform sampler2DArray ShadowMap;   
#pragma ShadowMap : ShadowSampler 
    
uniform float ArraySlice;

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec2 oFragColor;

void main()
{
    vec2 filtered  = vec2(0.0);
    
    // box filter
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(-2, 0)).rg;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(-1, 0)).rg;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(0,  0)).rg;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(1,  0)).rg;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(2,  0)).rg;
    oFragColor = filtered;
    
    //filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(-3, 0)).rg;
    //filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(3, 0)).rg;
    //gl_FragColor = vec4(filtered * (1.0 / 7.0), 0.0, 1.0);
      
    /*
    // 5 tap gaussian filter
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(-2, 0)).rg * 0.0097576614950975;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(-1, 0)).rg * 0.20584891912478;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(0,  0)).rg * 0.56878683876025;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(1,  0)).rg * 0.20584891912478;
    filtered  += textureLodOffset(ShadowMap, vec3(iTex, ArraySlice), 0, ivec2(2,  0)).rg * 0.012439754406416;
    oFragColor = filtered;*/
}               

[[Fragment=ShadowBlurYPS]]

uniform sampler2D ShadowMapBlurX; 
#pragma ShadowMapBlurX : ShadowSampler     

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec2 oFragColor;

void main()
{
    vec2 filtered = vec2(0.0);
    
    // box filter
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(-2, 0)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(-1, 0)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0,  0)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(1,  0)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(2,  0)).rg;
	oFragColor = filtered;
    
    //filtered  += textureLodOffset(ShadowMap, iTex, 0, ivec2(-3,  0)).rg;
    //filtered  += textureLodOffset(ShadowMap, iTex, 0, ivec2(3,  0)).rg;
    //gl_FragColor = vec4(filtered * (1.0 / 7.0), 0.0, 1.0);
    
    /*
    // 5 tap gaussian filter
    filtered += textureOffset(ShadowMap, iTex, ivec2(0, -2)).rg * 0.0097576614950975;
    filtered += textureOffset(ShadowMap, iTex, ivec2(0, -1)).rg * 0.20584891912478;
    filtered += textureOffset(ShadowMap, iTex, ivec2(0,  0)).rg * 0.56878683876025;
    filtered += textureOffset(ShadowMap, iTex, ivec2(0,  1)).rg * 0.20584891912478;
    filtered += textureOffset(ShadowMap, iTex, ivec2(0,  2)).rg * 0.012439754406416;
    gl_FragColor = filtered;*/
}   