Texture2D SourceMap;

void BlitColor(in float2 iTex	    : TEXCOORD0,
			   in float4 iFragCoord : SV_Position,
			  out float4 oFragColor : SV_Target0)
{
	oFragColor = SourceMap.Load(int3(iFragCoord.xy, 0));
}

void BlitDepth(in float2 iTex	    : TEXCOORD0,
			   in float4 iFragCoord : SV_Position,
			  out float  oFragDepth : SV_Depth)
{
	oFragDepth = SourceMap.Load(int3(iFragCoord.xy, 0)).r;
}
