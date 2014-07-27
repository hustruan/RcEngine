[[Vertex=VSMain]]

uniform mat4 ViewProj;

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTex;
layout(location = 3) in mat4 iTransfrom; // Per-Instance

layout(location = 0) out vec4 oPosWS;
layout(location = 1) out vec3 oNormalWS;
layout(location = 2) out vec2 oTex;

out gl_PerVertex  {
	vec4 gl_Position;
};

void main() 
{
	oPosWS = iPos * iTransfrom;
	oNormalWS = iNormal * mat3(iTransfrom);
	iTex = oTex;

	gl_Position = oPosWS * ViewProj;
}
