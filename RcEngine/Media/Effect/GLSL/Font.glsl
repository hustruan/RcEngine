[[Fragment=SDFFontPS]]

uniform sampler2D SpriteTexture; // Font
#pragma SpriteTexture : LinearSampler  // LinearSampler

layout (location = 0) in vec2 iTex;
layout (location = 1) in vec4 iColor;

layout (location = 0) out vec4 oFragColor;

uniform float GlyphCenter = 0.46; // 用Slider调出来的，对目前的SDF字体效果最好

void main()
{
	// retrieve signed distance
	float sdf = texture2D( SpriteTexture, iTex ).r;

	// perform adaptive anti-aliasing of the edges
	float w = fwidth(sdf);
	float a = smoothstep(GlyphCenter-w,GlyphCenter+w, sdf);

	// final color
	oFragColor = vec4(iColor.rgb, iColor.a*a);
}

[[Fragment=SDFFontOutlinePS]]

uniform sampler2D SpriteTexture; // Font
#pragma SpriteTexture : LinearSampler  // LinearSampler

layout (location = 0) in vec2 iTex;
layout (location = 1) in vec4 iColor;

layout (location = 0) out vec4 oFragColor;

const float GlyphCenter = 0.46; 
const float OutlineCenter = 0.65;
const vec3 OutlineColor = vec3(1, 0, 0);

void main()
{
	// retrieve signed distance
	float sdf = texture2D( SpriteTexture, iTex ).r;

	// perform adaptive anti-aliasing of the edges
	float w = fwidth(sdf);
	float a = smoothstep(GlyphCenter-w,GlyphCenter+w, sdf);

	// Outline
    float mu = smoothstep(OutlineCenter-w, OutlineCenter+w, sdf);
    vec3 rgb = mix(OutlineColor, iColor.rgb, mu);
    oFragColor = vec4(rgb, max(a, mu));
}
