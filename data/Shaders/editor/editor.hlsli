#ifndef _EDITOR__HLSLI_
#define _EDITOR__HLSLI_

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct EditorRootConstants3D
{
    float4x4 mat;
    uint4 data;
    float4 color;

    #ifdef __cplusplus
    EditorRootConstants3D() :
        mat(float4x4::identity()),
        data({0,0,0,0})
    {

    }
    #endif

    void setBufferHandle(uint _value)           { data.x = (data.x & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint getBufferHandle()                      { return data.x & 0xFFFF; }

    void setVertexBufferOffset(uint _value)     { data.x = (data.x & ~0xFFFF0000UL) | (_value & 0xFFFF) << 16; }
    uint getVertexBufferOffset()                { return data.x >> 16; }
};

#define EditorRootConstants3DCount sizeof(EditorRootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(EditorRootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _EDITOR__HLSLI_