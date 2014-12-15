struct VSInput 
{
	float3 Pos 		 	 : POSITION;

#ifdef _Skinning
	float4 BlendWeights  : BLENDWEIGHTS;
	uint4  BlendIndices  : BLENDINDICES;
#endif

	float3 Normal		 : NORMAL;
	
#if defined(_DiffuseMap) || defined(_DummyMap)
	float2 Tex			 : TEXCOORD0;
#endif

#ifdef _NormalMap
	float3 Tangent		 : TANGENT;
	float3 Binormal      : BINORMAL;
#endif
};

// Outputs
struct VSOutput
{
	float4 PosWS : TEXCOORD0;
	float2 Tex   : TEXCOORD1;

#ifdef _NormalMap
	float3x3 TangentToWorld : TEXCOORD3;
#else
	float3 NormalWS 	    : TEXCOORD3;
#endif 
	
	float4 PosCS : SV_POSITION;
};

// Help function
#ifdef _Skinning
	
	#define MaxNumBone 92
	cbuffer SkinCB
	{
		float4x4 SkinMatrices[MaxNumBone];	
	};
	
	float4x4 CalculateSkinMatrix(in float4 BlendWeights, in float4 BlendIndices)
	{
		return SkinMatrices[BlendIndices[0]] * BlendWeights[0] +
			   SkinMatrices[BlendIndices[1]] * BlendWeights[1] +
			   SkinMatrices[BlendIndices[2]] * BlendWeights[2] +
			   SkinMatrices[BlendIndices[3]] * BlendWeights[3];
	}
#endif