// Random Possion Disk PCF
SamplerComparisonState PCFSampler;

#define MAX_POSSION_SAMPLES 24

cbuffer cbPossionDiskSamples
{
	int NumPossionSamples							: packoffset(c0.x);
    float ShadowFilterSize							: packoffset(c0.y);
    float ShadowBias								: packoffset(c0.z);
    float padding12									: packoffset(c0.w);
	float4 PoissonDiskSamples[MAX_POSSION_SAMPLES]  : packoffset(c1.x);
};

float EvalPossionPCF(Texture2D shadowMap, float3 shadowTexCoord, float filterRadiusUV)
{
	float sum = 0.0f;
    for(int i=0; i < NumPossionSamples; i++)
	{
        float2 offset = PoissonDiskSamples[i].xy * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(PCFSampler, shadowTexCoord.xy + offset, shadowTexCoord.z-ShadowBias);
    }
    return sum /NumPossionSamples;
}

//static const float gShadowBias[4] = { 0.003f, 0.003f, 0.003f, 0.003f };

// GIRoom
static const float gShadowBias[4] = { 0.005f, 0.006f, 0.0009f, 0.0009f };

float EvalPossionPCF(Texture2DArray CSM, float4 shadowTexCoord, float filterRadiusUV)
{
	//float shadowBias = 0.003f; 
	//float shadowBias = 0.0003f;
	float shadowBias = gShadowBias[(int)shadowTexCoord.w];

	float sum = 0.0f;
    for(int i=0; i < NumPossionSamples; i++)
    {
        float2 offset = PoissonDiskSamples[i].xy * filterRadiusUV;
        sum += CSM.SampleCmpLevelZero(PCFSampler, float3(shadowTexCoord.xy + offset, shadowTexCoord.w), shadowTexCoord.z-shadowBias);
    }
    return sum /NumPossionSamples;
}


