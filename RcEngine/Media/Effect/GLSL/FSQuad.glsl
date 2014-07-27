[[Vertex=VSMain]]

in vec2 iPos;
in vec2 iTex;

out vec2 oTex;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	oTex = iTex;
	gl_Position = vec4(iPos, 0.0, 1.0);
}

[[Fragment=PSMain]]
#pragma ColorMap : LinearSampler

uniform sampler2D ColorMap;

in vec2 oTex;

out vec4 oFragColor;

void main()
{
	oFragColor = texture(ColorMap, oTex);
}

