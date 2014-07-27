Texture2D ColorMap;
SamplerState LinearSampler;

float4 PSMain(in float2 iTex : TEXCOORD0) : SV_Target0
{
	float4 final = ColorMap.Sample(LinearSampler, iTex);
	return final;
}