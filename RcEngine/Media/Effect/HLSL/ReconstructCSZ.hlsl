Texture2D DepthBuffer;
float2 ClipInfo;
int PreviousMIPLevel;

void ReconstructCSZ(in float2 oTex	    : TEXCOORD0, 
					in float4 iFragCoord : SV_Position,
					out float oCSZDepth : SV_Target0)
{
	float z = DepthBuffer.Load(int3(iFragCoord.xy, 0)).r;
	oCSZDepth = ClipInfo.y / ( z - ClipInfo.x);
}

void CSZMinify(in float4 iFragCoord : SV_Position, out float oCSZLevel : SV_Target0)
{
	int2 ssP = int2(iFragCoord.xy);
	
	// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling
	//oCSZLevel = DepthBuffer.Load(int3(ssP * 2 + int2((ssP.y & 1) ^ 1, (ssP.x & 1) ^ 1), PreviousMIPLevel)).r;
	oCSZLevel = DepthBuffer.Load(int3(ssP, 0)).r;
}

float4 CopyColor(in float2 oTex		  : TEXCOORD0,
			     in float4 iFragCoord : SV_Position) : SV_Target0
{
	float z = DepthBuffer.Load(int3(iFragCoord.xy, 0)).r;
	return float4(z, z, z, 1.0);
}

