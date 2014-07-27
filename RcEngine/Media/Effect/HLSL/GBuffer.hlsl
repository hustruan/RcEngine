#include "ModelVertexFactory.hlsl"
#include "ModelMaterialFactory.hlsl"

void GBufferPS(in VSOutput input,
               out float4 oFragColor0 : SV_Target0,
			   out float4 oFragColor1 : SV_Target1 )
{
	Material material;
	GetMaterial(input.Tex, material);
	
	// normal map
#ifdef _NormalMap
	float3 normal = NormalMap.Sample(MaterialSampler , input.Tex ).rgb * 2.0 - 1.0;
	//normal = normalize( mul(normal, input.TangentToWorld) );
	normal = normalize( input.TangentToWorld[2] );
#else
	float3 normal = normalize(input.NormalWS);
#endif	
	
	// Use RGBA16F
	//normal = normal * 0.5 + 0.5;
	//CompressUnsignedNormalToNormalsBuffer(normal);	

	oFragColor0 = float4(normal, material.Shininess / 255.0);
	oFragColor1 = float4(material.DiffuseAlbedo, dot(material.SpecularAlbedo, float3(0.2126, 0.7152, 0.0722)) );	 // Specular luminance
}
