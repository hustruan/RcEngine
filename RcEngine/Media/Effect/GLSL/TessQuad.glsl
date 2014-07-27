[[Vertex=PassThroughVS]]

layout (location=0) in vec2 iPos;

out gl_PerVertex {
	vec4 gl_Position;
};

void main ()
{
	gl_Position = vec4(iPos, 0, 1);
}

[[TessControl=TCSQuad]]

layout( vertices=4 ) out;

uniform vec4 TessLevelOuter;
uniform vec2 TessLevelInner;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
	vec4 gl_Position;
} gl_out[];

void main ()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = TessLevelOuter[0];
		gl_TessLevelOuter[1] = TessLevelOuter[1];
		gl_TessLevelOuter[2] = TessLevelOuter[2];
		gl_TessLevelOuter[3] = TessLevelOuter[3];

		gl_TessLevelInner[0] = TessLevelInner[0];
		gl_TessLevelInner[1] = TessLevelInner[1];
	}
}

[[TessEval=TESQuad]]

layout( quads, equal_spacing, ccw ) in;

uniform vec2 ViewportDim;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
	vec4 gl_Position;
};

void main ()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	gl_Position = p0 * (1-u) * (1-v) + 
	              p1 * u * (1-v) +
				  p2 * u * v +
				  p3 * v * (1-u);

	gl_Position.xy  = gl_Position.xy / ViewportDim * 2.0;
}

[[Fragment=PSQuad]]

layout (location=0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(1, 0, 0, 1);
}