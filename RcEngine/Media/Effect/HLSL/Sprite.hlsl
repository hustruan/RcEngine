float2 InvWindowSize;
Texture2D SpriteTexture;
SamplerState LinearSampler;

//--------------------------------------------------------------------------------
void SpriteVS(in float3 iPos	: POSITION,
			  in float2 iTex	: TEXCOORD0,
			  in float4 iColor  : COLOR,
			  out float2 oTex   : TEXCOORD0,
			  out float4 oColor : TEXCOORD1,
			  out float4 oPosCS : SV_POSITION)
{
	float2 normPos = float2(iPos.xy) * InvWindowSize;
	normPos.y = 1.0 - normPos.y;

	oTex = iTex;	
	oColor = iColor;

	oPosCS = float4(normPos * 2.0 - 1.0, iPos.z, 1.0);
}

//--------------------------------------------------------------------------------
void SpritePS(in float2 iTex		: TEXCOORD0,
			  in float4 iColor		: TEXCOORD1,
			  out float4 oFragColor : SV_Target0)
{
	oFragColor = iColor * SpriteTexture.Sample( LinearSampler, iTex);
}

//--------------------------------------------------------------------------------
static const float Gamma = 2.2;
static const float Smoothness = 96.0;

void SDFFontPS(in float2 iTex		: TEXCOORD0,
			   in float4 iColor		: TEXCOORD1,
			  out float4 oFragColor : SV_Target0)
{
    float dist = SpriteTexture.Sample(LinearSampler, iTex).a;
	
	// perform adaptive anti-aliasing of the edges\n
	float w = clamp( Smoothness * (abs(ddx(iTex.x)) + abs(ddy(iTex.y))), 0.0, 0.5);
	
	float alpha = smoothstep(0.5-w, 0.5+w, dist);
	alpha = pow(alpha, 1.0/Gamma);
	oFragColor = float4(iColor.xyz, iColor.a * alpha);
}