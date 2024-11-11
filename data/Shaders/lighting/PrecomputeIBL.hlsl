#include "precomputeIBL.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/pbr.hlsli"
#include "system/cubemap.hlsli"
#include "system/pbr.hlsli"

//--------------------------------------------------------------------------------------
float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
	float a = roughness*roughness;

	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

	float3 H = float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

	float3 up = float3(1.0, 0.0, 0.0);
	float3 tangent = normalize(cross(up, N));
	float3 bitangent = cross(N, tangent);

	return normalize(tangent * H.x + bitangent * H.y + N * H.z);
}

//--------------------------------------------------------------------------------------
float2 IntegrateBRDF(float _NdotV, float _roughness, uint _samples)
{
	float3 V = float3(sqrt(1.0 - _NdotV * _NdotV), 0, _NdotV);

	float A = 0.0;
	float B = 0.0;

	float3 N = float3(0, 0, 1);

	for (unsigned int i = 0u; i < _samples; ++i)
	{
		float2 Xi = sampleHammersley(i, _samples);
		float3 H = ImportanceSampleGGX(Xi, _roughness, N);
		float3 L = normalize(2.0f * dot(V, H) * H - V);

		float NoL = max(L.z, 0.0f);
		float NoH = max(H.z, 0.0f);
		float VoH = max(dot(V, H), 0.0f);
		float NoV = max(dot(N, V), 0.0f);

		if (NoL > 0.0)
		{
			float G = GeometrySmith(_roughness, NoV, NoL);
			//float G = gaSchlickGGX(NoV, NoL, _roughness);

			float G_Vis = (G * VoH) / (NoH * NoV);
			float Fc = pow(1.0 - VoH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return float2(A, B) / (float)_samples;
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateSpecularBRDF(int2 dispatchThreadID : SV_DispatchThreadID)
{   
	const uint samples = 1024;
	RWTexture2D<float2> dest = getRWTexture2D_float2(precomputeIBLConstants.getDestination());
	uint width, height;
	dest.GetDimensions(width, height);
    const uint2 size = uint2(width, height);

    float cosLo = (float)(dispatchThreadID.x) / (float)(size.x-1);
	float roughness = (float)(size.y-dispatchThreadID.y-1) / (float)(size.y-1);

	// clamp cosLo to avoid NaN
	cosLo = max(cosLo, 0.001);

	dest[dispatchThreadID.xy] = IntegrateBRDF(cosLo, roughness, samples);
}

//--------------------------------------------------------------------------------------
float3 getUVW(float2 _uv, CubemapFace _face)
{
    float2 uv = 2.0 * float2(_uv.x, _uv.y) - 1.0;

	float3 uvw;
	switch(_face)
	{
		case CubemapFace::PositiveX: 
			uvw = float3(1,  uv.y, -uv.x); 
			break;

		case CubemapFace::NegativeX: 
			uvw =  float3(-1.0, uv.y,  uv.x); 
			break;
		case CubemapFace::PositiveY:
			uvw =  float3(uv.x, 1.0, -uv.y);
			break;
		case CubemapFace::NegativeY:
			uvw =  float3(uv.x, -1.0, uv.y); 
			break;
		case CubemapFace::PositiveZ:
			uvw =  float3(uv.x, uv.y, 1.0);
			break;
		case CubemapFace::NegativeZ:
			uvw =  float3(-uv.x, uv.y, -1.0);
			break;
	}
    return normalize(uvw);
}

//--------------------------------------------------------------------------------------
float3 sampleHemisphere(float u1, float u2)
{
	const float u1p = sqrt(max(0.0, 1.0 - u1*u1));
	return float3(cos(2.0*PI*u2) * u1p, sin(2.0*PI*u2) * u1p, u1);
}

//--------------------------------------------------------------------------------------
void computeBasisVectors(const float3 N, out float3 S, out float3 T)
{
	T = cross(N, float3(0.0, 1.0, 0.0));
	T = lerp(cross(N, float3(1.0, 0.0, 0.0)), T, step(Epsilon, dot(T, T)));

	T = normalize(T);
	S = normalize(cross(N, T));
}

//--------------------------------------------------------------------------------------
float3 tangentToWorld(const float3 v, const float3 N, const float3 S, const float3 T)
{
	return S * v.x + T * v.y + N * v.z;
}

//--------------------------------------------------------------------------------------
float3 SampleDirectionHemisphere(float u1, float u2)
{
    float z = u1;
    float r = sqrt(max(0.0f, 1.0f - z * z));
    float phi = 2 * PI * u2;
    float x = r * cos(phi);
    float y = r * sin(phi);

    return float3(x, y, z);
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateIrradianceCubemap(int3 dispatchThreadID : SV_DispatchThreadID)
{   
	const uint samples = 16384;

	RWTexture2DArray<float4> cubemapDest = getRWTexture2DArray(precomputeIBLConstants.getDestination());
	uint width, height, elements;
	cubemapDest.GetDimensions(width, height, elements);
    const uint2 size = uint2(width, height);

	float2 uv = float2((float)(dispatchThreadID.x+0.5) / (float)(size.x), (float)(dispatchThreadID.y+0.5) / (float)(size.y));
	uv.y = 1 - uv.y;
	const CubemapFace face = (CubemapFace)dispatchThreadID.z;
	const float3 N = getUVW(uv, face);
	float3 S, T;
	computeBasisVectors(N, S, T);

	TextureCube cubemapSource = getTextureCube(precomputeIBLConstants.getSource());
	uint srcMip = precomputeIBLConstants.getSourceMipLevel();

	float3 irradiance = 0; 
	for(uint i = 0; i < samples; ++i) 
	{
		float2 u  = sampleHammersley(i, samples);
		float3 Li = tangentToWorld(sampleHemisphere(u.x, u.y), N, S, T);
		float cosTheta = max(0.0, dot(Li, N));

		irradiance += 2.0 * min(cubemapSource.SampleLevel(linearClamp, Li, srcMip+1).rgb, HDRMax) * cosTheta;	
	}
	irradiance /= float(samples);

	if (0)
	{
		uint width, height, mipLevels;
		cubemapSource.GetDimensions(0, width, height, mipLevels);
		irradiance = cubemapSource.SampleLevel(linearClamp, N, srcMip).rgb;
	}

	cubemapDest[dispatchThreadID.xyz] = float4(irradiance,1);
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateSpecularReflectionCubemap(int3 dispatchThreadID : SV_DispatchThreadID)
{   
	RWTexture2DArray<float4> cubemapDest = getRWTexture2DArray(precomputeIBLConstants.getDestination());
	
	uint samples, srcMipLevel;
	const uint dstMipLevel = precomputeIBLConstants.getDestinationMipLevel();
	switch(dstMipLevel)
	{
		case 0:
			samples = 1;
			srcMipLevel = 0;
			break;

		case 1:
			samples = 65536;
			srcMipLevel = dstMipLevel;
			break;

		case 2:
			samples = 65536;
			srcMipLevel = dstMipLevel;
			break;

		case 3:
			samples = 16384;
			srcMipLevel = dstMipLevel+1;
			break;

		default:
			samples = 16384;
			srcMipLevel = dstMipLevel;
			break;		
	}
	
	uint dstWidth, dstHeight, dstElements;
	cubemapDest.GetDimensions(dstWidth, dstHeight, dstElements);
    const uint2 size = uint2(dstWidth, dstHeight);

	float2 uv = float2((float)(dispatchThreadID.x+0.5) / (float)(size.x), (float)(dispatchThreadID.y+0.5) / (float)(size.y));
	uv.y = 1 - uv.y;
	const CubemapFace face = (CubemapFace)dispatchThreadID.z;
	const float3 N = getUVW(uv, face);
	float3 S, T;
	computeBasisVectors(N, S, T);

	TextureCube cubemapSource = getTextureCube(precomputeIBLConstants.getSource());
	uint srcWidth, srcHeight, srcMipLevels;
	cubemapSource.GetDimensions(0, srcWidth, srcHeight, srcMipLevels);
	float wt = 4.0 * PI / (6.0 * (float)srcWidth * (float)srcHeight);

	const float roughness = (float)dstMipLevel / (precomputeIBLConstants.getDestinationMipLevelCount()-1);

	float3 color = 0;
	float weight = 0;

	for(uint i = 0; i < samples; ++i)
	{
		float2 u = sampleHammersley(i, samples);
		float3 Lh = tangentToWorld(sampleGGX(u.x, u.y, roughness), N, S, T);

		float3 Li = 2.0 * dot(N, Lh) * Lh - N;

		float cosLi = dot(N, Li);
		if(cosLi > 0.0) 
		{
			float cosLh = max(dot(N, Lh), 0.0);
			float pdf = ndfGGX(cosLh, roughness) * 0.25;
			float ws = 1.0 / (samples * pdf);
			float mipLevel = max(0.5 * log2(ws / wt) + 1.0, 0.0);

			color  += min(cubemapSource.SampleLevel(linearClamp, Li, srcMipLevel).rgb * cosLi, HDRMax);
			weight += cosLi;
		}
	}
	color /= weight;

	cubemapDest[dispatchThreadID.xyz] = float4(color, 1);
}

