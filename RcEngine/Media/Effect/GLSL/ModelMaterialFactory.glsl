#ifndef ModelMaterialFactory_GLSL__
#define ModelMaterialFactory_GLSL__

// Model material input
#ifdef _DiffuseMap
	uniform sampler2D DiffuseMap;
	// SamplerState binding
	#pragma DiffuseMap : MaterialSampler 
#else 
	uniform vec3 DiffuseColor;
#endif

#ifdef _SpecularMap
	uniform sampler2D SpecularMap;
	// SamplerState binding
	#pragma SpecularMap : MaterialSampler 
#else
	uniform vec3 SpecularColor;
#endif

#ifndef _GlossyMap
	uniform float Shininess;
#endif	

#ifdef _NormalMap
	uniform sampler2D NormalMap;
	// SamplerState binding
	#pragma NormalMap : MaterialSampler 
#endif

// Material definition
struct Material
{
	vec3 DiffuseAlbedo;
	vec3 SpecularAlbedo;
	float Shininess;
};

void GetMaterial(in vec2 iTex, out Material oMaterial)
{
	// diffuse material
#ifdef _DiffuseMap
	vec4 diffuseTap = texture2D(DiffuseMap, iTex);
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
	oMaterial.DiffuseAlbedo = diffuseTap.rgb;
#else
	oMaterial.DiffuseAlbedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	vec4 specularTap = texture2D(SpecularMap, iTex);
	//vec3 specular = specularTap.rgb;
	oMaterial.SpecularAlbedo = specularTap.rrr;
	#ifdef _GlossyMap
		oMaterial.Shininess = specularTap.a * 255;
	#else 
		oMaterial.Shininess = Shininess;
	#endif
#else
	oMaterial.SpecularAlbedo = SpecularColor;
	oMaterial.Shininess = Shininess;
#endif
}

#endif // ModelMaterialFactory_GLSL__