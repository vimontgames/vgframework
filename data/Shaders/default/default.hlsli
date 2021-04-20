#ifndef _DEFAULT__HLSLI_
#define _DEFAULT__HLSLI_

#include "../system/constants.hlsli"

#define DBG_NORMAL 0x0001

struct RootConstants3D
{
    float4x4 mat;
    uint4 data;

    void setBuffer(uint _value)     { data.x = _value & 0xFFFF; }
    uint getBuffer()                { return data.x & 0xFFFF; }

    void setTexture(uint _value)    { data.y = _value & 0xFFFF; }
    uint getTexture()               { return data.y & 0xFFFF; }

    void setFlags(uint _value)      { data.w = _value & 0xFFFF; }
    uint getFlags()                 { return data.w & 0xFFFF; }
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _DEFAULT__HLSLI_