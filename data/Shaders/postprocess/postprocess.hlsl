#include "postprocess.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/depthstencil.hlsli"
#include "system/msaa.hlsli"
#include "system/instancedata.hlsli"
#include "system/color.hlsli"
#include "system/outlinemask.hlsli"
#include "system/debugdisplay.hlsli"
#include "raytracing/raytracing.hlsli"
#include "system/lighting.hlsli"

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

void outlineEdgeSample(uint2 _center, uint2 _sample, inout bool _visibleOutline, inout bool _hiddenOutline)
{
    //if ((_sample.a & ~(uint)OutlineMaskFlags::DepthFail) != (_center.a & ~(uint)OutlineMaskFlags::DepthFail))
    //{
    //    if (0 != _sample.a)
    //    {
    //        if (_sample.a & (uint)OutlineMaskFlags::DepthFail)
    //            _hiddenOutline = true;
    //        else
    //            _visibleOutline = true;
    //    }
    //}
    
    if (0 != _sample.x)
    {
        if (_center.x != _sample.x)
        {
            if (_sample.y & (uint) OutlineMaskFlags::DepthFail)
                _hiddenOutline = true;
            else
                _visibleOutline = true;
        }
    }
}

void outlineEdge(uint2 s00, uint2 s01, uint2 s02,
                 uint2 s10, uint2 s11, uint2 s12,
                 uint2 s20, uint2 s21, uint2 s22, inout float _visible, inout float _hidden)
{
    bool visibleOutline = false, hiddenOutline = false;

    outlineEdgeSample(s11, s00, visibleOutline, hiddenOutline);
    outlineEdgeSample(s11, s01, visibleOutline, hiddenOutline);
    outlineEdgeSample(s11, s02, visibleOutline, hiddenOutline);

    outlineEdgeSample(s11, s10, visibleOutline, hiddenOutline);
    //outlineEdgeSample(s11, s11, visibleOutline, hiddenOutline);
    outlineEdgeSample(s11, s12, visibleOutline, hiddenOutline);

    outlineEdgeSample(s11, s20, visibleOutline, hiddenOutline);
    outlineEdgeSample(s11, s21, visibleOutline, hiddenOutline);
    outlineEdgeSample(s11, s22, visibleOutline, hiddenOutline);
    
    if (visibleOutline)
        _visible += 1.0;
    else if (hiddenOutline)
        _hidden += 1.0;
}

float4 getOutlineColorSample(uint2 _center, uint2 _sample)
{
    float4 outline = (float4)0;
    
    if (0 != _sample.x)
    {
        if (_center.x != _sample.x)
        {
            if (_sample.y & (uint) OutlineMaskFlags::DepthFail)
                outline = float4(1,0,0,1);
            else
                outline = float4(0,1,0,0.5);
        }
    }
    
    return outline;
}

float4 getOutlineColor( uint2 s00, uint2 s01, uint2 s02,
                        uint2 s10, uint2 s11, uint2 s12,
                        uint2 s20, uint2 s21, uint2 s22)
{
    float4 outline = (float4)0;
    
    outline += getOutlineColorSample(s11, s00) / 8.0f;
    outline += getOutlineColorSample(s11, s01) / 8.0f;
    outline += getOutlineColorSample(s11, s02) / 8.0f;

    outline += getOutlineColorSample(s11, s10) / 8.0f;
    outline += getOutlineColorSample(s11, s12) / 8.0f;
                                              
    outline += getOutlineColorSample(s11, s20) / 8.0f;
    outline += getOutlineColorSample(s11, s21) / 8.0f;
    outline += getOutlineColorSample(s11, s22) / 8.0f;
        
    return outline;
}

