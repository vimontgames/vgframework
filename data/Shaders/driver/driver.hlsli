#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#include "../system/quad.hlsli"
#include "../system/constants.hlsli"

struct CBConstants
{
    float4 color0;
    float4 color1;
};

struct RootConstants
{
    QuadConstants   quad;
    uint            texID;
    float4x4        mat;
};

#define RootConstantsCount sizeof(RootConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants, rootConstants, 0, 0);
#endif

#endif // _DRIVER__HLSLI_