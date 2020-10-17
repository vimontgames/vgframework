#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#include "../system/quad.hlsli"
#include "../system/constants.hlsli"

struct RootConstants
{
    QuadConstants   quad;
    uint            texID;
};

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants, rootConstants, 0, 0);
#endif

#endif // _DRIVER__HLSLI_