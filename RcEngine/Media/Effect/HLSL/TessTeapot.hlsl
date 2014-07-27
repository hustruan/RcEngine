float4x4 World;
float4x4 ViewProj;
int TessLevel;

struct VSOutput
{
	float4 Position : C0NTROL_POINT_P0SITION;
};

struct HSOutput
{
	float4 Position : C0NTROL_POINT_P0SITION;
};

struct HSPerPatchOutput
{
	float TessLevelOuter[4] : SV_TessFactor;
	float TessLevelInner[2] : SV_InsideTessFactor;
};

struct DSOutput
{
	float4 PosWS    : POSITION;
	float3 NormalWS : NORMAL;
	float4 PosCS    : SV_POSITION;
};

VSOutput PassThroughVS(in float3 iControlPoint : POSITION)
{
	VSOutput output;
	output.Position = mul(float4(iControlPoint, 1.0), World);
	
	return output;
}

HSPerPatchOutput PatchConstantFunc()
{
	HSPerPatchOutput output = (HSPerPatchOutput)0;
	
	output.TessLevelOuter[0] = float(TessLevel);
	output.TessLevelOuter[1] = float(TessLevel);
	output.TessLevelOuter[2] = float(TessLevel);
	output.TessLevelOuter[3] = float(TessLevel);

	output.TessLevelInner[0] = float(TessLevel);
	output.TessLevelInner[1] = float(TessLevel);
	
	return output;
}

[domain("quad")]
[outputtopology("triangle_cw")]
[partitioning("fractional_even")]
[outputcontrolpoints(16)]
[patchconstantfunc("PatchConstantFunc")]
HSOutput TCSTeapot(uint i : SV_OutputControlPointID,
                   InputPatch<VSOutput, 16> pathch)
{
	HSOutput output;
	output.Position = pathch[i].Position;
	
	return output;
}

// Compute bernstein and partial derivatives of bernstein
void Bernstein(out float b[4], out float db[4], float t)
{
	float t1 = (1.0 - t);
	float t12 = t1 * t1;
	
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;

	// Derivatives
	db[0] = -3.0 * t1 * t1;
	db[1] = -6.0 * t * t1 + 3.0 * t12;
	db[2] = -3.0 * t * t + 6.0 * t * t1;
	db[3] = 3.0 * t * t;
}

[domain("quad")]
DSOutput TESTeapot(HSPerPatchOutput dummy, float2 uv : SV_DomainLocation, const OutputPatch<HSOutput, 16> pathch)
{
	DSOutput output = (DSOutput)0;
	
	float bu[4], bv[4], dbu[4], dbv[4];
	Bernstein(bu, dbu, uv.x);
	Bernstein(bv, dbv, uv.y);

	float4 p = (float4)0;
	float4 du = (float4)0;
	float4 dv = (float4)0;

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			p  += bu[i] * bv[j] * pathch[i*4+j].Position;	
			du += dbu[i] * bv[j] * pathch[i*4+j].Position;
			dv += bu[i] * dbv[j] * pathch[i*4+j].Position;
		}

	output.PosWS = p;
	output.NormalWS = cross(du.xyz, dv.xyz);
	output.PosCS = mul(p, ViewProj);
	
	return output;
}

static const float3 LightPos = float3(100, 100, 100);

float4 PSTeapot(DSOutput input) : SV_Target
{
	float3 L = normalize(LightPos - input.PosWS.xyz);
	float3 N = normalize(input.NormalWS);
	
	return float4(1.0, 0, 0, 1.0) * max(dot(N, L), 0.0);
}

