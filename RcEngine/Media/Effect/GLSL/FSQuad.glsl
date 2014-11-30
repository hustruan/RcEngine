[[Fragment=BlitColor]]
uniform sampler2D SourceMap;

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = texelFetch(SourceMap, ivec2(gl_FragCoord.xy), 0);
}

[[Fragment=BlitDepth]]
uniform sampler2D SourceMap;

layout (location = 0) in vec2 iTex;

void main()
{
	gl_FragDepth = texelFetch(SourceMap, ivec2(gl_FragCoord.xy), 0).r;
}

