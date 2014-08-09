[[Vertex=ShadowMapVS]]

#include "/ModelVertexFactory.glsl"

uniform mat4 WorldView;	
uniform mat4 Projection;

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
	gl_Position = vec4(iPos, 1.0) * Skin * WorldView * Projection;
#else
	gl_Position = vec4(iPos, 1.0) * WorldView * Projection;
#endif

#ifdef _AlphaTest
	oTex = iTex;
#endif
}

[[Fragment=ShadowMapVSM]]

#include "/ModelMaterialFactory.glsl"

#ifdef _AlphaTest
	layout (location = 0) in vec2 iTex;
#endif

layout(location = 0) out vec2 oFragDepth;

void main()
{
#ifdef _AlphaTest
	vec4 tap = texture(DiffuseMap, iTex);
	if (tap.a < 0.1) discard;
#endif

	float z = gl_FragCoord.z * 0.5 + 0.5;
	
	oFragDepth.x = z;
	oFragDepth.y = z * z;
}