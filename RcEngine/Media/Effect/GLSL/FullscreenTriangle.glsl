#ifndef FullscreenTriangle_h__
#define FullscreenTriangle_h__

out vec2 oTex;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	// Parametrically work out vertex location for full screen triangle
	oTex= vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(oTex * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 0.0, 1.0);

}

#endif // FullscreenTriangle__h__