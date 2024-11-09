#ifndef _PBR__HLSLI_
#define _PBR__HLSLI_

#include "types.hlsli"

static const float3 Fdielectric = 0.04f;	
static const float PI = 3.141592f;
static const float Epsilon = 0.00001f;

//--------------------------------------------------------------------------------------
// Importance sample GGX normal distribution function for a fixed roughness value.
// This returns normalized half-vector between Li & Lo.
// For derivation see: http://blog.tobias-franke.eu/2014/03/30/notes_on_importance_sampling.html
//--------------------------------------------------------------------------------------
float3 sampleGGX(float u1, float u2, float roughness)
{
	float alpha = roughness * roughness;

	float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha*alpha - 1.0) * u2));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	float phi = 2.0 * PI * u1;

	// Convert to Cartesian upon return.
	return float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

//--------------------------------------------------------------------------------------
// Sample i-th point from Hammersley point set of NumSamples points total
//--------------------------------------------------------------------------------------
float2 sampleHammersley(uint i, uint _sampleCount)
{
	const float tof = 0.5 / float(0x80000000U);
    uint bits = i;
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float2(float(i) / _sampleCount, float(bits) * tof);
}

//--------------------------------------------------------------------------------------
// Single term for separable Schlick-GGX below.
//--------------------------------------------------------------------------------------
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0f - k) + k);
}

//--------------------------------------------------------------------------------------
// Schlick-GGX approximation of geometric attenuation function using Smith's method.
//--------------------------------------------------------------------------------------
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f; 
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

//--------------------------------------------------------------------------------------
// Shlick's approximation of the Fresnel factor.
//-------------------------------------------------------------------------------------- 
float3 fresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

//-------------------------------------------------------------------------------------- 
float3 fresnelSchlickRoughness(float3 F0, float cosTheta, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//--------------------------------------------------------------------------------------
// GGX/Towbridge-Reitz normal distribution function using Disney's reparametrization of alpha = roughness^2.
//--------------------------------------------------------------------------------------
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0f) + 1.0f;
	return alphaSq / (PI * denom * denom);
}

//--------------------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float a = roughness;
	float k = (a * a) / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

//--------------------------------------------------------------------------------------
float GeometrySmith(float roughness, float NoV, float NoL)
{
	float ggx2 = GeometrySchlickGGX(NoV, roughness);
	float ggx1 = GeometrySchlickGGX(NoL, roughness);

	return ggx1 * ggx2;
}

#endif