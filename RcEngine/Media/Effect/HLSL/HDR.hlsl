Texture2D HDRBuffer;
Texture2D<float> DepthBuffer;

float4 CopyColor(in float2 oTex		  : TEXCOORD0,
			     in float4 iFragCoord : SV_Position) : SV_Target0
{
	int3 samplePos = int3(iFragCoord.xy, 0);
	return HDRBuffer.Load(samplePos);
}

void CopyColorDepth(in float2 oTex : TEXCOORD0 : TEXCOORD0,
					in float4 iFragCoord : SV_Position,
				   out float4 oFragColor : SV_Target0,
				   out float  oDepth     : SV_Depth)
{

	int3 samplePos = int3(iFragCoord.xy, 0);
	oFragColor = HDRBuffer.Load(samplePos);
	oDepth = DepthBuffer.Load(samplePos);
}