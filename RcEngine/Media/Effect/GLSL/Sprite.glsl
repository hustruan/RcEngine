[[Vertex=SpriteVS]]

uniform vec2 InvWindowSize;

// Shader input
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iTex;
layout (location = 2) in vec4 iColor;

// Shader output
layout (location = 0) out vec2 oTex;
layout (location = 1) out vec4 oColor;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	vec3 ndcPos = iPos * vec3(InvWindowSize, 1.0); 
	ndcPos.y = 1.0 - ndcPos.y;

	oTex = iTex;	
	oColor = iColor;

	// OpenGL NDC z [-1, 1]
	gl_Position = vec4(ndcPos * 2.0 - 1.0, 1.0);
}

[[Fragment=SpritePS]]

uniform sampler2D SpriteTexture;
#pragma SpriteTexture : LinearSampler  // LinearSampler

layout (location = 0) in vec2 iTex;
layout (location = 1) in vec4 iColor;

layout (location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = iColor * texture2D(SpriteTexture, iTex);
}
