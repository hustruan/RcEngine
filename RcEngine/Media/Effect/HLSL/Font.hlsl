Texture2D SpriteTexture;
SamplerState LinearSampler;

static const float GlyphCenter = 0.46;
static const float OutlineCenter = 0.65;
static const float3 OutlineColor = float3(1, 0, 0);

void SDFFontPS(in float2 iTex		: TEXCOORD0,
			   in float4 iColor		: TEXCOORD1,
			  out float4 oFragColor : SV_Target0)
{
   // retrieve signed distance
	float sdf = SpriteTexture.Sample( LinearSampler, iTex).a;

	// perform adaptive anti-aliasing of the edges
	float w = fwidth(sdf);
	float a = smoothstep(GlyphCenter-w,GlyphCenter+w, sdf);

	// final color
	oFragColor = float4(iColor.rgb, iColor.a*a);
}

void SDFFontOutlinePS(in float2 iTex		: TEXCOORD0,
					  in float4 iColor		: TEXCOORD1,
					  out float4 oFragColor : SV_Target0)
{
    // retrieve signed distance
	float sdf = SpriteTexture.Sample( LinearSampler, iTex).a;

	// perform adaptive anti-aliasing of the edges
	float w = fwidth(sdf);
	float a = smoothstep(GlyphCenter-w,GlyphCenter+w, sdf);

	// Outline
    float mu = smoothstep(OutlineCenter-w, OutlineCenter+w, sdf);
    float3 rgb = lerp(OutlineColor, iColor.rgb, mu);
    oFragColor = float4(rgb, max(a, mu));
}