#ifndef _ROOTCONSTANTS3D__HLSLI_
#define _ROOTCONSTANTS3D__HLSLI_

#include "constants.hlsli"
#include "displayoptions.hlsli"

enum RootConstantsFlags : uint
{
    Wireframe = 0x0001
};

struct RootConstants3D
{
    uint4 data;
    float4x4 mat;
    float4 color;

    #ifdef __cplusplus
    RootConstants3D() :
        mat(float4x4::identity()),
        data(uint4(0,0,0,0))
    {

    }
    #endif

    // data.x 
    // uint BufferHandle        : 16;
    // uint VertexBufferOffset  : 16
    void setBufferHandle(uint _value)
    {
        data.x = (data.x & ~0x0000FFFFUL) | (_value & 0xFFFF);
    }
    uint getBufferHandle()
    {
        return data.x & 0xFFFF;
    }
    void setVertexBufferOffset(uint _value)
    {
        data.x = (data.x & ~0xFFFF0000UL) | (_value & 0xFFFF) << 16;
    }
    uint getVertexBufferOffset()
    {
        return data.x >> 16;
    }

    // data.y 
    // uint AlbedoTex : 16
    // uint NormalTex : 16
    void setAlbedoTextureHandle(uint _value)
    {
        data.y = (data.y & ~0x0000FFFFUL) | (_value & 0xFFFF);
    }
    uint getAlbedoTextureHandle()
    {
        return data.y & 0xFFFF;
    }
    void setNormalTextureHandle(uint _value)
    {
        data.y = (data.y & ~0xFFFF0000UL) | (_value & 0xFFFF) << 16;
    }
    uint getNormalTextureHandle()
    {
        return data.y >> 16;
    }
    
    // data.z
    // uint pickingID : 32
    void setPickingID(uint _value)
    {
        data.z = _value;
    }
    
    uint getPickingID()
    {
        return data.z;
    }

    // data.w
    // uint flags   : 16 (unused)
    // uint matID   : 8 
    // uint _pad    : 8 (unused)
    void setFlags(uint _value)
    {
        data.w = (data.w & ~0x0000FFFFUL) | (_value & 0xFFFF);
    }
    uint getFlags()
    {
        return data.w & 0xFFFF;
    }
    void setMatID(uint _value)
    {
        data.w = (data.w & ~0xFF000000UL) | ((_value & 0xFF) << 24);
    }
    uint getMatID()
    {
        return (data.w >> 24) & 0xFF;
    }
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _ROOTCONSTANTS3D__HLSLI_