SamplerState VSMSampler;

float ChebyshevUpperBound(float2 moments, float depth, float minVariance)
{
	float mean = moments.x;
	float measSquared = moments.y;
	
	float p = float(depth <= mean);
	
	float variance = max(minVariance, measSquared - mean*mean);
	float d = depth - mean;
	float pmax = variance / (variance + d * d);
	
	 // To combat light-bleeding, experiment with raising p_max to some power
     // (Try values from 0.1 to 100.0, if you like.)	
	return pow( max(p, pmax), 100.0f );
}


/**
 * CascadeShadowTexCoord.2 == cascade index
 */
float EvalVSM(Texture2DArray CSM, float4 CascadeShadowTexCoord)
{
	float2 moments = CSM.Sample(VSMSampler, CascadeShadowTexCoord).rg;
	return ChebyshevUpperBound(moments, CascadeShadowTexCoord.w, 0.0001);
}

