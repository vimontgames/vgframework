#ifndef _ROOTCONSTANTS2D__HLSLI_
#define _ROOTCONSTANTS2D__HLSLI_

#include "quad.hlsli"
#include "constants.hlsli"

struct RootConstants2D
{
    void reset()
    {
        quad.reset();
        texID = -1;
    }    

    QuadConstants quad;
    uint texID;
};

#define RootConstants2DCount sizeof(RootConstants2D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants2D, rootConstants2D, 0, 0);
#endif

#endif // _ROOTCONSTANTS2D__HLSLI_