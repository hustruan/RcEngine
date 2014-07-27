[[Vertex=BezierVS]]

layout (location=0) in vec2 iControlPoint;
layout (location=0) out vec2 oControlPoint;

void main ()
{
	oControlPoint = iControlPoint;
}

[[TessControl=BezierTCS]]

layout( vertices=4 ) out;

uniform int NumSegments;
uniform int NumStrips;

layout (location=0) in vec2 iControlPoint[];
layout (location=0) out vec2 oControlPoint[];

void main()
{
    // Pass along the vertex position unmodified
    oControlPoint[gl_InvocationID] = iControlPoint[gl_InvocationID];

    gl_TessLevelOuter[0] = float(NumStrips);
    gl_TessLevelOuter[1] = float(NumSegments);
}

[[TessEval=BezierTES]]

layout( isolines ) in;

uniform vec2 ViewportDim;

layout (location=0) in vec2 iControlPoint[];

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main()
{
    float u = gl_TessCoord.x;

    vec2 p0 = iControlPoint[0];
    vec2 p1 = iControlPoint[1];
    vec2 p2 = iControlPoint[2];
    vec2 p3 = iControlPoint[3];

    float u1 = (1.0 - u);
    float u2 = u * u;

    // Bernstein polynomials
    float b3 = u2 * u;
    float b2 = 3.0 * u2 * u1;
    float b1 = 3.0 * u * u1 * u1;
    float b0 = u1 * u1 * u1;

    // Cubic Bezier interpolation
    vec2 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;

	gl_Position = vec4(p.xy / ViewportDim * 2.0, 0.0, 1.0);
}

[[Fragment=BezierePS]]

layout (location=0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(1, 0, 0, 1);
}