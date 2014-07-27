
#ifdef _DiffuseMap
	Texture2D DiffuseMap;
#else
	float3 DiffuseColor;
#endif

#ifdef _SpecularMap
	Texture2D SpecularMap;
#else
	float3 SpecularColor;
#endif

#ifndef _GlossyMap
	float Shininess;
#endif	

#ifdef _NormalMap
	Texture2D NormalMap;
#endif

#if defined(_DiffuseMap) || defined(_SpecularMap) || defined(_NormalMap)
	SamplerState MaterialSampler;
#endif

// Material definition
struct Material
{
	float3 DiffuseAlbedo;
	float3 SpecularAlbedo;
	float Shininess;
};

void  GetMaterial(in float2 iTex, out Material material)
{
	 // diffuse material
#ifdef _DiffuseMap
	float4 diffuseTap = DiffuseMap.Sample(MaterialSampler, iTex);
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
	material.DiffuseAlbedo = diffuseTap.rgb;
#else
	material.DiffuseAlbedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	float4 specularTap = SpecularMap.Sample(MaterialSampler, iTex);
	//vec3 specular = specularTap.rgb;
	material.SpecularAlbedo = specularTap.rrr;
	#ifdef _GlossyMap
		material.Shininess = specularTap.a * 255;
	#else 
		material.Shininess = Shininess;
	#endif
#else
	material.SpecularAlbedo = SpecularColor;
	material.Shininess = Shininess;
#endif
}