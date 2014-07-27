// threads per group
#define WORK_GROUP_SIZE  32
#define MAX_LIGHTS 256   // Max lights per tile
#define NumGroupThreads (WORK_GROUP_SIZE  * WORK_GROUP_SIZE) // Toal threads per Group

cbuffer CSConstants
{
	matrix View;
	matrix Projection;
	matrix InvProj;
	matrix InvViewProj;
	float4 ViewportDim; // zw for invDim
	float2 ProjRatio;
	uint LightCount;
};

// Shader Resource View
Texture2D<float> DepthBuffer;
Buffer<float4> PointLightsPosRange; // Position and Range

// Unorder Access View
RWBuffer<uint> RWLightIndexCounter;
RWBuffer<uint> RWLightIndexList;
RWBuffer<uint2> RWLightListRange;

// Shared memory
groupshared uint TileMinZ;
groupshared uint TileMaxZ;

// Light list for the tile
groupshared uint TileLightList[MAX_LIGHTS];
groupshared uint NumTileLights;
groupshared uint LightIndexStart;

float3 ReconstructViewPosition(float zBuffer, uint2 fragCoord)
{
	float2 clipPos = (float2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	clipPos.y *= -1.0;
	
	float4 viewPositionH = mul( float4(clipPos, zBuffer, 1.0), InvProj );
    return viewPositionH.xyz / viewPositionH.w; 
}

// p1 is always camera origin in view space, float3(0, 0, 0)
float4 CreatePlaneEquation(/*float3 p1,*/ float3 p2, float3 p3)
{
	float4 plane;

	plane.xyz = normalize(cross(p2, p3));
	plane.w = 0;

	return plane;
}

//----------------------------------------------------------------------------------
[numthreads(WORK_GROUP_SIZE, WORK_GROUP_SIZE, 1)]
void LightCullCSMain(
	uint3 DispatchThreadID	: SV_DispatchThreadID,
	uint3 GroupID			: SV_GroupID,
	uint  GroupIndex		: SV_GroupIndex)
{
	int3 pixelIndex = int3(DispatchThreadID.xy, 0);

	float zw = DepthBuffer.Load(pixelIndex);
	float viewSpaceZ = ProjRatio.y / (zw - ProjRatio.x);	

	// Initialize per-tile variables
    if (GroupIndex == 0) 
	{
        TileMinZ = 0x7F7FFFFF;     
        TileMaxZ = 0;
		NumTileLights = 0;
    }
    GroupMemoryBarrierWithGroupSync();
	
	/*if (zw < 1.0)
	{
		InterlockedMin(TileMinZ, asuint(viewSpaceZ));
		InterlockedMax(TileMaxZ, asuint(viewSpaceZ));
	}*/
	InterlockedMin(TileMinZ, asuint(viewSpaceZ));
	InterlockedMax(TileMaxZ, asuint(viewSpaceZ));
	GroupMemoryBarrierWithGroupSync();

	float minTileZ = asfloat(TileMinZ);
	float maxTileZ = asfloat(TileMaxZ);
	
	// Construct frustum planes
	float4 frustumPlanes[6];
	{
		/*float3 frustumCorner[4];

		frustumCorner[0] = ReconstructViewPosition(1.0, uint2(GroupID.x * WORK_GROUP_SIZE, GroupID.y *WORK_GROUP_SIZE));
		frustumCorner[1] = ReconstructViewPosition(1.0, uint2(GroupID.x * WORK_GROUP_SIZE, (GroupID.y+1) * WORK_GROUP_SIZE));
		frustumCorner[2] = ReconstructViewPosition(1.0, uint2((GroupID.x+1) * WORK_GROUP_SIZE, (GroupID.y+1) * WORK_GROUP_SIZE));
		frustumCorner[3] = ReconstructViewPosition(1.0, uint2((GroupID.x+1) * WORK_GROUP_SIZE, GroupID.y * WORK_GROUP_SIZE));

		for(int i=0; i <4; i++)
			frustumPlanes[i] = CreatePlaneEquation(frustumCorner[i], frustumCorner[(i+1)&3]);		*/
		
		float2 tileScale = float2(ViewportDim.xy) * rcp(2.0f * float2(WORK_GROUP_SIZE, WORK_GROUP_SIZE));
		float2 tileBias = tileScale - float2(GroupID.xy);

		frustumPlanes[0] = float4(Projection._11 * tileScale.x, 0, tileBias.x, 0);
		frustumPlanes[1] = float4(-Projection._11 * tileScale.x, 0, 1 - tileBias.x, 0);
		frustumPlanes[2] = float4(0, Projection._22 * tileScale.y, 1 - tileBias.y, 0);
		frustumPlanes[3] = float4(0, -Projection._22 * tileScale.y, tileBias.y, 0);

		[unroll]
		for (uint i = 0; i < 4; ++i)
			frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));

		// Near/far
		frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
		frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);
	}

	// Cull lights for this tile	
	for (uint lightIndex = GroupIndex; lightIndex < LightCount; lightIndex += NumGroupThreads)
	{
		float4 lightPosAndRange = PointLightsPosRange[lightIndex];
		
		float4 lightPosVS = mul( float4(lightPosAndRange.xyz, 1.0), View);
		float lightRange = lightPosAndRange.w;

		// Intersect with each frustun plane
		bool inFrustum = true;
		[unroll]
		for (int i = 0; i < 6; ++i)
		{
			float d = dot(frustumPlanes[i], float4(lightPosVS.xyz, 1.0));
			inFrustum = inFrustum && (d >= -lightRange);
		}

		if (inFrustum)
		{
			uint listIndex;
            InterlockedAdd(NumTileLights, 1, listIndex);
            TileLightList[listIndex] = lightIndex;
		}
	}
	GroupMemoryBarrierWithGroupSync();

	// Write shared light list index to global memory
	uint startOffset = 0;
	if (GroupIndex == 0)
	{
		uint tileIdx = GroupID.y * WORK_GROUP_SIZE + GroupID.x;
	
		if (NumTileLights > 0)
			InterlockedAdd(RWLightIndexCounter[0], NumTileLights, startOffset);

		// Store tile light index range
		RWLightListRange[tileIdx] = uint2(NumTileLights, startOffset);
		LightIndexStart = startOffset;
	}
	GroupMemoryBarrierWithGroupSync();
	startOffset = LightIndexStart;

	for (uint i = GroupIndex; i < NumTileLights; i += NumGroupThreads)
		RWLightIndexList[startOffset+i] = TileLightList[i];	
}


