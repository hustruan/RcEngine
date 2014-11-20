#include "ModelVertexFactory.hlsl"
#include "ModelMaterialFactory.hlsl"

float4x4 World;
float4x4 ViewProj;

void ShadowMapVS(VSInput input, 
			#if defined(_AlphaTest)
				 out float2 oTex   : TEXCOORD0,
			#endif
				 out float4 oPos : SV_POSITION)
{
// calculate position in view space:
#ifdef _Skinning
	float4x4 Skin = CalculateSkinMatrix(input.BlendWeights, input.BlendIndices);
	oPos = mul( float4(input.Pos, 1.0), mul(mul(Skin, World), ViewProj) );
#else
	oPos = mul( float4(input.Pos, 1.0), mul(World, ViewProj) );
#endif
	
#if defined(_AlphaTest)
	oTex = input.Tex;
#endif
}

// PCF
void ShadowMapAlphaPCF(in float2 iTex : TEXCOORD0)
{
	float4 tap = DiffuseMap.Sample(MaterialSampler, iTex);
	if( tap.a < 0.01 ) discard;
}


// VSM
void ShadowMapVSM(
			#if defined(_AlphaTest)
				  in float2 iTex        : TEXCOORD0,
			#endif
				  in float4 iFragCoord   : SV_POSITION,
				  out float2 oFragDepth : SV_Target0)
{
#if defined(_AlphaTest)
	float4 tap = DiffuseMap.Sample(MaterialSampler, iTex);
	if( tap.a < 0.01 ) discard;
#endif

	oFragDepth.x = iFragCoord.z;
	oFragDepth.y = iFragCoord.z * iFragCoord.z;
	
	//vec2 dxdy = float2(dFdx(FragCoord.z), dFdy(FragCoord.z));
	//oFragDepth.y = FragCoord.z * FragCoord.z + 0.25 * dot(dxdy, dxdy);
}

// EVSM
void ShadowMapEVSM(
			#if defined(_AlphaTest)
				  in float2 iTex         : TEXCOORD0,
			#endif
				  in float4 iFragCoord   : SV_POSITION,
				  out float2 oFragDepth  : SV_Target0)
{
	const float K_EVSM_VALUE = 80.0f;
	float k = K_EVSM_VALUE;

	oFragDepth.x = exp(iFragCoord.z * k);
	oFragDepth.y = oFragDepth.x * oFragDepth.x;
}