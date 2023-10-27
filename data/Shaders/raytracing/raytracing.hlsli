#ifndef _RAYTRACING__HLSLI_
#define _RAYTRACING__HLSLI_

#include "system/constants.hlsli"

struct RayTracingRootConstants
{
    float4 test;
};

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RayTracingRootConstants, rootConstants, 0, 0);
#endif

#endif // _RAYTRACING__HLSLI_