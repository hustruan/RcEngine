[[Vertex=TriangleVS]]
layout (location=0) in vec2 iControlPoint;
uniform vec4 ViewportDim;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main ()
{
	gl_Position = vec4(iControlPoint * ViewportDim.zw * 2.0, 0.0, 1.0);
}

[[Vertex=BezierVS]]

layout (location=0) in vec2 iControlPoint;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main ()
{
	gl_Position.xy = iControlPoint;
}

[[TessControl=BezierTCS]]

layout( vertices=4 ) out;

uniform int NumSegments;
uniform int NumStrips;

in gl_PerVertex 
{
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex 
{
	vec4 gl_Position;
} gl_out[];

void main()
{
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = float(NumStrips);
    gl_TessLevelOuter[1] = float(NumSegments);
}

[[TessEval=BezierTES]]

layout( isolines ) in;

uniform vec4 ViewportDim;

in gl_PerVertex 
{
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main()
{
    float u = gl_TessCoord.x;

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    float u1 = (1.0 - u);
    float u2 = u * u;

    // Bernstein polynomials
    float b3 = u2 * u;
    float b2 = 3.0 * u2 * u1;
    float b1 = 3.0 * u * u1 * u1;
    float b0 = u1 * u1 * u1;

    // Cubic Bezier interpolation
    vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;

	gl_Position = vec4(p.xy * ViewportDim.zw * 2, 0.0, 1.0);
}

[[Fragment=BezierePS]]

layout (location=0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(1, 0, 0, 1);
}