#if _TOOLMODE
#if _RAYTRACING
float4 DebugRayTracing(float4 color, float2 uv, uint2 screenSize, ViewConstants viewConstants)
{
    DisplayMode mode = viewConstants.getDisplayMode();
    DisplayFlags flags = viewConstants.getDisplayFlags();
    
    if (!IsRaytracingDebugDisplayMode(mode))
        return color;
    
    // Clear background
    color = 0; 
    
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

    float nearHeight = 2.0f * tan(fovRadians / 2.0f) * nearDist;
    float farHeight = 2.0f * tan(fovRadians / 2.0f) * farDist;
    float nearWidth = nearHeight * viewRatio;
    float farWidth = farHeight * viewRatio;

    float3 farTopLeft = farCenter - camUp * (farHeight * 0.5f) + camRight * (farWidth * 0.5f);
    float3 farTopRight = farCenter - camUp * (farHeight * 0.5f) - camRight * (farWidth * 0.5f);
    float3 farBottomLeft = farCenter + camUp * (farHeight * 0.5f) + camRight * (farWidth * 0.5f);
    float3 farBottomRight = farCenter + camUp * (farHeight * 0.5f) - camRight * (farWidth * 0.5f);

    float3 nearTopLeft = nearCenter - camUp * (nearHeight * 0.5f) + camRight * (nearWidth * 0.5f);
    float3 nearTopRight = nearCenter - camUp * (nearHeight * 0.5f) - camRight * (nearWidth * 0.5f);
    float3 nearBottomLeft = nearCenter + camUp * (nearHeight * 0.5f) + camRight * (nearWidth * 0.5f);
    float3 nearBottomRight = nearCenter + camUp * (nearHeight * 0.5f) - camRight * (nearWidth * 0.5f);

    float3 origin = camPos;

    float w00 = (1.0f - uv.x) * (1.0f - uv.y);
    float w01 = uv.x * (1.0f - uv.y);
    float w10 = (1.0f - uv.x) * uv.y;
    float w11 = uv.x * uv.y;
        
    float3 nearDir = nearTopLeft * w00 + nearTopRight * w01 + nearBottomLeft * w10 + nearBottomRight * w11;
    float3 farDir = farTopLeft * w00 + farTopRight * w01 + farBottomLeft * w10 + farBottomRight * w11;
    float3 dir = normalize(farDir - nearDir);
    
    bool hitOpaque = false;
    float hitOpaqueDist = farDist;
    
    bool drawOpaque = true;
    bool drawAlpha = true;
    
    color = getRaytracedColor(getTLAS(viewConstants.getTLASHandle()), origin, dir, nearDist, farDist, drawOpaque, drawAlpha, flags, mode);

    return color;
}
#endif // _RAYTRACING
#endif // _TOOLMODE

