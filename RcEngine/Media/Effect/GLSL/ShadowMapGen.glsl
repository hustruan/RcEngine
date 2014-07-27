[[Vertex=ShadowMapVS]]

#include "/ModelVertexFactory.glsl"

uniform mat4 WorldView;	
uniform mat4 Projection;

#ifdef _AlphaTest
	out vec2 oTex;
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

layout(location = 0) out vec2 oFragDepth;

void main()
{
	oFragDepth.x = gl_FragCoord.z;
	oFragDepth.y = gl_FragCoord.z * gl_FragCoord.z;
}