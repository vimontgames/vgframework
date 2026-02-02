#include "screenSpaceAmbient.hlsli"
#include "system/samplers.hlsli"
#include "system/bindless.hlsli"
#include "lighting/GBuffer.hlsli"
#include "system/compute.hlsli"
#include "system/view.hlsli"

#define GTAO_DIRECTIONS 5
#define GTAO_STEPS      3
#define GTAO_RADIUS     8.0f 
#define GTAO_BIAS       0.1f
#define PI              3.1417f

//--------------------------------------------------------------------------------------   
float Hash12(uint2 p)
{
    uint h = p.x * 1664525u + p.y * 1013904223u;
    return frac(h * 0.0000001192092896f);
}

float ComputeGTAO(uint2 pixel, float2 uv, float3 P, float3 N, ViewConstants vc)
{
    float ao = 0.0f;
    float rand = Hash12(pixel);
    float2 invScreenSize = 1.0f / (float2)screenSpaceAmbientConstants.getScreenSize();
    
    Texture2D normalTex = getTexture2D(screenSpaceAmbientConstants.getNormalGBuffer());
    Texture2D linearDepthTex = getTexture2D(screenSpaceAmbientConstants.getLinearDepthBuffer());

    for (int d = 0; d < GTAO_DIRECTIONS; ++d)
    {
        float angle = (2.0f * PI * (d + rand)) / GTAO_DIRECTIONS;
        float2 dir = float2(cos(angle), sin(angle));

        float maxHorizon = -PI * 0.5f;

        for (int s = 1; s <= GTAO_STEPS; ++s)
        {
            float t = (float)s / GTAO_STEPS;
            float2 offsetUV = dir * t * GTAO_RADIUS * invScreenSize;

            float sd = linearDepthTex.SampleLevel(linearClamp, uv + offsetUV, 0).x;
            float3 S = vc.getWorldPosFromLinearDepth(uv + offsetUV, sd);

            float3 V = S - P;
            float dist = length(V);
            if (dist < 1e-4f || dist > GTAO_RADIUS)
                continue;

            V /= dist;

            if (dot(N, V) < 0.0f)
                continue;

            float horizon = asin(dot(V, N));
            maxHorizon = max(maxHorizon, horizon);
        }

        ao += saturate(sin(maxHorizon) - GTAO_BIAS);
    }

    ao /= GTAO_DIRECTIONS;
    return saturate(1.0f - ao);
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
    
    float3 normal = normalize(normalTex.SampleLevel(linearClamp, uv, 0).xyz);
    float depth = linearDepthTex.SampleLevel(linearClamp, uv, 0).x;

    float3 worldPos = viewConstants.getWorldPosFromLinearDepth(uv, depth);

    float ao = ComputeGTAO(dispatchThreadID.xy, uv, worldPos, normal, viewConstants);
    
    float3 result = ao;
    getRWTexture2D(screenSpaceAmbientConstants.getRWBufferOut())[dispatchThreadID] = float4(result.rgb,1); 
}