[[Fragment=Minify]]
uniform sampler2D SourceMap;
uniform ivec3 PreviousMIP;

#define PreviousMIPLevel PreviousMIP.x
#define PreviousMIPSize	  PreviousMIP.yz

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec4 oFragColor;

void main()
{
	ivec2 ssP = ivec2(gl_FragCoord.xy);
	oFragColor = texelFetch(SourceMap, clamp(ssP * 2 + ivec2(ssP.y & 1, ssP.x & 1), ivec2(0), PreviousMIPSize), PreviousMIPLevel);
}


[[Fragment=BlitColor]]

uniform sampler2D SourceMap;
#pragma SourceMap : LinearSampler 

uniform int MipLevel;

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = textureLod(SourceMap, iTex, MipLevel);
}
