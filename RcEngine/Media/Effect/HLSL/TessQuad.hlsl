float4 TessLevelOuter;
float2 TessLevelInner;
float2 ViewportDim;

struct VSOutput
{
	float4 Position  : POSITION;
};

VSOutput PassThroughVS(in float2 iPos : POSITION)
{
	VSOutput o;
	o.Position = float4(iPos, 0, 1);
	
	return o;
}

struct HSPerPatchOutput
{
	float TessLevelOuter[4] : SV_TessFactor;
	float TessLevelInner[2] : SV_InsideTessFactor;
};

HSPerPatchOutput PatchConstantFunc()
{
	HSPerPatchOutput output = (HSPerPatchOutput)0;
	
	output.TessLevelOuter[0] = TessLevelOuter[0];
	output.TessLevelOuter[1] = TessLevelOuter[1];
	output.TessLevelOuter[2] = TessLevelOuter[2];
	output.TessLevelOuter[3] = TessLevelOuter[3];
                               
	output.TessLevelInner[0] = TessLevelInner[0];
	output.TessLevelInner[1] = TessLevelInner[1];
	
	return output;
}

[domain("quad")]
[outputtopology("triangle_cw")]
[partitioning("fractional_even")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunc")]
VSOutput TCSQuad (uint i : SV_OutputControlPointID, InputPatch<VSOutput, 4> inputPatch) : POSITION
{
	VSOutput o;
	o.Position = inputPatch[i].Position;
	
	return o;
}

[domain("quad")]
float4 TESQuad (HSPerPatchOutput dummy, float2 uv : SV_DomainLocation, const OutputPatch<VSOutput, 4> inputPatch) : SV_POSITION
{
	float u = uv.x;
	float v = uv.y;

	float4 p0 = inputPatch[0].Position;
	float4 p1 = inputPatch[1].Position;
	float4 p2 = inputPatch[2].Position;
	float4 p3 = inputPatch[3].Position;
		
	float4 o;
	
	o = p0 * (1-u) * (1-v) + 
	    p1 * u * (1-v) +
		p2 * u * v +
		p3 * v * (1-u);

	o.xy  = o.xy / ViewportDim * 2.0;
	
	return o;
}

float4 PSQuad() : SV_Target
{
	return float4(1, 0, 0, 1);
}