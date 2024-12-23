#ifndef _BACKGROUND__HLSLI_
#define _BACKGROUND__HLSLI_

#include "system/quad.hlsli"
#include "system/constants.hlsli"

struct BackgroundRootConstants
{
    void reset()
    {
        quad.reset();
        color = float4(1,1,1,1);
    }  

    QuadConstants   quad;
    float4          color;
};

#ifndef __cplusplus
DECL_ROOTCONSTANTS(BackgroundRootConstants, rootConstants, 0, 0);
#endif

#endif // _BACKGROUND__HLSLI_