#include "postprocess.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/depthstencil.hlsli"
#include "system/msaa.hlsli"
#include "system/instancedata.hlsli"

#if _FXAA
#include "FXAA.hlsli"
#elif _SMAA
#include "SMAA.hlsli"
#endif

[numthreads(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y, 1)]
void CS_ResolveMSAA(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    uint2 screenSize = postProcessConstants.getScreenSize();    

    int2 coords = dispatchThreadID;
    float2 uv = ((float2)dispatchThreadID.xy + 0.5) / (float2)screenSize.xy;

    if (all(dispatchThreadID.xy < screenSize))
    {
        float4 color = (float4)0.0;

        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            color += getTexture2DMS(postProcessConstants.getColor()).Load(coords, i);
        color /= (float)SAMPLE_COUNT;           

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = color;
    }
}

#if _TOOLMODE
#if _RAYTRACING
float4 DebugRayTracing(float4 color, float2 uv, uint2 screenSize, ViewConstants viewConstants)
{
    float4x4 viewInv = viewConstants.getViewInv();

    float nearDist = viewConstants.getCameraNearFar().x;
    float farDist = viewConstants.getCameraNearFar().y;
    float fovRadians = viewConstants.getCameraFieldOfView();
    float viewRatio = viewConstants.getCameraAspectRatio();
    float3 camRight = viewConstants.getCameraRight();
    float3 camUp = viewConstants.getCameraUp();
    float3 camForward = viewConstants.getCameraForward();
    float3 camPos = viewConstants.getCameraPos();

    float3 nearCenter = camPos + camForward * nearDist;
    float3 farCenter = camPos + camForward * farDist;

    float nearHeight = 2.0f * tan(fovRadians/ 2.0f) * nearDist;
    float farHeight = 2.0f * tan(fovRadians / 2.0f) * farDist;
    float nearWidth = nearHeight * viewRatio;
    float farWidth  = farHeight * viewRatio;

    float3 farTopLeft = farCenter - camUp * (farHeight*0.5f) + camRight * (farWidth*0.5f);
    float3 farTopRight = farCenter - camUp * (farHeight*0.5f) - camRight * (farWidth*0.5f);
    float3 farBottomLeft = farCenter + camUp * (farHeight*0.5f) + camRight * (farWidth*0.5f);
    float3 farBottomRight = farCenter + camUp * (farHeight*0.5f) - camRight * (farWidth*0.5f);

    float3 nearTopLeft = nearCenter - camUp * (nearHeight*0.5f) + camRight * (nearWidth*0.5f);
    float3 nearTopRight = nearCenter - camUp * (nearHeight*0.5f) - camRight * (nearWidth*0.5f);
    float3 nearBottomLeft = nearCenter + camUp * (nearHeight*0.5f) + camRight * (nearWidth*0.5f);
    float3 nearBottomRight = nearCenter + camUp * (nearHeight*0.5f) - camRight * (nearWidth*0.5f);

    float3 origin = camPos;

    float w00 = (1.0f - uv.x) * (1.0f - uv.y);
    float w01 = uv.x * (1.0f - uv.y);
    float w10 = (1.0f - uv.x) * uv.y;
    float w11 = uv.x * uv.y;
        
    float3 nearDir = nearTopLeft * w00 + nearTopRight * w01 + nearBottomLeft * w10 + nearBottomRight * w11;
    float3 farDir = farTopLeft * w00 + farTopRight * w01 + farBottomLeft * w10 + farBottomRight * w11;
    float3 dir = normalize(farDir - nearDir);

    #define tlas getTLAS(viewConstants.getTLASHandle())
        
    RayQuery<RAY_FLAG_FORCE_OPAQUE> query;
    RayDesc ray;
    ray.Origin    = origin;
    ray.Direction = dir;
    ray.TMin      = nearDist;
    ray.TMax      = farDist;
    query.TraceRayInline(tlas, 0, 0xff, ray);
    query.Proceed();

    DisplayMode mode = viewConstants.getDisplayMode();

    float3 colors[6] = 
    {
        float3(1,0,0),
        float3(0,1,0),
        float3(1,1,0),
        float3(0,0,1),
        float3(1,0,1),
        float3(0,1,1)
    };
        
    switch(query.CommittedStatus())
    {
        case COMMITTED_NOTHING:
        {
            switch(mode)
            {
                default:
                break;

                case DisplayMode::RayTracing_Hit:
                color.rgb = lerp(color.rgb, float3(1,0,0), 0.5f);
                break;
            }
            
        }       
        break;

        case COMMITTED_TRIANGLE_HIT:
        {
            switch(mode)
            {
                default:
                break;

                case DisplayMode::RayTracing_Hit:
                color.rgb = lerp(color.rgb, float3(0,1,0), 0.5f);
                break;

                case DisplayMode::RayTracing_Barycentrics:
                color.rgb = float3(query.CommittedTriangleBarycentrics(), 0);
                break;

                case DisplayMode::RayTracing_InstanceID:
                {
                    uint instanceID = query.CommittedInstanceID();
                    color.rgb = float3(colors[instanceID % 6]);
                }
                break;

                case DisplayMode::RayTracing_GeometryIndex:
                {
                    uint geoIndex = query.CommittedGeometryIndex();
                    color.rgb = float3(frac(geoIndex.xxx / 16.0f));
                }
                break;

                case DisplayMode::RayTracing_PrimitiveIndex:
                {
                    uint primitiveIndex = query.CommittedPrimitiveIndex();
                    color.rgb = float3(frac(primitiveIndex.xxx / 256.0f));
                }
                break;
                
                case DisplayMode::RayTracing_Material:
                {
                    uint instanceDataOffset = query.CommittedInstanceID();
                    GPUInstanceData instanceDataHeader = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);
                    float4 instanceColor = instanceDataHeader.getInstanceColor();

                    GPUMaterialData materialData = instanceDataHeader.getGPUMaterialData(instanceDataOffset, query.CommittedGeometryIndex());
                    float4 materialColor = materialData.getAlbedoColor();                

                    color.rgb = instanceColor.rgb * materialColor.rgb;
                }
                break;
            }
        }       
        break;

        case COMMITTED_PROCEDURAL_PRIMITIVE_HIT:
        {
            switch(mode)
            {
                default:
                break;

                case DisplayMode::RayTracing_Hit:
                color.rgb = lerp(color.rgb, float3(0,0,1), 0.5f);
                break;
            }
        }       
        break;
    }

    return color;
}
#endif // _RAYTRACING
#endif // _TOOLMODE

