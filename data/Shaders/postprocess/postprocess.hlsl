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
    getRWTexture2D(src_dst.y)[coords] = getTexture2D(src_dst.x).SampleLevel(nearestRepeat, uv, 0);
    
    //getRWTexture2D(src_dst.y)[coords] = float4(frac(dispatchThreadID.x / (float) POSTPROCESS_THREADGROUP_SIZE_X), frac(dispatchThreadID.y / (float) POSTPROCESS_THREADGROUP_SIZE_Y), 0, 1);
}