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

float3 shadeSample(GBufferSample _gbuffer, DepthStencilSample _depthStencil, float2 _uv, ViewConstants _viewConstants)
{
    if (_depthStencil.depth >= 1.0f)
    {
        float3 envColor = _viewConstants.getEnvironmentColor();
        return envColor;       
    }

    float3 worldPos = _viewConstants.getWorldPos(_uv, _depthStencil.depth);
    float3 camPos = _viewConstants.getCameraPos();
                        
    LightingResult lighting = computeDirectLighting(_viewConstants, camPos, worldPos, _gbuffer.albedo.xyz, _gbuffer.normal.xyz, _gbuffer.pbr);
            
    float3 color = applyLighting(_gbuffer.albedo.rgb, lighting, _viewConstants.getDisplayMode());

    #if _TOOLMODE
    switch(_viewConstants.getDisplayMode())
    {
        case DisplayMode::None:
            break;
            
        // Handled in applyLighting
        case DisplayMode::Lighting_EnvironmentDiffuse:
        case DisplayMode::Lighting_EnvironmentSpecular:
        case DisplayMode::Lighting_DirectLightDiffuse:
        case DisplayMode::Lighting_DirectLightSpecular:
        case DisplayMode::Lighting_Diffuse:
        case DisplayMode::Lighting_Specular:
        case DisplayMode::Lighting_RayCount:
            break;
            
        default:
            break;
            
        case DisplayMode::Deferred_GBuffer0_Albedo:
            color = _gbuffer.albedo.rgb;
            break;
            
        //case DisplayMode::Deferred_GBuffer0_A:
        //    color = _gbuffer.albedo.aaa;
        //    break;
            
        case DisplayMode::Deferred_GBuffer1_Normal:
            color = _gbuffer.normal.rgb*0.5f+0.5f;
            break;
            
        //case DisplayMode::Deferred_GBuffer1_A:
        //    color = _gbuffer.normal.aaa;
        //    break;
            
        case DisplayMode::Deferred_GBuffer2_Occlusion:
            color = _gbuffer.pbr.rrr;
            break;
            
        case DisplayMode::Deferred_GBuffer2_Roughness:
            color = _gbuffer.pbr.ggg;
            break;
            
        case DisplayMode::Deferred_GBuffer2_Metalness:
            color = _gbuffer.pbr.bbb;
            break;
            
        //case DisplayMode::Deferred_GBuffer1_A:
        //    color = _gbuffer.normal.aaa;
        //    break;

        case DisplayMode::Deferred_MSAAEdges:
            // Handled in CS_DeferredLighting(int2 dispatchThreadID : SV_DispatchThreadID)
            break;

        case DisplayMode::PostProcess_FXAAEdges:
            // Handled in float3 FXAA(Texture2D src, float2 uv, DisplayMode _displayMode) in FXAA.hlsli
            break;

        case DisplayMode::Environment_Cubemap:
            color = getTextureCube(_viewConstants.getEnvironmentTextureHandle()).SampleLevel(linearClamp, normalize(_gbuffer.normal.rgb), 0).rgb;
            break;
    }
    #endif

    return color;
}

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

        // TODO: early out if all depth samples >= 1.0f or keep early-out per-sample?
        //if (depth >= 1.0f)
        //    return;

        GBufferSample gbufferSamples[SAMPLE_COUNT];
        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            gbufferSamples[i].Load(coords, i);

        // Deferred shading result
        float3 color[SAMPLE_COUNT];

        // Shade sample 0
        color[0] = shadeSample(gbufferSamples[0], depthStencilSamples[0], uv, viewConstants);

        #if SAMPLE_COUNT > 1

        bool isMSAAEdge = false;
        for (uint i = 1; i < SAMPLE_COUNT; ++i)
        {        
            if (any(gbufferSamples[i].normal.xyz != gbufferSamples[0].normal.xyz))
            {
                isMSAAEdge = true;
                break;
            }
        }

        [branch]
        if (isMSAAEdge)
        {
            // shade other MSAA samples 
            for (uint i = 1; i < SAMPLE_COUNT; ++i)
                 color[i] = shadeSample(gbufferSamples[i], depthStencilSamples[i], uv, viewConstants);

            #if _TOOLMODE
            if (DisplayMode::Deferred_MSAAEdges == viewConstants.getDisplayMode())
            {
                for (uint i = 1; i < SAMPLE_COUNT; ++i)
                    color[i] = float3(1,0,0);
            }
            #endif
        }
        else
        {
            [unroll]
            for (uint i = 1; i < SAMPLE_COUNT; ++i)
                color[i] = color[0];
        }

        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords * getMSAASampleScale(SAMPLE_COUNT) + getMSAASampleOffset(SAMPLE_COUNT, i)] = float4(color[i], 1);

        #else

        // When no MSAA, there's always only one sample to store
        getRWTexture2D(deferredLightingConstants.getRWBufferOut())[coords] = float4(color[0],1);

        #endif
    }
}