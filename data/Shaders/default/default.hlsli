#ifndef _DEFAULT__HLSLI_
#define _DEFAULT__HLSLI_

#include "../system/constants.hlsli"

struct RootConstants3D
{
    float4x4 mat;
    uint4 data;
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _DEFAULT__HLSLI_