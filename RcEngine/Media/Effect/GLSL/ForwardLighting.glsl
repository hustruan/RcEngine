[[Fragment=DirectionalLightingPS]]

#include "/ModelMaterialFactory.glsl"
#include "/LightingUtil.glsl"

uniform vec4 LightDir;
uniform vec3 LightColor;
uniform vec3 CameraOrigin;

// shader input
in vec4 oPosWS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToWorld;
#else
	in vec3 oNormalWS;
#endif

layout(location = 0) out vec4 oFragColor;

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
        
    vec3 L = normalize(-LightDir.xyz);
    vec3 V = normalize(CameraOrigin - oPosWS.xyz);
    vec3 H = normalize(V + L);
        	         
    vec3 final = vec3(0);   
    float NdotL = dot(L, N);
    if (NdotL > 0.0)
    {	
		float normTerm = (material.Shininess + 2.0) / 8.0;
        float fresnel = CalculateFresnel(material.SpecularAlbedo, L, H);

		// Diffuse + Specular
        final = (material.DiffuseAlbedo + normTerm * CalculateSpecular(N, H, material.Shininess) * fresnel) * LightColor * NdotL;
	}

	// Ambient
	final += material.DiffuseAlbedo * 0.1;

	oFragColor = vec4(final, 1.0);

   // oFragColor = vec4(LightColor * NdotL, final.r);
}