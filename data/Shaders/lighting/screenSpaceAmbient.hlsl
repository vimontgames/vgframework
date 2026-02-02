#include "screenSpaceAmbient.hlsli"
#include "system/bindless.hlsli"
#include "system/samplers.hlsli"
#include "system/view.hlsli"
#include "system/msaa.hlsli"
#include "system/depthstencil.hlsli"
#include "lighting/GBuffer.hlsli"

[numthreads(SCREEN_SPACE_AMBIENT_OCCLUSION_THREADGROUP_SIZE_X, SCREEN_SPACE_AMBIENT_OCCLUSION_THREADGROUP_SIZE_X, 1)]
void CS_ScreenSpaceAmbient(int2 dispatchThreadID : SV_DispatchThreadID)
{
    
}