#include "ModelVertexFactory.hlsl"
#include "ModelMaterialFactory.hlsl"

float4x4 WorldView;
float4x4 Projection;

void ShadowMapVS(VSInput input, 
			#if defined(_AlphaTest)
				 out float2 oTex   : TEXCOORD0,
			#endif
				 out float4 oPosCS : SV_POSITION)
{
	float4x4 wvp = mul(WorldView, Projection);

#ifdef _Skinning
	float4x4 Skin = CalculateSkinMatrix(input.BlendWeights, input.BlendIndices);
	oPosCS = mul(float4(input.Pos, 1.0), mul(Skin, wvp));
#else
	oPosCS = mul(float4(input.Pos, 1.0), wvp);
#endif
	
#if defined(_AlphaTest)
	oTex = input.Tex;
#endif
}

void ShadowMapVSM(
			#if defined(_AlphaTest)
				  in float2 iTex        : TEXCOORD0,
			#endif
				  in float4 FragCoord   : SV_POSITION,
				  out float2 oFragDepth : SV_Target0)
{
#if defined(_AlphaTest)
	float4 tap = DiffuseMap.Sample(MaterialSampler, iTex);
	if( tap.a < 0.01 ) discard;
#endif

	oFragDepth.x = FragCoord.z;
	oFragDepth.y = FragCoord.z * FragCoord.z;
	
	//vec2 dxdy = float2(dFdx(FragCoord.z), dFdy(FragCoord.z));
	//oFragDepth.y = FragCoord.z * FragCoord.z + 0.25 * dot(dxdy, dxdy);
}
