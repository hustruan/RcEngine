[[Vertex=VSMain]]
layout (location = 0) in vec3 iPos;

out VSOutput
{
	vec3 Pos;
} vsOutput;

void main()
{
	vsOutput.Pos = iPos;
}

[[TessControl=TCMain]]
layout(vertices = 3) out;

uniform float TessLevelInner;
uniform float TessLevelOuter;

in VSOutput
{
	vec3 Pos;
} tcInput[];

out TCOutput
{
	vec3 Pos;
} tcOutput[];

void main()
{
	tcOutput[gl_InvocationID].Pos = tcInput[gl_InvocationID].Pos;
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
	}
}

[[TessEval=TEMain]]
layout(triangles, equal_spacing, cw) in;

uniform mat4 World;
uniform mat4 ViewProj;

out TCOutput
{
	vec3 Pos;
} teInput[];

out vec4 oPosWS;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main()
{
	vec3 p0 = gl_TessCoord.x * teInput[0].Pos;
    vec3 p1 = gl_TessCoord.y * teInput[1].Pos;
    vec3 p2 = gl_TessCoord.z * teInput[2].Pos;

    vec3 tePosition = normalize(p0 + p1 + p2);

    oPosWS = vec4(tePosition, 1.0) * World;
	gl_Position = oPosWS * ViewProj;
}

