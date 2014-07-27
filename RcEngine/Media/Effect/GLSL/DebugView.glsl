[[Vertex=VSMain]]

uniform mat4 World;
uniform mat4 ViewProj;

layout (location = 0) in vec3 iPos;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(iPos, 1.0) * World * ViewProj;
} 

[[Fragment=PSMain]]

uniform vec3 Color;

layout (location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(Color, 1.0);
}