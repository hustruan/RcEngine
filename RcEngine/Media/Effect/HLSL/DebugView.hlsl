float4x4 World;
float4x4 ViewProj;
float3 Color;

float4 VSMain(in float3 iPos : POSITION) : SV_POSITION
{
	float4x4 wvp = mul(World, ViewProj);
	return mul( float4(iPos, 1.0), wvp );
}

float4 PSMain() : SV_Target0
{
	return float4(Color, 1.0);
}
