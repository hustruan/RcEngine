[[Vertex=DirectionalVSMain]]

uniform mat4 InvViewProj;

out gl_PerVertex {
    vec4 gl_Position;
};

out vec3 oViewRay;

// Fullscreen triangle 
void main()
{
	vec2 grid = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec2 ndcXY = grid * vec2(2.0, -2.0) + vec2(-1.0, 1.0);

	gl_Position = vec4(ndcXY, 0.0, 1.0);
	oViewRay = vec3(vec4(ndcXY, 1.0, 0.0) * InvViewProj);
}

[[Vertex=LightVolumeVSMain]]

uniform mat4 WorldViewProj;

layout(location = 0) in vec3 iPos;

out gl_PerVertex {
    vec4 gl_Position;
};

// Clip space position
out vec4 oPosCS;

void main()
{
	gl_Position = vec4(iPos, 1.0) * WorldViewProj;
	oPosCS = gl_Position;
}

[[Fragment=DirectionalPSMain]]

#include "/DeferredUtil.glsl"
#include "/LightingUtil.glsl"
	
in vec3 oViewRay;

// Output
layout(location = 0) out vec4 oFragColor;

void main()
{
	ivec2 sampleIndex = ivec2(gl_FragCoord.xy);

	vec3 viewRay = normalize(oViewRay);

	// Convert non-linear depth to view space linear depth
	float viewZ = GetLinearDepth(sampleIndex);
	vec3 worldPosition = CameraOrigin + viewRay * viewZ;
	
	// Decode normal and shininess from GBuffer
	vec3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);

	// Light accumulate in HDR Buffer
	oFragColor = vec4(0);

	vec3 L = normalize(-LightDir.xyz);
	float nDotl = dot(L, N);
	if (nDotl > 0.0)
	{
		vec3 V = -viewRay;
		vec3 H = normalize(V + L);
		
		vec3 diffuse = LightColor * nDotl;
		
		// Frensel in moved to calculate in shading pass
		vec3 specular = CalculateSpecular(N, H, shininess) * LightColor * nDotl;

		oFragColor = vec4(diffuse, Luminance(specular));
	}
}	

[[Fragment=PointLightingPSMain]]

#include "/LightingUtil.glsl"
#include "/DeferredUtil.glsl"

// Varying
in vec4 oPosCS;

layout(location = 0) out vec4 oFragColor;

void main()
{
	ivec2 sampleIndex = ivec2(gl_FragCoord.xy);
		
	vec3 worldPosition = ReconstructWorldPosition(sampleIndex, oPosCS);
	vec3 L = normalize(LightPos.xyz - worldPosition);
	
	// Decode normal and shininess from GBuffer
	vec3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);
	
	// Light accumulate in HDR Buffer
	oFragColor = vec4(0);

	float NdotL = dot(L, N);
	if (NdotL > 0.0)
	{
		vec3 V = normalize(CameraOrigin - worldPosition);
		vec3 H = normalize(V + L);
		
		// calculate attenuation
		float attenuation = CalcAttenuation(LightPos.xyz, worldPosition, LightFalloff);
		
		vec3 diffuseLight = LightColor * NdotL * attenuation;	
		vec3 specularLight = CalculateSpecular(N, H, shininess) * diffuseLight; // Frensel in moved to calculate in shading pass
		
		oFragColor = vec4(diffuseLight, Luminance(specularLight));
	}
}	

[[Fragment=SpotLightingPSMain]]

#include "/DeferredUtil.glsl"
#include "/LightingUtil.glsl"

// Varying
in vec4 oPosCS;

layout(location = 0) out vec4 oFragColor;

void main()
{
	// Light accumulate in HDR Buffer
	oFragColor = vec4(0);

	ivec2 sampleIndex = ivec2(gl_FragCoord.xy);

	vec3 worldPosition = ReconstructWorldPosition(sampleIndex, oPosCS);
	vec3 L = normalize(LightPos.xyz - worldPosition);

	// Decode normal and shininess from GBuffer
	vec3 N;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);

	float spot = SpotLighting(L, LightDir.xyz, vec2(LightPos.w, LightDir.w));
	if(spot > 0.0)
	{
		float NdotL = dot(L, N);
		if (NdotL > 0.0)
		{
			vec3 V = normalize(CameraOrigin - worldPosition);
			vec3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = spot * CalcAttenuation(LightPos.xyz, worldPosition, LightFalloff);
		
			vec3 diffuse = LightColor * NdotL * attenuation;
		
			// Frensel in moved to calculate in shading pass
			vec3 specular = CalculateSpecular(N, H, shininess) * LightColor * NdotL * attenuation;
		
			oFragColor = vec4(diffuse, Luminance(specular));
		}
	}
}	


[[Fragment=DeferredShadingPSMain]]

#include "/DeferredUtil.glsl"
#include "/LightingUtil.glsl"

uniform sampler2D LightAccumulateBuffer;

in vec3 oViewRay;

layout(location = 0) out vec4 oFragColor;

// Deferred shading pass
void main()
{
	ivec2 sampleIndex = ivec2(gl_FragCoord.xy);

	vec3 V = normalize(-oViewRay);

	// Decode normal and shininess from GBuffer
	vec3 N, diffuseAlbedo, specularAlbedo;
	float shininess;
	GetNormalAndShininess(sampleIndex, N, shininess);
	GetDiffuseAndSpecular(sampleIndex, diffuseAlbedo, specularAlbedo);
	
	vec4 lightColor = texelFetch( LightAccumulateBuffer, sampleIndex, 0 );
	                    
	vec3 diffueLight = lightColor.rgb;
	vec3 specularLight = lightColor.a / (Luminance(diffueLight) + 1e-6) * diffueLight;

	// Approximate fresnel by N and V
	vec3 fresnelTerm = CalculateAmbiemtFresnel(specularAlbedo, N, V);
	
	vec3 final = vec3(0);
	final =  diffueLight * diffuseAlbedo + ((shininess + 2.0) / 8.0) * fresnelTerm * specularLight;
	final += vec3(0.1, 0.1, 0.1) * diffuseAlbedo;

	oFragColor = vec4(final, 1.0);
}