#if SAMPLE_COUNT > 1
groupshared float4 localData[POSTPROCESS_THREADGROUP_SIZE_X*POSTPROCESS_THREADGROUP_SIZE_Y];
#endif

[numthreads(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y, 1)]
void CS_PostProcessMain(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    uint2 screenSize = postProcessConstants.getScreenSize();    

    int2 coords = dispatchThreadID;
    float2 uv = ((float2)dispatchThreadID.xy + 0.5) / (float2)screenSize.xy;

    if (all(dispatchThreadID.xy < screenSize))
    {
        ViewConstants viewConstants;
        viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

        int3 address = int3(dispatchThreadID.xy, 0);

        float4 color;

        // Select Anti-Aliasing mode
        #if _FXAA
        color = FXAA(getTexture2D(postProcessConstants.getColor()), uv, viewConstants.getDisplayMode());
        #elif _SMAA
        color = SMAA(getTexture2D(postProcessConstants.getColor()), address);
        #else
        color = getTexture2D(postProcessConstants.getColor()).Load(address);
        #endif

        float depth = loadDepth(postProcessConstants.getDepth(), address); 
        uint stencil = loadStencil(postProcessConstants.getStencil(), address);
        
        #if _TOOLMODE

        #if _RAYTRACING
        color = DebugRayTracing(color, uv, screenSize, viewConstants);
        #endif

        switch(viewConstants.getDisplayMode())
        {
            default:
            break;
        
            case DisplayMode::PostProcess_Depth:
            color.rgb = frac(depth * viewConstants.getCameraNearFar().y);
            break;

            case DisplayMode::PostProcess_LinearDepth:
            {
                float linearDepth = viewConstants.getLinearDepth(depth);
                color.rgb = depth < 1.0f ? frac(linearDepth * viewConstants.getCameraNearFar().y) : 0.0f;
            }
            break;

            case DisplayMode::PostProcess_WorldPos:
            {
                float3 worldPos = viewConstants.getWorldPos(uv, depth);
                color.rgb = depth < 1.0f ? frac(worldPos.xyz) : 0.0f;
            }
            break;

            case DisplayMode::PostProcess_Stencil:
            color.rgb = stencil / 255.0f;
            break;

            case DisplayMode::Lighting_EnvironmentSpecularBRDF:
            color.rgb = getTexture2D(viewConstants.getSpecularBRDF()).SampleLevel(nearestClamp, uv, 0).rgb;
            break;
        }

        #endif // _TOOLMODE

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = color;
    }
}