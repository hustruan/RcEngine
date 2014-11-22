#include "ModelVertexFactory.hlsl"
#include "ModelMaterialFactory.hlsl"
#include "LightingUtil.hlsl"
#include "PSSM.hlsl"

float3 LightColor;
float4 LightDir;
float3 CameraOrigin;

bool ShadowEnabled;

static const float3 vCascadeColorsMultiplier[4] = {

    float3 ( 1.5, 0.0, 0.0 ),
    float3 ( 0.0, 1.5, 0.0 ),
    float3 ( 0.0, 0.0, 1.5 ),
    float3 ( 1.5, 1.5, 0.0 )
};

void DirectionalLightingPS(in VSOutput input,
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
        
    float3 L = normalize(-LightDir.xyz);
    float3 V = normalize(CameraOrigin - input.PosWS.xyz);
    float3 H = normalize(V + L);
                          
    float3 final = 0;   
    float NdotL = dot(L, N);
    if (NdotL > 0.0)
    {
		float normTerm = (material.Shininess + 2.0) / 8.0;
        float3 fresnel = CalculateFresnel(material.SpecularAlbedo, L, H);
        final = (material.DiffuseAlbedo /*+ normTerm * CalculateSpecular(N, H, material.Shininess) * fresnel*/) * LightColor * NdotL;
    }

	if (ShadowEnabled)
	{
		int iCascadeSelected = 0;
		float percentLit = EvalCascadeShadow(input.PosWS, iCascadeSelected);
		final *= percentLit; 
		//final = final * vCascadeColorsMultiplier[iCascadeSelected];
	}
    
	// Ambient 
	final += material.DiffuseAlbedo * 0.15;

	//oFragColor = float4(material.DiffuseAlbedo, final.r);
    oFragColor = float4(final, 1.0);
}