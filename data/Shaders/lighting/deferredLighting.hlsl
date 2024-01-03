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

        float4 albedo = getTexture2D(deferredLightingConstants.getAlbedoGBuffer()).Load(address);
        float4 normal = getTexture2D(deferredLightingConstants.getNormalGBuffer()).Load(address);
        float4 pbr = getTexture2D(deferredLightingConstants.getPBRGBuffer()).Load(address);

        float depth = getTexture2D(deferredLightingConstants.getDepth()).Load(address).r;

        if (albedo.a <= 0.0f)
            return;

        float3 worldPos = viewConstants.getWorldPos(uv, depth);
        float3 camPos = viewConstants.getCameraPos();

        float4 color = float4(frac(worldPos),1);

        LightingResult lighting = computeDirectLighting(viewConstants, camPos, worldPos, albedo.xyz, normal.xyz, pbr);
        
        color.rgb = applyLighting(albedo.rgb, lighting, viewConstants.getDisplayMode());

        #if _TOOLMODE
        switch(viewConstants.getDisplayMode())
        {
            case DisplayMode::None:
                break;
        
            case DisplayMode::Lighting_Diffuse:
            case DisplayMode::Lighting_Specular:
            case DisplayMode::Lighting_RayCount:
                break;

            default:
                color = float4(albedo.rgb, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer0_Albedo:
                color = float4(albedo.rgb, 1.0f);
                break;

            //case DisplayMode::Deferred_GBuffer0_A:
            //    color = float4(albedo.aaa, 1.0f);
            //    break;

            case DisplayMode::Deferred_GBuffer1_Normal:
                color = float4(normal.rgb*0.5f+0.5f, 1.0f);
                break;

            //case DisplayMode::Deferred_GBuffer1_A:
            //    color = float4(normal.aaa, 1.0f);
            //    break;

            case DisplayMode::Deferred_GBuffer2_Occlusion:
                color = float4(pbr.rrr, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer2_Roughness:
                color = float4(pbr.ggg, 1.0f);
                break;

            case DisplayMode::Deferred_GBuffer2_Metalness:
                color = float4(pbr.bbb, 1.0f);
                break;

            //case DisplayMode::Deferred_GBuffer1_A:
            //    color = float4(normal.aaa, 1.0f);
            //    break;
        }
        #endif

        getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords] = color;
    }
}