#ifndef DeferredPath_GLSL__
#define DeferredPath_GLSL__

uniform vec4 LightPos;		// w dimension is spot light inner cone cos angle
uniform vec4 LightDir;		// w dimension is spot light outer cone cos angle
uniform vec3 LightColor;
uniform vec3 LightFalloff; 

uniform sampler2D GBuffer0;	
uniform sampler2D GBuffer1;
uniform sampler2D DepthBuffer;	

uniform mat4 InvViewProj;
uniform vec3 CameraOrigin;
uniform vec2 ProjRatio;

float GetLinearDepth(ivec2 texelPos)
{
	float zBuffer = texelFetch(DepthBuffer, texelPos, 0).x;
	return ProjRatio.y / (zBuffer - ProjRatio.x);
}

// Reconstruct world position from zbuffer
vec3 ReconstructWorldPosition(ivec2 texelPos, in vec4 clipPos)
{
	float zBuffer = texelFetch(DepthBuffer, texelPos, 0).x;
	vec4 worldPositionH = vec4(clipPos.xy / clipPos.w, zBuffer, 1.0) * InvViewProj;
	return vec3(worldPositionH.xyz / worldPositionH.w);
}

void GetNormalAndShininess(in ivec2 texelPos, out vec3 oNormal, out float oShininess)
{
	vec4 tap = texelFetch(GBuffer0, texelPos, 0);
	
	// RGBA16F 
	//oNormal = normalize(tap.xyz * 2.0 - 1.0);	 // World Space Normal
	oNormal = normalize(tap.xyz);
	oShininess = tap.a /** 255.0*/;  			 // Specular power
}

void GetDiffuseAndSpecular(in ivec2 texelPos, out vec3 oDiffuse, out vec3 oSpecular)
{
	vec4 tap = texelFetch(GBuffer1, texelPos, 0);
	// Get Diffuse Albedo and Specular
	oDiffuse = tap.rgb;
	oSpecular = tap.aaa;
}

#endif //DeferredPath_GLSL__