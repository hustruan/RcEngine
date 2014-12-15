float signNotZero(in float k) {
    return k >= 0.0 ? 1.0 : -1.0;
}

float2 signNotZero(in float2 v) {
    return float2( signNotZero(v.x), signNotZero(v.y) );
}

float2 octEncode(in float3 v) {
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    float2 result = v.xy * (1.0/l1norm);
    if (v.z < 0.0) {
        result = (1.0 - abs(result.yx)) * signNotZero(result.xy);
    }
    return result;
}

float3 finalDecode(float x, float y) {
    float3 v = float3(x, y, 1.0 - abs(x) - abs(y));
    if (v.z < 0) {
        v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);
    }
    return normalize(v);
}

float2 encode16(in float3 v) {
    return octEncode(v);  
}

float3 decode16(in float2 p) {
    return finalDecode(p.x, p.y);
}