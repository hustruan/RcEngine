
SamplerState EnvSampler;
TextureCube EnvMap;

float4x4 WorldViewProj;

void SkyVS(in float3 iPos : POSITION, 
		  out float3 oTex : TEXCOORD0,
		  out float4 oPosition : SV_Position)
{
	oTex = iPos;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    oPosition = mul( float4(iPos, 1.0), WorldViewProj ).xyww;
}


float4 SkyPS(in float3 iTex : TEXCOORD0) : SV_Target0
{
	return EnvMap.Sample(EnvSampler, iTex);
}