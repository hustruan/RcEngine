[[Fragment=GBufferPS]]

#include "/ModelMaterialFactory.glsl"

// PS Inputs
in vec4 oPosWS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToWorld;
#else
	in vec3 oNormalWS;
#endif

// PS Outputs
layout(location = 0) out vec4 oFragColor0; // Normal + Shininess
layout(location = 1) out vec4 oFragColor1; // Diffuse + Specular
	
void main() 
{	
	Material material;
	GetMaterial(oTex, material);
	
	// normal map
#ifdef _NormalMap
	vec3 normal = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
	//normal = normalize(normal * oTangentToWorld);
	normal = normalize(oTangentToWorld[2]);
#else
	vec3 normal = normalize(oNormalWS);
#endif	
	
	// Use RGBA16F
	//normal = normal * 0.5 + 0.5;
	//CompressUnsignedNormalToNormalsBuffer(normal);	

	oFragColor0 = vec4(normal, material.Shininess);
	oFragColor1 = vec4(material.DiffuseAlbedo,  dot(material.SpecularAlbedo, vec3(0.2126, 0.7152, 0.0722)));	 // Specular luminance
}