[[Compute=LightCullCSMain]]

#extension GL_ARB_compute_shader : enable

#define WORK_GROUP_SIZE  32
#define NumGroupThreads (WORK_GROUP_SIZE  * WORK_GROUP_SIZE ) // Toal threads per Group
#define MAX_LIGHTS 256   // Max lights per tile

// Uniforms
layout (std140) uniform CSConstants
{
	mat4 View;
	mat4 Projection;
	mat4 InvProj;
	mat4 InvViewProj;
	vec4 ViewportDim;	 // zw for invDim
	vec2 ProjRatio;
	uint LightCount;
};

// Shader Resource View
uniform sampler2D DepthBuffer;
uniform samplerBuffer PointLightsPosRange; // Position and Range

// Unorder Access View
layout (r32ui) uniform uimageBuffer RWLightIndexCounter;
layout (r32ui) uniform writeonly uimageBuffer RWLightIndexList;
layout (rg32ui) uniform writeonly uimageBuffer RWLightListRange;

// Group Shared 
shared uint TileMinZ;
shared uint TileMaxZ;

shared uint TileLightList[MAX_LIGHTS];
shared uint NumTileLights;
shared uint LightIndexStart;

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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
		NumTileLights = 0;
        TileMinZ = 0x7F7FFFFF;      // Max float;
        TileMaxZ = 0;
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
		vec4 lightPosAndRange = texelFetch(PointLightsPosRange, int(lightIndex));
		
		vec4 lightPosVS = vec4(lightPosAndRange.xyz, 1.0) * View;
		float lightRange = lightPosAndRange.w;

		// Intersect with each frustun plane
		bool inFrustum = true;
		for (int i = 0; i < 6; ++i)
		{
			float d = dot(frustumPlanes[i], vec4(lightPosVS.xyz, 1.0));
			inFrustum = inFrustum && (d >= -lightRange);
		}

		if (inFrustum)
		{
			uint listIndex = atomicAdd(NumTileLights, 1);
			if (listIndex < MAX_LIGHTS)		
				TileLightList[listIndex] = lightIndex;		
		}
	}
	barrier();
	
	// Write shared light list index to global memory
	uint startOffset = 0;
	if (gl_LocalInvocationIndex == 0)
	{
		uint tileIdx = gl_WorkGroupID.y * WORK_GROUP_SIZE + gl_WorkGroupID.x;
	
		if (NumTileLights > 0)
			startOffset = imageAtomicAdd(RWLightIndexCounter, 0, NumTileLights);

		// Store tile light index range
		imageStore(RWLightListRange, int(tileIdx), uvec4(NumTileLights, startOffset, 0, 0));
		LightIndexStart = startOffset;
	}
	barrier();
	startOffset = LightIndexStart;

	for (uint i = gl_LocalInvocationIndex; i < NumTileLights; i += NumGroupThreads)
		imageStore(RWLightIndexList, int(startOffset+i), uvec4(TileLightList[i]));	
}

[[Fragment=ForwardShadingPSMain]]

#define WORK_GROUP_SIZE 32

#include "/ModelMaterialFactory.glsl"
#include "/LightingUtil.glsl"

// Turn on early fragment testing
layout (early_fragment_tests) in;

// Uniform
uniform vec3 CameraOrigin;

uniform samplerBuffer PointLightsPosRange;
uniform samplerBuffer PointLightsColor;
uniform samplerBuffer PointLightsFalloff;

uniform usamplerBuffer LightIndexList;
uniform usamplerBuffer LightListRange;

// Varyings
//layout (location = 0) in vec4 oPosWS;
//layout (location = 1) in vec2 oTex;
in vec4 oPosWS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToWorld;
#else
	in vec3 oNormalWS;
#endif

layout(location = 0) out vec4 oFragColor;

//--------------------------------------------------------------------------------------------
void EvalulateAndAccumilateLight(in int lightIndex, in vec3 litPos, in vec3 N, in vec3 V, in vec3 specularAlbedo,
								 in float shininess, inout vec3 diffuseLight, inout vec3 specularLight)
{
	vec4 lightPosRange = texelFetch(PointLightsPosRange, lightIndex);
	
	vec3 L = lightPosRange.xyz - litPos;
	float dist = length(L);
	if (dist < lightPosRange.w)
	{
		L = normalize(L);
		
		vec3 lightColor = texelFetch(PointLightsColor, lightIndex).xyz;
		vec3 lightFalloff = texelFetch(PointLightsFalloff, lightIndex).xyz;

		vec3 lightCombined = lightColor * saturate(dot(N,L)) * CalcAttenuation(dist, lightFalloff);

		diffuseLight += lightCombined;

		// Frensel in moved to calculate in shading pass
		vec3 H = normalize(V + L);
		vec3 fresnel = CalculateFresnel(specularAlbedo, L, H);
		specularLight += CalculateSpecular(N, H, shininess) * fresnel * lightCombined; 
	}
}

void main()
{
	Material material;
	GetMaterial(oTex, material);
     
    // normal map
#ifdef _NormalMap
    vec3 N = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
    N = normalize(N * oTangentToWorld);
#else
    vec3 N = normalize(oNormalWS);
#endif          
     
	vec3 V = normalize(CameraOrigin - oPosWS.xyz);

	vec3 diffuseLight = vec3(0);
	vec3 specularLight = vec3(0);
	 
	ivec2 tileXY = ivec2(gl_FragCoord.xy) / ivec2(WORK_GROUP_SIZE, WORK_GROUP_SIZE);
	int tileIdx = tileXY.y * WORK_GROUP_SIZE + tileXY.x;

	uvec2 tileLightRange = texelFetch(LightListRange, tileIdx).xy;
	for (int i = 0; i < int(tileLightRange.x); ++i)
	{
		int lightIndex = int(tileLightRange.y) + i;
		int globalLightIndex = int(texelFetch(LightIndexList, lightIndex).x);

		EvalulateAndAccumilateLight(globalLightIndex, oPosWS.xyz, N, V, material.SpecularAlbedo,
		                            material.Shininess, diffuseLight, specularLight);
	}
	 
	vec3 final = diffuseLight * material.DiffuseAlbedo;
	
	float specularNormTerm = (material.Shininess + 2.0) / 8.0;
	final += specularNormTerm * specularLight;
	final += material.DiffuseAlbedo * 0.1; // Ambient

	oFragColor = vec4(final, 1.0);  	         
}
