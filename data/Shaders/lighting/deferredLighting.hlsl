#include "deferredLighting.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/lighting.hlsli"
#include "system/msaa.hlsli"
#include "system/environment.hlsli"
#include "system/depthstencil.hlsli"
#include "lighting/GBuffer.hlsli"

struct DepthStencilSample
{
    float depth;

    void Load(int2 _coords, uint _sampleIndex)
    {
        #if SAMPLE_COUNT > 1
        depth = loadDepthMSAA(deferredLightingConstants.getDepth(), _coords, _sampleIndex);
        #else
        depth = loadDepth(deferredLightingConstants.getDepth(), _coords);
        #endif

        #if VG_GFX_REVERSE_DEPTH
        depth = 1.0f - depth;
        #endif
    }
};

GBufferSample LoadGBufferSample(int2 _coords, uint _sampleIndex)
{
    GBufferSample sample;
    sample.Load(_coords, deferredLightingConstants.getAlbedoGBuffer(), deferredLightingConstants.getNormalGBuffer(), deferredLightingConstants.getPBRGBuffer(), deferredLightingConstants.getEmissiveGBuffer(), _sampleIndex);
    return sample;
}

float3 shadeSample(GBufferSample _gbuffer, DepthStencilSample _depthStencil, float2 _uv, ViewConstants _viewConstants)
{
    #if SAMPLE_COUNT > 1
    if (_depthStencil.depth >= 1.0f)
        return getEnvironmentBackgroundColor(_uv, _viewConstants);  
    #endif  

    float3 worldPos = _viewConstants.getWorldPos(_uv, _depthStencil.depth);
    float3 camPos = _viewConstants.getCameraPos();
                        
    LightingResult lighting = computeLighting(_viewConstants, camPos, worldPos, _gbuffer.albedo.xyz, _gbuffer.normal.xyz, _gbuffer.pbr, _gbuffer.emissive.rgb);
            
    float3 color = applyLighting(_gbuffer.albedo.rgb, lighting, _viewConstants.getDisplayMode());

    #if _TOOLMODE
    switch(_viewConstants.getDisplayMode())
    {
        case DisplayMode::None:
            break;
            
        // Handled in applyLighting
        case DisplayMode::Lighting_EnvironmentDiffuse:
        case DisplayMode::Lighting_EnvironmentSpecular:
        case DisplayMode::Lighting_DirectDiffuse:
        case DisplayMode::Lighting_DirectSpecular:
        case DisplayMode::Lighting_Diffuse:
        case DisplayMode::Lighting_Specular:
        case DisplayMode::Lighting_Emissive:
        case DisplayMode::Lighting_RayCount:
        case DisplayMode::Environment_Cubemap:
        case DisplayMode::Environment_IrradianceCubemap:
        case DisplayMode::Environment_SpecularReflectionCubemap: 
            break;

        default:
        case DisplayMode::Instance_Color:
        case DisplayMode::Forward_SurfaceType:
        case DisplayMode::Geometry_MaterialID:
	    case DisplayMode::Geometry_VertexNormal:
        case DisplayMode::Geometry_VertexTangent:
        case DisplayMode::Geometry_VertexBinormal:
        case DisplayMode::Geometry_VertexColor:
	    case DisplayMode::Geometry_UV0:
        case DisplayMode::Geometry_UV1:
        case DisplayMode::Material_Albedo:
        case DisplayMode::Material_AmbientOcclusion:
        case DisplayMode::Material_Roughness:
        case DisplayMode::Material_Metalness:
        case DisplayMode::Material_NormalMap:
        case DisplayMode::Forward_WorldNormal:
        case DisplayMode::Forward_WorldPosition:
        case DisplayMode::Forward_ScreenPos:
            color = _gbuffer.albedo.rgb;
            break;
                        
        case DisplayMode::Deferred_Albedo:
            color = _gbuffer.albedo.rgb;
            break;
                        
        case DisplayMode::Deferred_Normal:
            color = _gbuffer.normal.rgb*0.5f+0.5f;
            break;
                        
        case DisplayMode::Deferred_PBR:
            color = _gbuffer.pbr.rgb;
            break;
                            
        case DisplayMode::Deferred_Emissive:
            color = _gbuffer.emissive.rgb;
            break;
                        
        case DisplayMode::Deferred_MSAAEdges:
            // Handled in CS_DeferredLighting(int2 dispatchThreadID : SV_DispatchThreadID)
            break;

        case DisplayMode::PostProcess_FXAAEdges:
            // Handled in float3 FXAA(Texture2D src, float2 uv, DisplayMode _displayMode) in FXAA.hlsli
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
            gbufferSamples[i] = LoadGBufferSample(coords, i);

        // Deferred shading result
        float3 color[SAMPLE_COUNT];

        #if SAMPLE_COUNT == 1
        if (depthStencilSamples[0].depth >= 1.0f)
            return;  
        #endif  

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