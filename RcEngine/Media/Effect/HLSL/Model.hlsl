#include "ModelVertexFactory.hlsl"

// Unifroms
float4x4 World;
float4x4 ViewProj;

//-------------------------------------------------------------------------------------
VSOutput ModelVS(VSInput input)
{
	VSOutput output = (VSOutput)0;
	
	// calculate position in view space:
#ifdef _Skinning
	float4x4 Skin = CalculateSkinMatrix(input.BlendWeights, input.BlendIndices);
	float4x4 SkinWorld = mul(Skin, World);
	output.PosWS = mul( float4(input.Pos, 1.0), SkinWorld );
#else
	output.PosWS = mul( float4(input.Pos, 1.0), World );
#endif

	// calculate view space normal.
#ifdef _Skinning
	float3 normal = normalize( mul(input.Normal, (float3x3)SkinWorld) );
#else
	float3 normal = normalize( mul(input.Normal, (float3x3)World) );
#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		float3 tangent = normalize( mul(input.Tangent, (float3x3)SkinWorld) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)SkinWorld) );
	#else
		float3 tangent = normalize( mul(input.Tangent, (float3x3)World) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)World) );
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	output.TangentToWorld = float3x3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	 //output.TangentToWorld = transpose(output.TangentToWorld);
#else
	output.NormalWS = normal;
#endif
	
#ifndef _DiffuseMap
	output.Tex = float2(0, 0); // Todo: Not output tex if no texture
#else 
	output.Tex = input.Tex;
#endif

	output.PosCS = mul(output.PosWS, ViewProj);

	return output;
}
