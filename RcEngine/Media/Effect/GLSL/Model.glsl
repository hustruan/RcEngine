[[Vertex=ModelVS]]

#include "/ModelVertexFactory.glsl"

// Shader uniforms	
uniform mat4 World;	
uniform mat4 ViewProj;

// VS Outputs
out vec4 oPosWS;
out vec2 oTex;

#ifdef _NormalMap
	out mat3 oTangentToWorld;
#else
	out vec3 oNormalWS;
#endif

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
#ifdef _Skinning

	mat4 Skin = CalculateSkinMatrix();
	mat4 SkinWorld = Skin * World;
	
	oPosWS = vec4(iPos, 1.0)* SkinWorld;
	vec3 normal = normalize(iNormal * mat3(SkinWorld));

#else

	oPosWS = vec4(iPos, 1.0) * World;
	vec3 normal = normalize(iNormal * mat3(World));

#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		vec3 tangent = normalize(iTangent * mat3(SkinWorld));
		vec3 binormal = normalize(iBinormal * mat3(SkinWorld));
	#else
		vec3 tangent = normalize(iTangent * mat3(World));
		vec3 binormal = normalize(iBinormal * mat3(World));
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	oTangentToWorld = mat3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	//oTangentToWorld = transpose(oTangentToWorld);
#else
	oNormalWS = normal;
#endif
	
	// texcoord
	oTex = iTex;
	gl_Position = oPosWS * ViewProj;
}