void outlineEdgeColor(
    uint2 s00, uint2 s01, uint2 s02,
    uint2 s10, uint2 s11, uint2 s12,
    uint2 s20, uint2 s21, uint2 s22,
    inout float visible,
    inout float hidden,
    inout uint outlineID,
    inout uint depthFail)
{
    uint centerID = s11.x;
    //if (centerID == 0)
    //    return;

    uint2 neighbors[8] =
    {
        s00,s01,s02,
        s10,    s12,
        s20,s21,s22
    };
    
    //if (s11.y & (uint)OutlineMaskFlags::DepthFail)
    //    visible += 1;
    //
    //outlineID = 0;

    [unroll]
    for (uint i = 0; i < 8; ++i)
    {
        if (neighbors[i].x != centerID && neighbors[i].x != 0)
        {
            uint flags = neighbors[i].y;
            uint id    = flags & (uint)OutlineMaskFlags::CategoryMask;
            
            outlineID = id;
            
            //if ( (neighbors[i].y & (uint)OutlineMaskFlags::DepthFail) || neighbors[i].x == 0)
            //    hidden += 1.0;
            //else
            //    visible += 1.0;
            
             if (0 != (neighbors[i].y & (uint)OutlineMaskFlags::DepthFail))
                hidden += 1.0f;
            else
                visible += 1.0f;
    
            //return;
        }
    }
}

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
        
        #endif

        // Outline
        {
            Texture2D<uint2> outlineTex = getTexture2D_UInt2(postProcessConstants.getOutlineMask());
            uint2 s[5][5];

            s[0][0] = outlineTex.Load(int3(coords.x - 2, coords.y - 2,0)); s[0][1] = outlineTex.Load(int3(coords.x - 1, coords.y - 2,0)); s[0][2] = outlineTex.Load(int3(coords.x, coords.y - 2,0)); s[0][3] = outlineTex.Load(int3(coords.x + 1, coords.y - 2,0)); s[0][4] = outlineTex.Load(int3(coords.x + 2, coords.y - 2,0));
            s[1][0] = outlineTex.Load(int3(coords.x - 2, coords.y - 1,0)); s[1][1] = outlineTex.Load(int3(coords.x - 1, coords.y - 1,0)); s[1][2] = outlineTex.Load(int3(coords.x, coords.y - 1,0)); s[1][3] = outlineTex.Load(int3(coords.x + 1, coords.y - 1,0)); s[1][4] = outlineTex.Load(int3(coords.x + 2, coords.y - 1,0));
            s[2][0] = outlineTex.Load(int3(coords.x - 2, coords.y - 0,0)); s[2][1] = outlineTex.Load(int3(coords.x - 1, coords.y    ,0)); s[2][2] = outlineTex.Load(int3(coords.x, coords.y    ,0)); s[2][3] = outlineTex.Load(int3(coords.x + 1, coords.y    ,0)); s[2][4] = outlineTex.Load(int3(coords.x + 2, coords.y    ,0));
            s[3][0] = outlineTex.Load(int3(coords.x - 2, coords.y + 1,0)); s[3][1] = outlineTex.Load(int3(coords.x - 1, coords.y + 1,0)); s[3][2] = outlineTex.Load(int3(coords.x, coords.y + 1,0)); s[3][3] = outlineTex.Load(int3(coords.x + 1, coords.y + 1,0)); s[3][4] = outlineTex.Load(int3(coords.x + 2, coords.y + 1,0));
            s[4][0] = outlineTex.Load(int3(coords.x - 2, coords.y + 2,0)); s[4][1] = outlineTex.Load(int3(coords.x - 1, coords.y + 2,0)); s[4][2] = outlineTex.Load(int3(coords.x, coords.y + 2,0)); s[4][3] = outlineTex.Load(int3(coords.x + 1, coords.y + 2,0)); s[4][4] = outlineTex.Load(int3(coords.x + 2, coords.y + 2,0));
            
            float visible = 0.0f;
            float hidden  = 0.0f;
            uint  outlineID = 0xFFFFFFFF;
            uint  depthFail = 0;
            
            outlineEdgeColor(s[0][1], s[0][2], s[0][3],
                             s[1][1], s[1][2], s[1][3],
                             s[2][1], s[2][2], s[2][3],
                             visible, hidden, outlineID, depthFail);
            
            outlineEdgeColor(s[1][0], s[1][1], s[1][2],
                             s[2][0], s[2][1], s[2][2],
                             s[3][0], s[3][1], s[3][2],
                             visible, hidden, outlineID, depthFail);

            outlineEdgeColor(s[1][1], s[1][2], s[1][3],
                             s[2][1], s[2][2], s[2][3],
                             s[3][1], s[3][2], s[3][3],
                             visible, hidden, outlineID, depthFail);

            outlineEdgeColor(s[1][2], s[1][3], s[1][4],
                             s[2][2], s[2][3], s[2][4],
                             s[3][2], s[3][3], s[3][4],
                             visible, hidden, outlineID, depthFail);
            
            outlineEdgeColor(s[2][1], s[2][2], s[2][3],
                             s[3][1], s[3][2], s[3][3],
                             s[4][1], s[4][2], s[4][3],
                             visible, hidden, outlineID, depthFail);
            
            //outlineID = s[2][2].y & 0xF;
            
            if (0)
            {
                bool hide = (0 != (s[2][2].y & (uint)OutlineMaskFlags::DepthFail));
                
                if (outlineID != 0 && hide)
                {
                    float4 zFailColor = viewConstants.getZFailOutlineColor(outlineID);
                    color.rgb = lerp(color.rgb, zFailColor.rgb, zFailColor.a);
                }
            }
            else
            {
            
                float4 zPassColor = viewConstants.getZPassOutlineColor(outlineID);
                float4 zFailColor = viewConstants.getZFailOutlineColor(outlineID);
            
                visible = saturate(visible / 4.0f);
                hidden = min(saturate(hidden / 4.0f), 1 - visible);
            
                float4 outlineColor = zFailColor.rgba * hidden + zPassColor.rgba * visible;
            
                color.rgb = lerp(color.rgb, outlineColor.rgb, outlineColor.a);
            }
        }

        #if _TOOLMODE
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
            
            case DisplayMode::Outline_OutlineMaskID:
            {
                uint2 sample = getTexture2D_UInt2(postProcessConstants.getOutlineMask()).Load(int3(coords,0));
                uint id = sample.x & (uint)~0x80000000;
                float alpha = (sample.x & 0x80000000) ? 0.5 : 1.0;
                color.rgb = (0 != id.x) ? frac(float3(id.x * 31.0, id.x * 17.0, id.x * 59.0) / 255.0) : float3(0,0,0);
                color.rgb *= alpha;
            }
            break;
            
            case DisplayMode::Outline_OutlineMaskFlags:
            {
                uint2 sample = getTexture2D_UInt2(postProcessConstants.getOutlineMask()).Load(int3(coords,0));
                
                uint outlineID = sample.y & 0xF;
                
                color.rgb = viewConstants.getZPassOutlineColor(outlineID).rgb;
                
                if (sample.y & (uint)OutlineMaskFlags::DepthFail)
                    color.rgb *= 0.5f;
            }
            break;
            
            case DisplayMode::PostProcess_PixelChecker:
            {
                 switch (coords.x & 0x3)
                {
                    case 0 : color.rgb = float3(1,0,0); break;
                    case 1 : color.rgb = float3(0,1,0); break;
                    case 2 : color.rgb = float3(0,0,1); break;
                    case 3 : color.rgb = float3(1,1,1); break;
                }
                
                if ((coords.x>>2 & 1) == (coords.y>>2 & 1))
                {
                    color.rgb *= 0.25;
                }
            }
            break;
        }

        #endif // _TOOLMODE
        
        float4 fadeColor = postProcessConstants.getFadeColor();
        color.rgb = lerp(color.rgb, fadeColor.rgb, fadeColor.a);

        getRWTexture2D(postProcessConstants.getRWBufferOut())[coords] = color;
    }
}