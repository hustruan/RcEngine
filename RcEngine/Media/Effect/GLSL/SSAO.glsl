[[Fragment=AlchemyAO]]

// Total number of direct samples to take at each pixel
#define NUM_SAMPLES (11)

// This is the number of turns around the circle that the spiral pattern makes.  This should be prime to prevent
// taps from lining up.  This particular choice was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS (7)


uniform sampler2D DepthBuffer;

vec3 ReconstructViewPosition(uvec2 fragCoord)
{
	vec2 clipPos = (vec2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	
	vec4 viewPositionH = vec4(clipPos, zBuffer, 1.0) * InvProj;
    return viewPositionH.xyz / viewPositionH.w; 
}

void main()
{
	ivec2 iFragCoord = ivec2(gl_FragCoord.xy);
	
	float3 C = FetchEyePos(iTex);

	// Hash function used in the HPG12 AlchemyAO paper
    float randomPatternRotationAngle = (3 * iFragCoord.x ^ iFragCoord.y + iFragCoord.x * iFragCoord.y) * 10;

}