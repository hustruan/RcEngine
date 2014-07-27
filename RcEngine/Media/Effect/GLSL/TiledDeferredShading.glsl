#extension GL_ARB_compute_shader : enable
#include "/LightingUtil.glsl"

#define WORK_GROUP_SIZE  32
#define NumGroupThreads (WORK_GROUP_SIZE  * WORK_GROUP_SIZE ) // Toal threads per Group
#define MAX_LIGHTS 256   // Max lights per tile

struct Light
{
	vec3 Color;
	float Range;
	vec3 Position;
	vec3 Falloff;
};

// Uniforms
layout (std140) uniform CSConstants
{
	mat4 View;
	mat4 Projection;
	mat4 InvProj;
	mat4 InvViewProj;
	vec4 ViewportDim;	 // zw for invDim
	vec2 ProjRatio;
	vec3 CameraOrigin;
	uint LightCount;
};

// Shader resource view
layout (binding = 0) uniform sampler2D DepthBuffer;
layout (binding = 1) uniform sampler2D GBuffer0; // Normal + Power
layout (binding = 2) uniform sampler2D GBuffer1; // Albedo + Specular Intensity

// Unorder resource view
layout (binding = 0, rgba32f) uniform writeonly image2D RWLightAccumulation;

// Structure Buffer
layout (std430, binding = 0) buffer LightsSRV
{
    Light Lights[];
};

shared uint TileMinZ;
shared uint TileMaxZ;

// Light list for the tile
shared uint TileLightList[MAX_LIGHTS];
shared uint NumTileLights;

