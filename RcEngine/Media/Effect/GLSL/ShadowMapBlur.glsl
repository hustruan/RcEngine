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
   
    oFragColor = filtered * 0.2;
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
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0, -2)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0, -1)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0, 0)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0, 1)).rg;
    filtered += textureLodOffset(ShadowMapBlurX, iTex, 0, ivec2(0, 2)).rg;
	
	oFragColor = filtered * 0.2;
}   