////////////////////////////////////////////////////////////////////////////
#include "ModelVertexFactory.hlsl"
#include "ModelMaterialFactory.hlsl"
#include "LightingUtil.hlsl"

float3 CameraOrigin;

Buffer<float3> PointLightsColor; // Position and Range
Buffer<float3> PointLightsFalloff; // Position and Range
Buffer<uint> LightIndexList;
Buffer<uint2> LightListRange;

void EvalulateAndAccumilateLight(in int lightIndex, in float3 litPos, in float3 N, in float3 V, in float3 specularAlbedo,
								 in float shininess, inout float3 diffuseLight, inout float3 specularLight)
{
	float4 lightPosRange = PointLightsPosRange[lightIndex];
	
	float3 L = lightPosRange.xyz - litPos;
	float dist = length(L);
	if (dist < lightPosRange.w)
	{
		L = normalize(L);
		
		float3 lightColor = PointLightsColor[lightIndex];
		float3 lightFalloff = PointLightsFalloff[lightIndex]; 

		float3 lightCombined = lightColor * saturate(dot(N,L)) * CalcAttenuation(dist, lightFalloff);

		diffuseLight += lightCombined;

		// Frensel in moved to calculate in shading pass
		float3 H = normalize(V + L);
		float3 fresnel = CalculateFresnel(specularAlbedo, L, H);
		specularLight += CalculateSpecular(N, H, shininess) * lightCombined * fresnel;
	}
}


void ForwardShadingPSMain(in VSOutput input,
						  out float4 oFragColor : SV_Target0 ) 
{
	Material material;
	GetMaterial(input.Tex, material);
        
	// normal map
#ifdef _NormalMap
	float3 N = NormalMap.Sample(MaterialSampler, input.Tex).rgb * 2.0 - 1.0;
	N = normalize( mul(N, input.TangentToWorld) );
#else
	float3 N = normalize(input.NormalWS);
#endif	      
        
    float3 V = normalize(CameraOrigin - input.PosWS.xyz);
    
	float3 diffuseLight = (float3)0;
	float3 specularLight = (float3)0;

	uint2 tileXY = uint2(input.PosCS.xy) / uint2(WORK_GROUP_SIZE, WORK_GROUP_SIZE);
	uint tileIdx = tileXY.y * WORK_GROUP_SIZE + tileXY.x;

	uint2 tileLightRange = LightListRange[tileIdx];
	for (uint i = 0; i < tileLightRange.x; ++i)
	{
		uint lightIndex = tileLightRange.y + i;
		uint globalLightIndex = LightIndexList[lightIndex];

		EvalulateAndAccumilateLight(globalLightIndex, input.PosWS.xyz, N, V, material.SpecularAlbedo,
		                            material.Shininess, diffuseLight, specularLight);
	}

	float3 final = diffuseLight * material.DiffuseAlbedo;
	
	float specularNormTerm = (material.Shininess + 2.0) / 8.0;
	final += specularNormTerm * specularLight;

	final += material.DiffuseAlbedo * 0.1; // Ambient

	oFragColor = float4(final, 1.0);  	               
}


