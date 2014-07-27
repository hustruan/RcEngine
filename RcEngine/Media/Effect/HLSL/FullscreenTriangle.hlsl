#ifndef FullscreenTriangle_h__
#define FullscreenTriangle__h__

void FullscreenTriangleVS(uint iVertexID    : SV_VertexID,
						  out float2 oTex   : TEXCOORD0,
						  out float4 oPosCS : SV_Position)
{
    // Parametrically work out vertex location for full screen triangle
    float2 grid = float2((iVertexID << 1) & 2, iVertexID & 2);
    
	oTex = grid;
	oPosCS = float4(grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
}

#endif // FullscreenTriangle__h__