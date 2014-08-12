[[Fragment=DirectionalLightingPS]]

#include "/ModelMaterialFactory.glsl"
#include "/LightingUtil.glsl"
#include "/PSSM.glsl"

uniform vec4 LightDir;
uniform vec3 LightColor;
uniform vec3 CameraOrigin;
uniform bool ShadowEnabled;

const vec3 vCascadeColorsMultiplier[4] = vec3[4](
    vec3 ( 1.5, 0.0, 0.0 ),
    vec3 ( 0.0, 1.5, 0.0 ),
    vec3 ( 0.0, 0.0, 1.5 ),
    vec3 ( 1.5, 1.5, 0.0 )
);

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

	if (ShadowEnabled)
	{
		int iCascadeSelected = 0;
        float percentLit = EvalCascadeShadow(oPosWS, iCascadeSelected);
		final *= percentLit; 
		//final = final  * max(1.0, percentLit);
		//final = final * vCascadeColorsMultiplier[iCascadeSelected];
	}
	
	// Ambient
	final += material.DiffuseAlbedo * 0.1;

	//final = pow(final, vec3(1.0 / 2.2));
	oFragColor = vec4(final, 1.0);
}