
/** Increase to make edges crisper. Decrease to reduce temporal flicker. */
#define EDGE_SHARPNESS     (1.0)

/** Step in 2-pixel intervals since we already blurred against neighbors in the
    first AO pass.  This constant can be increased while R decreases to improve
    performance at the expense of some dithering artifacts. 
    
    Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
    unobjectionable after shading was applied but eliminated most temporal incoherence
    from using small numbers of sample taps.
    */
#define SCALE               (2)

/** Filter radius in pixels. This will be multiplied by SCALE. */
#define R                   (4)

// Gaussian coefficients
static const float gaussian[] = 
//	{ 0.356642, 0.239400, 0.072410, 0.009869 };
//	{ 0.398943, 0.241971, 0.053991, 0.004432, 0.000134 };  // stddev = 1.0
	{ 0.153170, 0.144893, 0.122649, 0.092902, 0.062970 };  // stddev = 2.0
//	{ 0.111220, 0.107798, 0.098151, 0.083953, 0.067458, 0.050920, 0.036108 }; // stddev = 3.0


Texture2D<float> Source;
Texture2D<float> CSZBuffer;

int2 BlurAxis;

void Blur(in float4 iFragCoord : SV_Position, out float oResult : SV_Target0)
{
	int2 ssC = int2(iFragCoord.xy);

	float sum = Source.Load(int3(ssC, 0));
	float key = CSZBuffer.Load(int3(ssC, 0));

	if (key == 1.0)
	{
		oResult = sum;
		return;
	}

	// Base weight for depth falloff.  Increase this for more blurriness,
	// decrease it for better edge discrimination
	float totalWeight = gaussian[0];
	sum *= totalWeight;

	[unroll]
	for (int r = -R; r <= R; ++r)
	{
		// We already handled the zero case above.  This loop should be unrolled and the branch discarded
		if (r != 0) 
		{
			int2 ssP = ssC + BlurAxis * (r * SCALE);

			float value = Source.Load(int3(ssP, 0));
			float tapKey = CSZBuffer.Load(int3(ssP, 0));

			// spatial domain: offset gaussian tap
			float weight = 0.3 + gaussian[abs(r)];

			// range domain (the "bilateral" weight). As depth difference increases, decrease weight.
			weight *= max(0.0, 1.0 - (2000.0 * EDGE_SHARPNESS) * abs(tapKey - key));

			sum += value * weight;
			totalWeight += weight;
		}
	}

	const float epsilon = 0.0001;
	oResult = sum / (totalWeight + epsilon);
}