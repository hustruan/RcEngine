[[Fragment=CopyColor]]

uniform sampler2D HDRBuffer;
uniform sampler2D DepthBuffer;

// Output
layout (location = 0) out vec4 oFragColor;

void main()
{
	ivec2 samplePos = ivec2(gl_FragCoord.xy);
	oFragColor = texelFetch(HDRBuffer, samplePos, 0);
}

[[Fragment=CopyColorDepth]]

uniform sampler2D HDRBuffer;
uniform sampler2D DepthBuffer;

// Output
layout (location = 0) out vec4 oFragColor;

void main()
{
	ivec2 samplePos = ivec2(gl_FragCoord.xy);
	oFragColor = texelFetch(HDRBuffer, samplePos, 0);
	gl_FragDepth = texelFetch(DepthBuffer, samplePos, 0).r;
}