//-----------------------------------------------------------------------------
// World space position
vec3 ReconstructWorldPosition(float zBuffer, uvec2 fragCoord)
{
	vec2 clipPos = (vec2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	
    vec4 worldPositionH = vec4(clipPos, zBuffer, 1.0) * InvViewProj;
    return worldPositionH.xyz / worldPositionH.w;
}

vec3 ReconstructViewPosition(float zBuffer, uvec2 fragCoord)
{
	vec2 clipPos = (vec2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	
	vec4 viewPositionH = vec4(clipPos, zBuffer, 1.0) * InvProj;
    return viewPositionH.xyz / viewPositionH.w; 
}


// p1 is always camera origin in view space, float3(0, 0, 0)
vec4 CreatePlaneEquation(/*float3 p1,*/ vec3 p2, vec3 p3)
{
	vec4 plane;

	plane.xyz = normalize(cross(p2, p3));
	plane.w = 0;

	return plane;
}

void EvalulateAndAccumilateLight(in Light light, in vec3 litPos, in vec3 N, in vec3 V,
							     in float shininess, inout vec3 diffuseLight, inout vec3 specularLight)
{
	vec3 L = light.Position - litPos;
	float dist = length(L);
	if (dist < light.Range)
	{
		L = normalize(L);
		
		vec3 lightCombined = light.Color * saturate(dot(N,L)) * CalcAttenuation(dist, light.Falloff);

		diffuseLight += lightCombined;

		// Frensel in moved to calculate in shading pass
		vec3 H = normalize(V + L);
		specularLight += CalculateSpecular(N, H, shininess) * lightCombined; 
	}
}

//--------------------------------------------------------------------------------
// CSMain 
layout (local_size_x = WORK_GROUP_SIZE , local_size_y = WORK_GROUP_SIZE , local_size_z = 1) in;
void main()
{
	ivec2 fragCoord = ivec2(gl_GlobalInvocationID.xy);

	// Compute view space z
	float zw = texelFetch(DepthBuffer, fragCoord, 0).r;
	float viewSpaceZ = ProjRatio.y / (zw - ProjRatio.x);
	
	// Initialize shared memory light list and Z bounds
	if (gl_LocalInvocationIndex == 0)
	{
		TileMinZ = 0x7F7FFFFF;     
        TileMaxZ = 0;
		NumTileLights = 0;
	}
	barrier();
	
	if (zw < 1.0) // Avoid shading skybox/background pixel
	{
		atomicMin(TileMinZ, floatBitsToUint(viewSpaceZ));
		atomicMax(TileMaxZ, floatBitsToUint(viewSpaceZ));
	}
	barrier();
	
	float minTileZ = uintBitsToFloat(TileMinZ);
	float maxTileZ = uintBitsToFloat(TileMaxZ);
		
	// Construct frustum planes
	vec4 frustumPlanes[6];
	{
		//vec3 frustumCorner[4];

		//frustumCorner[0] = ReconstructViewPosition(1.0, uvec2(gl_WorkGroupID.x * gl_WorkGroupSize.x, gl_WorkGroupID.y * gl_WorkGroupSize.y));
		//frustumCorner[1] = ReconstructViewPosition(1.0, uvec2((gl_WorkGroupID.x+1) * gl_WorkGroupSize.x, gl_WorkGroupID.y * gl_WorkGroupSize.y));
		//frustumCorner[2] = ReconstructViewPosition(1.0, uvec2((gl_WorkGroupID.x+1) * gl_WorkGroupSize.x, (gl_WorkGroupID.y+1) * gl_WorkGroupSize.y));
		//frustumCorner[3] = ReconstructViewPosition(1.0, uvec2(gl_WorkGroupID.x * gl_WorkGroupSize.x, (gl_WorkGroupID.y+1) * gl_WorkGroupSize.y));

		//for(int i=0; i <4; i++)
		//	frustumPlanes[i] = CreatePlaneEquation(frustumCorner[i], frustumCorner[(i+1)&3]);		
		
		vec2 tileScale = vec2(ViewportDim.xy) / (2.0f * vec2(WORK_GROUP_SIZE, WORK_GROUP_SIZE));
		vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

		// Left/Right/Bottom/Top
		frustumPlanes[0] = vec4(Projection[0][0] * tileScale.x, 0, tileBias.x, 0);
		frustumPlanes[1] = vec4(-Projection[0][0] * tileScale.x, 0, 1 - tileBias.x, 0);
		frustumPlanes[2] = vec4(0, Projection[1][1] * tileScale.y, tileBias.y, 0);
		frustumPlanes[3] = vec4(0, -Projection[1][1] * tileScale.y, 1 - tileBias.y, 0);

		for (uint i = 0; i < 4; ++i)
			frustumPlanes[i] /= length(frustumPlanes[i].xyz);

		// Near/Far
		frustumPlanes[4] = vec4(0, 0, 1, -minTileZ);
		frustumPlanes[5] = vec4(0, 0, -1, maxTileZ);
	}
	
	// Cull lights for this tile	
	for (uint lightIndex = gl_LocalInvocationIndex; lightIndex < LightCount; lightIndex += NumGroupThreads)
	{
		vec4 lightPosVS = vec4(Lights[lightIndex].Position, 1.0) * View;

		// Intersect with each frustun plane
		bool inFrustum = true;
		for (int i = 0; i < 6; ++i)
		{
			float d = dot(frustumPlanes[i], vec4(lightPosVS.xyz, 1.0));
			inFrustum = inFrustum && (d >= -Lights[lightIndex].Range);
		}

		if (inFrustum)
		{
			uint listIndex = atomicAdd(NumTileLights, 1);
			if (listIndex < MAX_LIGHTS)		
				TileLightList[listIndex] = lightIndex;		
		}
	}
	barrier();
	
	// Only process onscreen pixels (tiles can span screen edges)
	if (all(lessThan(gl_GlobalInvocationID.xy, ViewportDim.xy)))
	{
		vec3 worldPosition = ReconstructWorldPosition(zw, fragCoord);
		vec3 V = normalize(CameraOrigin - worldPosition);
		
		vec4 tap = texelFetch(GBuffer0, fragCoord, 0);
		vec3 N = normalize(tap.rgb); // Normal
		float shininess = tap.a;

		vec3 diffuseLight = vec3(0);
		vec3 specularLight = vec3(0);
		for (uint i = 0; i < NumTileLights; ++i)
		{
			uint lightIndex = TileLightList[i];
			EvalulateAndAccumilateLight(Lights[lightIndex], worldPosition, N, V, shininess, diffuseLight, specularLight);
		}
		
		imageStore(RWLightAccumulation, fragCoord, vec4(diffuseLight, Luminance(specularLight)));
	}
}
