#include "precomputeIBL.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/pbr.hlsli"

static const uint samples = 1024;

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
float2 IntegrateBRDF(float NdotV, float roughness)
{
	float3 V = float3(sqrt(1.0 - NdotV * NdotV), 0, NdotV);

	float A = 0.0;
	float B = 0.0;

	float3 N = float3(0, 0, 1);

	for (unsigned int i = 0u; i < samples; ++i)
	{
		float2 Xi = sampleHammersley(i, samples);
		float3 H = ImportanceSampleGGX(Xi, roughness, N);
		float3 L = normalize(2.0f * dot(V, H) * H - V);

		float NoL = max(L.z, 0.0f);
		float NoH = max(H.z, 0.0f);
		float VoH = max(dot(V, H), 0.0f);
		float NoV = max(dot(N, V), 0.0f);

		if (NoL > 0.0)
		{
			float G = GeometrySmith(roughness, NoV, NoL);
			//float G = gaSchlickGGX(NoV, NoL, roughness);

			float G_Vis = (G * VoH) / (NoH * NoV);
			float Fc = pow(1.0 - VoH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return float2(A, B) / (float)samples;
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateSpecularBRDF(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    const int2 size = precomputeIBLConstants.getDestinationSize();

    float cosLo = (float)(dispatchThreadID.x) / (float)(size.x-1);
	float roughness = (float)(size.y-dispatchThreadID.y-1) / (float)(size.y-1);

	// clamp cosLo to avoid NaN
	cosLo = max(cosLo, 0.001);

	getRWTexture2D_float2(precomputeIBLConstants.getDestination())[dispatchThreadID.xy] = IntegrateBRDF(cosLo, roughness);
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateIrradianceCubemap(int3 dispatchThreadID : SV_DispatchThreadID)
{   
    const int2 size = precomputeIBLConstants.getDestinationSize();

	getRWTexture2DArray(precomputeIBLConstants.getDestination())[dispatchThreadID.xyz] = float4(1,0,1,1);
}

//--------------------------------------------------------------------------------------
[numthreads(PRECOMPUTE_IBL_THREADGROUP_SIZE_X, PRECOMPUTE_IBL_THREADGROUP_SIZE_Y, PRECOMPUTE_IBL_THREADGROUP_SIZE_Z)]
void CS_GenerateSpecularReflectionCubemap(int3 dispatchThreadID : SV_DispatchThreadID)
{   
    const int2 size = precomputeIBLConstants.getDestinationSize();

	getRWTexture2DArray(precomputeIBLConstants.getDestination())[dispatchThreadID.xyz] = float4(size / 256.0, (float)dispatchThreadID.z / 6.0f, 1);
}

