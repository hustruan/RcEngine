
// Make sure that attribe location match VertexStream
#define POSISTION 0

// Attributes
layout (location = POSISTION) in vec3 iPos;

#ifdef _Skinning
	#define BLENDWEIGHTS (POSISTION+1)
	#define BLENDINDICES (BLENDWEIGHTS+1)
	layout (location = BLENDWEIGHTS) in vec4 iBlendWeights;
	layout (location = BLENDINDICES) in uvec4 iBlendIndices;
#endif

#ifdef _Skinning
	#define NORMAL (BLENDINDICES+1)
#else
	#define NORMAL (POSISTION+1)
#endif

layout (location = NORMAL) in vec3 iNormal;

#define TEXCOORD (NORMAL+1)
layout (location = TEXCOORD) in vec2 iTex;

#ifdef _NormalMap
	#define TANGENT (TEXCOORD+1)
	#define BINORMAL (TANGENT+1)
	layout (location = TANGENT) in vec3 iTangent;
	layout (location = BINORMAL) in vec3 iBinormal;
#endif

// Helper function for skin mesh
#ifdef _Skinning

	#define MaxNumBone 92
	//uniform mat4 SkinMatrices[MaxNumBone];

	layout(std140) uniform SkinCB
	{
		mat4 SkinMatrices[MaxNumBone];
	};

	mat4 CalculateSkinMatrix()
	{
		return SkinMatrices[iBlendIndices[0]] * iBlendWeights[0] +
			   SkinMatrices[iBlendIndices[1]] * iBlendWeights[1] +
			   SkinMatrices[iBlendIndices[2]] * iBlendWeights[2] +
			   SkinMatrices[iBlendIndices[3]] * iBlendWeights[3];
	}

#endif	
