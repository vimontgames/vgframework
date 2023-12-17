#include "postprocess.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"

[numthreads(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y, 1)]
void CS_PostProcessMain(int2 dispatchThreadID : SV_DispatchThreadID)
{   
    uint2 src_dst = unpackUint16(postProcessConstants.srv_uav);
    uint2 width_height = unpackUint16(postProcessConstants.width_height);
    
    int2 coords = dispatchThreadID;
    float2 uv = dispatchThreadID.xy / (float2)width_height;

    if (all(dispatchThreadID.xy < width_height))
    {
        float4 color = getTexture2D(src_dst.x).Load(int3(dispatchThreadID.xy, 0));

        #if 0
        color.rgb = dot(color.rgb,0.33f).xxx;
        #endif

        getRWTexture2D(src_dst.y)[coords] = color;
    }
}