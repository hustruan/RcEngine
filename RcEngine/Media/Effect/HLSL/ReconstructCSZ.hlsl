Texture2D ColorBuffer;
Texture2D NormalBuffer;
Texture2D<float> DepthBuffer;

#ifdef USE_PEELED_BUFFERS
	Texture2D PeeledColorBuffer;
	Texture2D PeeledNormalBuffer;
	Texture2D<float> PeeledDepthBuffer;
#endif 

float3 ClipInfo;

// Shader Output
struct PSOutput
{
	float3 Color	: SV_Target0;
	float4 Normal   : SV_Target1;

#ifdef USE_PEELED_BUFFERS
	float2 CameraSpaceZ : SV_Target2;
	float3 PeeledColor	: SV_Target3;

	#ifndef USE_OCT16
		float4 PeeledNormal : SV_Target4;	
	#endif 
#else 
	float CameraSpaceZ  : SV_Target2;
#endif
};

float ComputeCameraSpaceZ(float z, float3 clip)
{
	return 	clip.x / ( z * clip.y + clip.z);
}

PSOutput ReconstructCSZ(in float2 oTex	    : TEXCOORD0, 
						in float4 iFragCoord : SV_Position)
{
	PSOutput output;
	
	int3 ssp = int3(iFragCoord.xy, 0);

	output.Color = ColorBuffer.Load(ssp).rgb;

#ifdef USE_PEELED_BUFFERS

	output.PeeledColor = PeeledColorBuffer.Load(ssp).rgb;
	output.CameraSpaceZ = float2( ComputeCameraSpaceZ(DepthBuffer.Load(ssp), ClipInfo),
								  ComputeCameraSpaceZ(PeeledDepthBuffer.Load(ssp), ClipInfo) );

	#ifdef USE_OCT16
		// Combine two normals
	#else
		output.Normal = NormalBuffer.Load(ssp);
		output.PeeledNormal = PeeledNormalBuffer.Load(ssp);
	#endif 
	
#else
	
	output.CameraSpaceZ = ComputeCameraSpaceZ(DepthBuffer.Load(ssp), ClipInfo);
	output.Normal = NormalBuffer.Load(ssp);

#endif

	return output;
}