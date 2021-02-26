#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#include "../system/quad.hlsli"
#include "../system/constants.hlsli"

struct RootConstants2D
{
    QuadConstants   quad;
    uint            texID;
};

#define RootConstants2DCount sizeof(RootConstants2D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants2D, rootConstants2D, 0, 0);
#endif

#endif // _DRIVER__HLSLI_