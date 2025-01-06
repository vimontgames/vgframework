#include "postprocess.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/depthstencil.hlsli"
#include "system/msaa.hlsli"
#include "system/instancedata.hlsli"
#include "system/gamma.hlsli"
#include "system/outlinemask.hlsli"

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
        Texture2DMS<float4> sourceTex = getTexture2DMS(postProcessConstants.getSource());
        [unroll]
        for (uint i = 0; i < SAMPLE_COUNT; ++i)
            color += sourceTex.Load(coords, i);
        color /= (float)SAMPLE_COUNT;           

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = color;
    }
}

float ComputeCOC(float linearDepth, ViewConstants viewConstants) 
{
    float focallength = viewConstants.getFocalLength(); 
    float aperture = viewConstants.getAperture(); 
    float objectdistance = linearDepth; 
    float planeinfocus = viewConstants.getFocusDistance(); 
    float coc = (aperture * (focallength * (objectdistance - planeinfocus)) / (objectdistance * (planeinfocus - focallength)));
    return coc * viewConstants.getDOFScale();
}

[numthreads(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y, 1)]
void CS_DepthOfField(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    uint2 screenSize = postProcessConstants.getScreenSize();    

    int2 coords = dispatchThreadID;
    float2 uv = ((float2)dispatchThreadID.xy + 0.5) / (float2)screenSize.xy;

    if (all(dispatchThreadID.xy < screenSize))
    {   
        ViewConstants viewConstants;
        viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

        float4 color = getTexture2D(postProcessConstants.getSource()).SampleLevel(linearClamp, uv, 0);
        float2 linearDepth = getTexture2D(postProcessConstants.getLinearDepth()).SampleLevel(nearestClamp, uv, 0).rg;

        #if _PASS0
        float2 coc = ComputeCOC(linearDepth.y, viewConstants).xx;

        // Blur color and CoC
        {
            int sampleCount = 4;
            for (int j = -sampleCount; j < sampleCount; ++j)
            {
                for (int i = -sampleCount; i < sampleCount; ++i)
                {
                    float2 uvAdd = float2(float(i)+0.5, float(j)+0.5) / float(sampleCount*2);
                    float2 sampleUV = uv + 0.01 * uvAdd;
                    float2 sampleZ = getTexture2D(postProcessConstants.getLinearDepth()).SampleLevel(nearestClamp, sampleUV, 0).rg;
                    float newCOC = ComputeCOC(sampleZ.y, viewConstants);
                    
                    if (newCOC < 0)
                        coc.x = min(coc.x, newCOC);
                }
            }
        }

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = float4(color.rgb, max(abs(coc.x), coc.y));
        #elif _PASS1
        float4 downscaleCOC = getTexture2D(postProcessConstants.getTemp()).SampleLevel(nearestClamp, uv, 0).rgba;
        float coc = ComputeCOC(linearDepth.y, viewConstants);
              coc = downscaleCOC.a;
        const float epsCOC = 0.001f;
      
        int sampleCount0 = 4;
        for (int j = -sampleCount0; j < sampleCount0; ++j)
        {
            for (int i = -sampleCount0; i < sampleCount0; ++i)
            {
                float2 uvAdd = float2(float(i)+0.5, float(j)+0.5) / float(sampleCount0*2);
                float2 sampleUV = uv.xy + 0.0025 * uvAdd;
                float4 sample = getTexture2D(postProcessConstants.getTemp()).SampleLevel(nearestClamp, sampleUV, 0); 
                    
                coc += sample.a; 
            }
        }
        coc /= float(sampleCount0*sampleCount0*4+1);
    
        float totalWeight = 1.0f;
        int sampleCount = 8;
        float scaledCOC = coc;
            
        for (int j = -sampleCount; j < sampleCount; ++j)
        {
            for (int i = -sampleCount; i < sampleCount; ++i)
            {
                float2 uvAdd = float2(float(i)+0.5, float(j)+0.5) / float(sampleCount*2);
                float2 sampleUV = uv.xy + scaledCOC * uvAdd;
                float4 sample = getTexture2D(postProcessConstants.getTemp()).SampleLevel(nearestClamp, sampleUV, 0); 
                    
                float sampleWeight = saturate((sample.a - coc + 0.001)*256) ; // saturate((sample.a - coc + 0.01)*16);// (1-saturate(1*(linearDepth.y - sampleZ.y)));
                float g = (1.0-(uvAdd.x*uvAdd.x)) * (1.0-(uvAdd.y*uvAdd.y));
                sampleWeight *= g;                    

                if (sampleWeight > 0)
                {
                    color.rgb += sample.rgb * sampleWeight;
                    totalWeight += sampleWeight;
                }
            }
        }
        color.rgb /= totalWeight;

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = float4(color.rgb, coc);
        #endif
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
        
    RayQuery<RAY_FLAG_FORCE_OPAQUE> query;
    RayDesc ray;
    ray.Origin    = origin;
    ray.Direction = dir;
    ray.TMin      = nearDist;
    ray.TMax      = farDist;
    query.TraceRayInline(getTLAS(viewConstants.getTLASHandle()), 0, 0xff, ray);
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

                case DisplayMode::RayTracing_Committed_Hit:
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

                case DisplayMode::RayTracing_Committed_Hit:
                color.rgb = lerp(color.rgb, float3(0,1,0), 0.5f);
                break;

                case DisplayMode::RayTracing_Committed_InstanceID:
                {
                    uint instanceID = query.CommittedInstanceID();
                    color.rgb = float3(colors[instanceID % 6]);
                }
                break;

                case DisplayMode::RayTracing_Committed_GeometryIndex:
                {
                    uint geoIndex = query.CommittedGeometryIndex();
                    color.rgb = float3(frac(geoIndex.xxx / 16.0f));
                }
                break;

                case DisplayMode::RayTracing_Committed_PrimitiveIndex:
                {
                    uint primitiveIndex = query.CommittedPrimitiveIndex();
                    color.rgb = float3(frac(primitiveIndex.xxx / 256.0f));
                }
                break;

                case DisplayMode::RayTracing_Committed_Barycentrics:
                color.rgb = float3(query.CommittedTriangleBarycentrics(), 0);
                break;
                
                case DisplayMode::RayTracing_Committed_WorldPosition:
                case DisplayMode::RayTracing_Attributes_UV0:
                case DisplayMode::RayTracing_Attributes_Albedo:
                {
                    uint instanceDataOffset = query.CommittedInstanceID() * GPU_INSTANCE_DATA_ALIGNMENT;
                    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);
                    float4 instanceColor = instanceData.getInstanceColor();
                    
                    uint batchIndex = query.CommittedGeometryIndex();                  
                    GPUBatchData batchData = instanceData.loadGPUBatchData(instanceDataOffset, batchIndex);
                    GPUMaterialData materialData = batchData.loadGPUMaterialData();
                    float4 materialColor = materialData.getAlbedoColor();                
                    
                    uint primitiveIndex = query.CommittedPrimitiveIndex();
                    ByteAddressBuffer ib = getNonUniformBuffer(instanceData.getIndexBufferHandle());
                    uint index[3];
                    uint triangleStartIndex = primitiveIndex*3 + batchData.getStartIndex();
                    
                    if (instanceData.getIndexSize() == 2)
                    {
                        uint load0 = ib.Load( ((triangleStartIndex & ~1)<<1) + 0);
                        uint load1 = ib.Load( ((triangleStartIndex & ~1)<<1) + 4);
                        if (triangleStartIndex & 1)
                        {
                            index[0] = load0 >> 16;
                            index[1] = load1 & 0xFFFF;
                            index[2] = load1 >> 16;
                        }
                        else    
                        {
                            index[0] = load0 & 0xFFFF;
                            index[1] = load0 >> 16;
                            index[2] = load1 & 0xFFFF;
                        }
                    }
                    else
                    {
                         index[0] = ib.Load((triangleStartIndex<<2) + 0);
                         index[1] = ib.Load((triangleStartIndex<<2) + 4);
                         index[2] = ib.Load((triangleStartIndex<<2) + 8);
                    }

                    ByteAddressBuffer vb = getNonUniformBuffer(instanceData.getVertexBufferHandle());
                    uint vbOffset = instanceData.getVertexBufferOffset();
                    VertexFormat vertexFormat = instanceData.getVertexFormat();

                    Vertex verts[3];
                    verts[0].Load(vb, vertexFormat, index[0], vbOffset);
                    verts[1].Load(vb, vertexFormat, index[1], vbOffset);
                    verts[2].Load(vb, vertexFormat, index[2], vbOffset);

                    float3 bary = float3(0, query.CommittedTriangleBarycentrics());
                           bary.x = (1-bary.y-bary.z);
                    
                    Vertex vert;
                    vert.Interpolate(verts, bary);

                    DisplayFlags flags = viewConstants.getDisplayFlags();

                    float3 rayOrigin = query.WorldRayOrigin();        
                    float3 rayDirection = query.WorldRayDirection();  
                    float hitDistance = query.CommittedRayT();        
                    float3 worldPosition = rayOrigin + hitDistance * rayDirection;

                    float2 uv0 = materialData.GetUV0(vert.uv[0], vert.uv[1], worldPosition);
                    
                    float4 vertexColor = materialData.getVertexColorOut(vert.getColor(), instanceData.getInstanceColor(), flags);
                    float4 albedo = materialData.getAlbedo(uv0, vertexColor, flags, true);

                    switch(mode)
                    {
                        default:
                        break;

                        case DisplayMode::RayTracing_Committed_WorldPosition:
                        color.rgb = /*sRGB2Linear*/(float3(frac(worldPosition)));
                        break;
                        
                        case DisplayMode::RayTracing_Attributes_UV0:
                        color.rgb = sRGB2Linear(float3(frac(uv0), any(saturate(uv0) != uv0) ? 1 : 0));
                        break;
                        
                        case DisplayMode::RayTracing_Attributes_Albedo:
                        color.rgb = albedo.rgb;
                        break;
                    }
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

                case DisplayMode::RayTracing_Committed_Hit:
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

        float4 color = getTexture2D(postProcessConstants.getSource()).Load(address);

        // Select Anti-Aliasing mode
        #if _FXAA
        color.rgb = FXAA(getTexture2D(postProcessConstants.getSource()), uv, viewConstants.getDisplayMode()).rgb;
        #elif _SMAA
        color.rgb = SMAA(getTexture2D(postProcessConstants.getSource()), address).rgb;
        #endif

        #if SAMPLE_COUNT > 1
        // TODO: pass to shader if original depthstencil is MSAA
        float depth = loadDepthMSAA(postProcessConstants.getDepth(), address, 0);
        uint stencil = loadStencilMSAA(postProcessConstants.getStencil(), address, 0);
        #else
        float depth = loadDepth(postProcessConstants.getDepth(), address); 
        uint stencil = loadStencil(postProcessConstants.getStencil(), address);
        #endif
   
        #if _TOOLMODE

        #if _RAYTRACING
        color = DebugRayTracing(color, uv, screenSize, viewConstants);
        #endif

        // outline
        {
            
            float2 pcf = (float2)0.0f;
            for (int jj = -1; jj < 1; ++jj)
            {
                for (int ii = -1; ii < 1; ++ii)
                {
                    uint4 center = getTexture2D_UInt4(postProcessConstants.getOutlineMask()).Load(int3(coords.x + ii, coords.y + jj, 0));

                    bool visibleOutline = false;
                    bool hiddenOutline = false;

                    for (int j = -1; j <= 1; ++j)
                    {
                        for (int i = -1; i <= 1; ++i)
                        {
                            uint4 sample = getTexture2D_UInt4(postProcessConstants.getOutlineMask()).Load(int3(coords.x + ii + i, coords.y + jj + j,0));

                            if ((sample.a & ~(uint)OutlineMaskFlags::DepthFail) != (center.a & ~(uint)OutlineMaskFlags::DepthFail))
                            {
                                if (0 != sample.a)
                                {
                                    if (sample.a & (uint)OutlineMaskFlags::DepthFail)
                                        hiddenOutline = true;
                                    else
                                        visibleOutline = true;
                                }
                            }
                        }
                    }
                    
                    if (visibleOutline)
                        pcf += 1;

                    if (hiddenOutline)
                        pcf.y += 0.325;
                }
            }

            color.rgb = lerp(color.rgb, float3(0,1,0), saturate((pcf.x + pcf.y) / 4.0f));
        }

        switch(viewConstants.getDisplayMode())
        {
            default:
            break;
        
            case DisplayMode::PostProcess_Depth:
            color.rgb = frac(depth.xxx * float3(65536, 256, 1));
            break;

            case DisplayMode::PostProcess_LinearDepth:
            {
                #if 0
                // Compute linear depth from ZBuffer
                float linearDepth = viewConstants.getLinearDepth(depth);
                color.rgb = frac(linearDepth * (viewConstants.getCameraNearFar().y-viewConstants.getCameraNearFar().x));
                #else
                // Use resolved MSAA linear depth texture
                float2 linearDepth = getTexture2D(postProcessConstants.getLinearDepth()).SampleLevel(nearestClamp, uv, 0).rg;   
                       //linearDepth *= viewConstants.getCameraDepthRange();           
                //color.rgb = float3(frac(linearDepth.xy), (linearDepth.x == linearDepth.y) ? frac(linearDepth.x) : 0);
                color.rgb = frac(linearDepth.xxx); // minZ
                #endif
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

            case DisplayMode::PostProcess_DepthOfField:
            {
                float2 linearDepth = getTexture2D(postProcessConstants.getLinearDepth()).SampleLevel(nearestClamp, uv, 0).rg;
                float coc = color.a;
                color.rgb = lerp(color.rgb, float3(1,0,0), saturate(coc*128));  
            }
            break;

            case DisplayMode::Lighting_EnvironmentSpecularBRDF:
            color.rgb = getTexture2D(viewConstants.getSpecularBRDF()).SampleLevel(nearestClamp, uv, 0).rgb;
            break;

            case DisplayMode::PostProcess_OutlineMask:
            {
                uint sample = getTexture2D_UInt4(postProcessConstants.getOutlineMask()).Load(int3(coords,0)).a;
                uint id = sample & (uint)~0x80000000;
                float alpha = (sample & 0x80000000) ? 0.5 : 1.0;
                color.rgb = (0 != id.x) ? frac(float3(id.x * 31.0, id.x * 17.0, id.x * 59.0) / 255.0) : float3(0,0,0);
                color.rgb *= alpha;
            }
            break;
        }

        #endif // _TOOLMODE

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = color;
    }
}