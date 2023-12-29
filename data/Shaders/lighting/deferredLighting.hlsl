#include "deferredLighting.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/lighting.hlsli"

[numthreads(DEFERRED_LIGHTING_THREADGROUP_SIZE_X, DEFERRED_LIGHTING_THREADGROUP_SIZE_Y, 1)]
void CS_DeferredLighting(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    uint2 screenSize = deferredLightingConstants.getScreenSize();    

    int2 coords = dispatchThreadID;
    float2 uv = dispatchThreadID.xy / (float2)screenSize;

    if (all(dispatchThreadID.xy < screenSize))
    {
        ViewConstants viewConstants;
        viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

        int3 address = int3(dispatchThreadID.xy, 0);

        float4 albedo = getTexture2D(deferredLightingConstants.getAlbedo()).Load(address);
        float4 normal = getTexture2D(deferredLightingConstants.getNormal()).Load(address);
        float depth = getTexture2D(deferredLightingConstants.getDepth()).Load(address).r;

        if (albedo.a <= 0.0f)
            return;

        float3 worldPos = viewConstants.getWorldPos(uv, depth);

        float4 color = float4(frac(worldPos),1);

        LightingResult lighting = ComputeLighting(normal.xyz);
        color.rgb = ApplyLighting(albedo.rgb, lighting);

        #if _TOOLMODE
        switch(viewConstants.getDisplayMode())
        {
            case DisplayMode::None:
                break;

            default:
                color = float4(albedo.rgb, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer0_RGB:
                color = float4(albedo.rgb, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer0_A:
                color = float4(albedo.aaa, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer1_RGB:
                color = float4(normal.rgb*0.5f+0.5f, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer1_A:
                color = float4(normal.aaa, 1.0f);
                break;
        }
        #endif

        getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords] = color;
    }
}