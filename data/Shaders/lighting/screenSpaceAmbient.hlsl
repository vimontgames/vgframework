#include "screenSpaceAmbient.hlsli"
#include "system/samplers.hlsli"
#include "system/bindless.hlsli"
#include "lighting/GBuffer.hlsli"
#include "system/compute.hlsli"
#include "system/view.hlsli"

#define GTAO_DIRECTIONS 8
#define GTAO_STEPS      5
#define GTAO_RADIUS     16.0f
#define GTAO_BIAS       0.04f
#define GTAO_POWER      2.0f
#define PI              3.14159265f

//--------------------------------------------------------------------------------------
// TODO: use blue noise instead
//--------------------------------------------------------------------------------------
float Hash12(uint2 p)
{
    uint h = p.x * 1664525u + p.y * 1013904223u;
    return frac(h * 0.0000001192092896f);
}

//--------------------------------------------------------------------------------------
float ComputeGTAO(uint2 pixel, float2 uv, float3 P, float3 N,
                   ViewConstants vc, Texture2D linearDepthTex)
{
    float ao = 0.0f;
    float rand = Hash12(pixel); // jitter 
    float2 invScreenSize = 1.0f / (float2)screenSpaceAmbientConstants.getScreenSize();

    for (int d = 0; d < GTAO_DIRECTIONS; ++d)
    {
        float angle = (2.0f * PI * (d + rand)) / GTAO_DIRECTIONS;
        float2 dir = float2(cos(angle), sin(angle));

        for (int s = 1; s <= GTAO_STEPS; ++s)
        {
            float t = (float)s / GTAO_STEPS;
            float2 offsetUV = dir * t * GTAO_RADIUS * invScreenSize;

            float sampleDepth = linearDepthTex.SampleLevel(linearClamp, uv + offsetUV, 0).x;
            float3 samplePos = vc.getWorldPosFromLinearDepth(uv + offsetUV, sampleDepth);

            float3 V = samplePos - P;
            float dist = length(V);
            if (dist < 1e-4f || dist > GTAO_RADIUS)
                continue;

            V /= dist;
            
            float NdotV = dot(N, V);

            // Keep only front-facing samples
            if (NdotV <= 0.0f)
                continue;

            // Distance attenuation
            float weight = (1.0f - dist / GTAO_RADIUS) * NdotV;
            ao += weight;
        }
    }
    
    ao /= (float)(GTAO_DIRECTIONS * GTAO_STEPS);
    ao = saturate(1.0f - ao + GTAO_BIAS);
    
    return pow(ao, GTAO_POWER);
}

//--------------------------------------------------------------------------------------
[numthreads(SCREEN_SPACE_AMBIENT_THREADGROUP_SIZE_X, SCREEN_SPACE_AMBIENT_THREADGROUP_SIZE_X, 1)]
void CS_ScreenSpaceAmbient(int2 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 screenSize = screenSpaceAmbientConstants.getScreenSize();
    float2 uv = GetScreenUVFromCoords(dispatchThreadID.xy, screenSize, false);

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    Texture2D normalTex = getTexture2D(screenSpaceAmbientConstants.getNormalGBuffer());
    Texture2D linearDepthTex = getTexture2D(screenSpaceAmbientConstants.getLinearDepthBuffer());

    float3 worldNormal = normalize(normalTex.SampleLevel(linearClamp, uv, 0).xyz);
    float linearDepth = linearDepthTex.SampleLevel(linearClamp, uv, 0).x;
    float3 worldPos = viewConstants.getWorldPosFromLinearDepth(uv, linearDepth);

    float ao = ComputeGTAO(dispatchThreadID.xy, uv, worldPos, worldNormal, viewConstants, linearDepthTex);

    getRWTexture2D(screenSpaceAmbientConstants.getRWBufferOut())[dispatchThreadID] = float4(ao, ao, ao, 1.0f);
}
