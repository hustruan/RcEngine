Texture2D UnfilteredValueBuffer;
Texture2D PreviousValueBuffer;

Texture2D DepthBuffer;
Texture2D PreviousDepthBuffer;

Texture2D SSVelocityBuffer;

float4x4 InvView;
float4x4 PreviousInvView;

float4 ProjInfo;
float2 ClipInfo;
float2 BufferSize;

float Alpha;
float FalloffStartDistance;
float FalloffEndDistance;

float3 reconstructCameraSpacePosition(float2 S, float z)
{
	float csz = ClipInfo.y / ( z - ClipInfo.x);
	return float3((S.xy * ProjInfo.xy + ProjInfo.zw) * csz, csz);
}

void TemporalFilter(in float2 iTex	   : TEXCOORD0,
					in float4 iFragCoord : SV_Position,
					out float4 oFragColor : SV_Target0)
{    
	float2 screenCoord = iFragCoord.xy;
	int2 C = int2(iFragCoord.xy);

	float4 currentVal = UnfilteredValueBuffer.Load(int3(C, 0)); 
	float depth = DepthBuffer.Load(int3(C, 0)).r; 
    float3 currentCSPosition = reconstructCameraSpacePosition(screenCoord, depth);
	
	float2 ssV = SSVelocityBuffer.Load(int3(C, 0)).rg;
    float2 previousCoord = screenCoord - ssV;
	int2 previousC = int2(previousCoord);

    if (previousCoord.x < 0.0 || previousCoord.y < 0.0 ||
        previousCoord.x > BufferSize.x || previousCoord.y > BufferSize.y) 
	{ // outside guard band
        oFragColor = currentVal;
        return;
    }

	float4 previousVal = PreviousValueBuffer.Load(int3(previousC, 0)); 
    float previousDepth = PreviousDepthBuffer.Load(int3(previousC, 0)).r;
	float3 previousCSPosition = reconstructCameraSpacePosition(previousCoord, previousDepth);

	float4 currentWSPosition = mul( float4(currentCSPosition, 1.0), InvView );
	float4 previousWSPosition = mul( float4(previousCSPosition, 1.0), PreviousInvView );
    float distance = length(currentWSPosition.rgb - previousWSPosition.rgb);

	float weight = Alpha * (1.0 - smoothstep(FalloffStartDistance, FalloffEndDistance, distance));
	float4 result = weight * previousVal + (1.0 - weight) * currentVal;

	oFragColor = result;
}