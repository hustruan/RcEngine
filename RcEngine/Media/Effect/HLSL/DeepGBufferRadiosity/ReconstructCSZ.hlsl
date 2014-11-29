Texture2D DepthBuffer;

#if USE_DEPTH_PEEL
	Texture2D PeeledDepthBuffer;
	#if USE_OCT16
		Texture2D NormalBuffer;
		Texture2D PeeledNormalBuffer;
	#endif
#endif

float2 ClipInfo;

struct PixelOutput
{
#if USE_DEPTH_PEEL == 0
	float CSZDepth : SV_Target0;
#else
	float2 CSZDepth : SV_Target0;
	#if USE_OCT16
		float4 Normal : SV_Target1;
	#endif
#endif
};

#if USE_OCT16
	#include "oct.hlsl"
	float4 FetchAndEncodeNormal(int3 ssP)
	{
		float2 n0 = encode16(NormalBuffer.Load(ssP).rgb * 2.0 - 1.0) * 0.5 + 0.5;
		float2 n1 = encode16(PeeledNormalBuffer.Load(ssP).rgb * 2.0 - 1.0) * 0.5 + 0.5;
		return float4(n0, n1);
	}
#endif

PixelOutput ReconstructCSZ(in float2 oTex	 : TEXCOORD0, 
					in float4 iFragCoord : SV_Position)
{
	PixelOutput output;

	int3 ssP = int3(iFragCoord.xy, 0);
	float z0 = DepthBuffer.Load(ssP).r;

#if USE_DEPTH_PEEL
	float z1 = PeeledDepthBuffer.Load(ssP).r;
	output.CSZDepth = float2( ClipInfo.y / ( z0 - ClipInfo.x), 
							  ClipInfo.y / ( z1 - ClipInfo.x) );

	#if USE_OCT16
		output.Normal = FetchAndEncodeNormal(ssP);
	#endif
#else
	output.CSZDepth = z0;
#endif

	return output;
}
