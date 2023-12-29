#pragma once

//--------------------------------------------------------------------------------------
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl 
//--------------------------------------------------------------------------------------

static const float3 Fdielectric = 0.04f;	
static const float PI = 3.141592f;
static const float Epsilon = 0.00001f;

//--------------------------------------------------------------------------------------
// GGX/Towbridge-Reitz normal distribution function. Uses Disney's reparametrization of alpha = roughness^2.
//--------------------------------------------------------------------------------------
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0f) + 1.0f;
	return alphaSq / (PI * denom * denom);
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
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 4.0f);
}

//--------------------------------------------------------------------------------------
struct LightingResult
{
    float3 diffuse;
    float3 specular;
};

//--------------------------------------------------------------------------------------
LightingResult computeDirectLighting(float3 _eyePos, float3 _worldPos, float3 _albedo, float3 _worldNormal, float4 _pbr)
{
    LightingResult output = (LightingResult)0;
    
    float occlusion = 1.0f; //_pbr.r;
    float roughness = 0.3f; //_pbr.g;
    float metalness = 0.0f; //_pbr.b;

    // Outgoing light direction (vector from world-space fragment position to camera).
	float3 Lo = normalize(_eyePos - _worldPos.xyz);

    // Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(_worldNormal, Lo));

    // Specular reflection vector.
	float3 Lr = 2.0 * cosLo * _worldNormal - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
	float3 F0 = lerp(Fdielectric, _albedo, metalness);

	//for(uint i=0; i<NumLights; ++i)
	{
		float3 Li = normalize(float3(2, -1, 3));//-lights[i].direction;
		float3 Lradiance = 1.0f; //lights[i].radiance;

		// Half-vector between Li and Lo.
		float3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0f, dot(_worldNormal, Li));
		float cosLh = max(0.0f, dot(_worldNormal, Lh));

        // Calculate Fresnel term for direct lighting. 
		float3 F = fresnelSchlick(F0, max(0.0f, dot(Lh, Lo)));

		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);

		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		float3 kd = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		float3 diffuseBRDF = kd;// * _albedo;

		// Cook-Torrance specular microfacet BRDF.
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		output.diffuse += diffuseBRDF * Lradiance * cosLi;
		output.specular += specularBRDF * Lradiance * cosLi;
    }	

    return output;
}

//--------------------------------------------------------------------------------------
float3 ApplyLighting(float3 _albedo, LightingResult _lighting)
{
    return _albedo.rgb * _lighting.diffuse + _lighting.specular;
}