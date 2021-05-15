#ifndef _DEFAULT__HLSLI_
#define _DEFAULT__HLSLI_

#include "../system/constants.hlsli"

#define FLAG_NONE       0x0000

#define MODE_DEFAULT    0x0000
#define MODE_MATID      0x0001
#define MODE_NORMAL	    0x0002
#define MODE_UV0		0x0003
#define MODE_ALBEDO     0x0004

struct RootConstants3D
{
    float4x4 mat;
    uint4 data;

    #ifdef __cplusplus
    RootConstants3D() :
        mat(float4x4::identity()),
        data({0,0,0,0})
    {

    }
    #endif

    void setBuffer(uint _value)     { data.x = (data.x & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint getBuffer()                { return data.x & 0xFFFF; }

    void setTexture(uint _value)    { data.y = (data.y & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint getTexture()               { return data.y & 0xFFFF; }

    void setFlags(uint _value)      { data.w = (data.w & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint getFlags()                 { return data.w & 0xFFFF; }

    void setMode(uint _value)       { data.w = (data.w & ~0x00FF0000UL) | ((_value & 0xFF) << 16); }
    uint getMode()                  { return (data.w >> 16) & 0xFF; }

    void setMatID(uint _value)      { data.w = (data.w & ~0xFF000000UL) | ((_value & 0xFF) << 24); }
    uint getMatID()                 { return (data.w >> 24) & 0xFF; }
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _DEFAULT__HLSLI_