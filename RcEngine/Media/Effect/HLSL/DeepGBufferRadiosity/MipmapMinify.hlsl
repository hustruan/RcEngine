Texture2D SourceMap;
int3 PreviousMIP;

#define PreviousMIPLevel PreviousMIP.x
#define PreviousMIPSize	  PreviousMIP.yz

void Minify(in float2 iTex	      : TEXCOORD0,
			in float4 iFragCoord  : SV_Position,
			out float4 oFragColor : SV_Target0)
{
	int2 ssP = int2(iFragCoord.xy);
	
	// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling
	oFragColor = SourceMap.Load(int3(clamp(ssP * 2 + int2(ssP.y & 1, ssP.x & 1), int2(0, 0), PreviousMIPSize), PreviousMIPLevel));
}

SamplerState LinearSampler;
int MipLevel;

void BlitColor(in float2 iTex	    : TEXCOORD0,
			   in float4 iFragCoord : SV_Position,
			  out float4 oFragColor : SV_Target0)
{
	oFragColor = SourceMap.SampleLevel(LinearSampler, iTex, MipLevel);
}