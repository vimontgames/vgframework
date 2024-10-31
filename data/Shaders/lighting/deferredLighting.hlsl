#include "deferredLighting.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/lighting.hlsli"
#include "system/msaa.hlsli"

struct DepthStencilSample
{
    float depth;

    void Load(int2 _coords, uint _sampleIndex)
    {
        #if SAMPLE_COUNT > 1
        depth = getTexture2DMS(deferredLightingConstants.getDepth()).Load(_coords, _sampleIndex).r;
        #else
        depth = getTexture2D(deferredLightingConstants.getDepth()).Load(int3(_coords, 0)).r;
        #endif

        #if VG_GFX_REVERSE_DEPTH
        depth = 1.0f - depth;
        #endif
    }
};

struct GBufferSample
{
    float4 albedo;
    float4 normal;
    float4 pbr; 

    void Load(int2 _coords, uint _sampleIndex)
    {
        #if SAMPLE_COUNT > 1
        albedo = getTexture2DMS(deferredLightingConstants.getAlbedoGBuffer()).Load(_coords, _sampleIndex);
        normal = getTexture2DMS(deferredLightingConstants.getNormalGBuffer()).Load(_coords, _sampleIndex);
        pbr = getTexture2DMS(deferredLightingConstants.getPBRGBuffer()).Load(_coords, _sampleIndex);   
        #else
        albedo = getTexture2D(deferredLightingConstants.getAlbedoGBuffer()).Load(int3(_coords,0));
        normal = getTexture2D(deferredLightingConstants.getNormalGBuffer()).Load(int3(_coords,0));
        pbr = getTexture2D(deferredLightingConstants.getPBRGBuffer()).Load(int3(_coords,0));        
        #endif
    }
};

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

        // Load depth samples
        DepthStencilSample depthStencilSamples[SAMPLE_COUNT];
        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            depthStencilSamples[i].Load(coords, i);

        // TODO: early out if all depth samples >= 1.0f
        //if (depth >= 1.0f)
        //    return;

        GBufferSample gbufferSamples[SAMPLE_COUNT];
        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            gbufferSamples[i].Load(coords, i);

        // Deferred shading result
        float3 color[SAMPLE_COUNT];

        for (uint i = 0; i < SAMPLE_COUNT; ++i)
        {
            float3 worldPos = viewConstants.getWorldPos(uv, depthStencilSamples[i].depth);
            float3 camPos = viewConstants.getCameraPos();
                        
            LightingResult lighting = computeDirectLighting(viewConstants, camPos, worldPos, gbufferSamples[i].albedo.xyz, gbufferSamples[i].normal.xyz, gbufferSamples[i].pbr);
            
            color[i].rgb = applyLighting(gbufferSamples[i].albedo.rgb, lighting, viewConstants.getDisplayMode());

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
                    break;
            
                case DisplayMode::Deferred_GBuffer0_Albedo:
                    color[i].rgb = gbufferSamples[i].albedo.rgb;
                    break;
            
                //case DisplayMode::Deferred_GBuffer0_A:
                //    color[i].rgb = gbufferSamples[i].albedo.aaa;
                //    break;
            
                case DisplayMode::Deferred_GBuffer1_Normal:
                    color[i].rgb = gbufferSamples[i].normal.rgb*0.5f+0.5f;
                    break;
            
                //case DisplayMode::Deferred_GBuffer1_A:
                //    color[i].rgb = gbufferSamples[i].normal.aaa;
                //    break;
            
                case DisplayMode::Deferred_GBuffer2_Occlusion:
                    color[i].rgb = gbufferSamples[i].pbr.rrr;
                    break;
            
                case DisplayMode::Deferred_GBuffer2_Roughness:
                    color[i].rgb = gbufferSamples[i].pbr.ggg;
                    break;
            
                case DisplayMode::Deferred_GBuffer2_Metalness:
                    color[i].rgb = gbufferSamples[i].pbr.bbb;
                    break;
            
                //case DisplayMode::Deferred_GBuffer1_A:
                //    color[i].rgb = gbufferSamples[i].normal.aaa;
                //    break;
            }
            #endif
        }

        // Store
        #if SAMPLE_COUNT > 1

        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords * getMSAASampleScale(SAMPLE_COUNT) + getMSAASampleOffset(SAMPLE_COUNT, i)] = float4(color[i], 1);

        #else

        getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords] = float4(color[0],1);

        #endif
    }
}