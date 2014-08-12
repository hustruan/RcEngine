[[Vertex=ShadowMapVS]]

#include "/ModelVertexFactory.glsl"

uniform mat4 World;	
uniform mat4 ViewProj;

#ifdef _AlphaTest
	layout (location = 0) out vec2 oTex;
#endif

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
#ifdef _Skinning
	mat4 Skin = CalculateSkinMatrix();
	mat4 SkinWorld = Skin * World;
	gl_Position = vec4(iPos, 1.0)* SkinWorld * ViewProj;
#else
	gl_Position = vec4(iPos, 1.0)* World * ViewProj;
#endif

#ifdef _AlphaTest
	oTex = iTex;
#endif
}

[[Fragment=ShadowMapVSM]]

#include "/ModelMaterialFactory.glsl"

layout (location = 0) in vec2 iTex;
layout (location = 0) out vec2 oFragDepth;

void main()
{
#ifdef _AlphaTest
	vec4 diffuseTap = texture2D(DiffuseMap, iTex);
	if( diffuseTap.a < 0.01 ) discard;
#endif	

	oFragDepth.x = gl_FragCoord.z;
	oFragDepth.y = gl_FragCoord.z * gl_FragCoord.z;
}