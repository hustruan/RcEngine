#ifndef ModelCommon_h__
#define ModelCommon_h__

// Attributes
in vec3 iPos;

#ifdef _Skinning
	in vec4 iBlendWeights;
	in uvec4 iBlendIndices;
#endif

in vec3 iNormal;

/**
 * ShadowMapGen pass don't use iNormal, so iNormal attribute will be inactive which 
 * makes iTex attribute slot can't be matched with ModelMesh. So we assign input slot 
 * for iTex attribute explicit.
 */
#if defined(_AlphaTest) && defined(_Skinning)
	layout (location = 4) in vec2 iTex;
#elif defined(_AlphaTest)
	layout (location = 2) in vec2 iTex;
#else
	in vec2 iTex;
#endif

#ifdef _NormalMap
	in vec3 iTangent;
	in vec3 iBinormal;
#endif

// Helper function for skin mesh
#ifdef _Skinning

	#define MaxNumBone 92
	uniform mat4 SkinMatrices[MaxNumBone];

	mat4 CalculateSkinMatrix()
	{
		return SkinMatrices[iBlendIndices[0]] * iBlendWeights[0] +
			   SkinMatrices[iBlendIndices[1]] * iBlendWeights[1] +
			   SkinMatrices[iBlendIndices[2]] * iBlendWeights[2] +
			   SkinMatrices[iBlendIndices[3]] * iBlendWeights[3];
	}

#endif	

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


#endif // ModelCommon_h__