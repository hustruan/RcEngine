#ifndef BRDF_HLSL__
#define BRDF_HLSL__

// Microfacet distribution function
// 0: Blinn
// 1: Beckmann
// 2: GGX
#define PHYSICAL_SPEC_D		2

// Geometric attenuation or shadowing
// 0: Implicit
// 1: Schlick
// 2: Smith (matched to GGX)
#define PHYSICAL_SPEC_G		2

// Fresnel
// 0: None
// 1: Schlick
// 2: Fresnel
#define PHYSICAL_SPEC_F		1

float Square( float x )
{
	return x*x;
}

float3 Diffuse_Lambert( float3 DiffuseColor )
{
	return DiffuseColor / PI;
}

// [Blinn 1977, "Models of light reflection for computer synthesized pictures"]
float D_Blinn( float Roughness, float NoH )
{
	// m = sqrt(2/(a+2))
	float m = Roughness * Roughness;
	float m2 = m * m;
	float n = 2 / m2 - 2;
	return (n+2) / 2 * pow(NoH, n);
}

// [Beckmann 1963, "The scattering of electromagnetic waves from rough surfaces"]
float D_Beckmann( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float NoH2 = NoH * NoH;
	return exp( (NoH2 - 1) / (m2 * NoH2) ) / ( m2 * NoH2 * NoH2 );
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float d = ( NoH * m2 - NoH ) * NoH + 1;	// 2 mad
	return m2 / ( d*d );					// 2 mul, 1 rcp
}

// G is grouped with the BRDF denominator to get a optimal form.
float G_Implicit()
{
	return 0.25;
}

// Tuned to match behavior of G_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float G_Schlick( float Roughness, float NoV, float NoL )
{
	float k = Square( Roughness ) * 0.5;
	float G_SchlickV = NoV * (1 - k) + k;
	float G_SchlickL = NoL * (1 - k) + k;
	return 0.25 / ( G_SchlickV * G_SchlickL );
}

// Smith term for GGX modified by Disney to be less "hot" for small roughness values
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
// [Burley 2012, "Physically-Based Shading at Disney"]
float G_Smith( float Roughness, float NoV, float NoL )
{
	float a = Square( Roughness );
	float a2 = a*a;

	float G_SmithV = NoV + sqrt( NoV * (NoV - NoV * a2) + a2 );
	float G_SmithL = NoL + sqrt( NoL * (NoL - NoL * a2) + a2 );
	return rcp( G_SmithV * G_SmithL );
}

float3 F_None( float3 SpecularColor )
{
	return SpecularColor;
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
// [Lagarde 2012, "Spherical Gaussian approximation for Blinn-Phong, Phong and Fresnel"]
float3 F_Schlick( float3 SpecularColor, float VoH )
{
	// Anything less than 2% is physically impossible and is instead considered to be shadowing 
	return SpecularColor + ( saturate( 50.0 * SpecularColor.g ) - SpecularColor ) * exp2( (-5.55473 * VoH - 6.98316) * VoH );

	//float Fc = exp2( (-5.55473 * VoH - 6.98316) * VoH );	// 1 mad, 1 mul, 1 exp
	//return Fc + (1 - Fc) * SpecularColor;					// 1 add, 3 mad
}

float3 F_Fresnel( float3 SpecularColor, float VoH )
{
	float3 SpecularColorSqrt = sqrt( clamp( float3(0, 0, 0), float3(0.99, 0.99, 0.99), SpecularColor ) );
	float3 n = ( 1 + SpecularColorSqrt ) / ( 1 - SpecularColorSqrt );
	float3 g = sqrt( n*n + VoH*VoH - 1 );
	return 0.5 * Square( (g - VoH) / (g + VoH) ) * ( 1 + Square( ((g+VoH)*VoH - 1) / ((g-VoH)*VoH + 1) ) );
}

float Distribution( float Roughness, float NoH )
{
#if   PHYSICAL_SPEC_D == 0
	return D_Blinn( Roughness, NoH );
#elif PHYSICAL_SPEC_D == 1
	return D_Beckmann( Roughness, NoH );
#elif PHYSICAL_SPEC_D == 2
	return D_GGX( Roughness, NoH );
#endif
}

float GeometricVisibility( float Roughness, float NoV, float NoL, float VoH, float3 L, float3 V )
{
#if   PHYSICAL_SPEC_G == 0
	return G_Implicit();
#elif PHYSICAL_SPEC_G == 1
	return G_Schlick( Roughness, NoV, NoL );
#elif PHYSICAL_SPEC_G == 2
	return G_Smith( Roughness, NoV, NoL );
#endif
}

#endif