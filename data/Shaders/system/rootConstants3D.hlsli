#ifndef _ROOTCONSTANTS3D__HLSLI_
#define _ROOTCONSTANTS3D__HLSLI_

#include "constants.hlsli"
#include "options.hlsli"

enum RootConstantsFlags : uint
{
    Wireframe = 0x0001
};

struct RootConstants3D
{
    float4x4 world; // world matrix (TODO: pass float3x4)
    float4 color;   // instance color (TODO: pass u32)
    uint buffer;    // stream0 | unused
    uint offset;    // stream0 offset
    uint mat;       // abeldo | normal (TODO : pass material constant buffer 16-bits handle instead)
    uint picking;   // PickingID
    uint misc;      // flags (16) | format (8) | matID (8)

    #ifdef __cplusplus
    RootConstants3D() :
        world(float4x4::identity()),
        color(float4(1,1,1,1)),
        buffer(0xFFFFFFFF),
        offset(0),
        mat(0xFFFFFFFF),
        picking(0),
        misc(0)
    {

    }
    #endif

    // buffer
    // uint Stream0BufferHandle     : 16;
    // uint Stream1BufferHandle     : 16
    void setBufferHandle(uint _stream0)
    {
        buffer = packUint16(uint2(_stream0, 0));
    }
    uint getBufferHandle()
    {
        return unpackUint16(buffer).x;
    }
    
    void setBufferOffset(uint _offset)
    {
        offset = _offset;
    }

    uint getBufferOffset()
    {
        return offset;
    }

    // mat
    // uint AlbedoTex : 16
    // uint NormalTex : 16
    void setAlbedoTextureHandle(uint _value)
    {
        mat = (mat & ~0x0000FFFFUL) | (_value & 0xFFFF);
    }
    uint getAlbedoTextureHandle()
    {
        return mat & 0xFFFF;
    }
    void setNormalTextureHandle(uint _value)
    {
        mat = (mat & ~0xFFFF0000UL) | (_value & 0xFFFF) << 16;
    }
    uint getNormalTextureHandle()
    {
        return mat >> 16;
    }
    
    // picking
    // uint pickingID : 32
    void setPickingID(uint _value)
    {
        picking = _value;
    }
    
    uint getPickingID()
    {
        return picking;
    }

    // misc
    // uint flags   : 16 (unused)
    // uint format  : 8 
    // uint matID   : 8 
    void setFlags(uint _value)
    {
        misc = (misc & ~0x0000FFFFUL) | (_value & 0xFFFF);
    }
    uint getFlags()
    {
        return misc & 0xFFFF;
    }
    void setVertexFormat(VertexFormat _value)
    {
        misc = (misc & ~0x00FF0000UL) | ((((uint) _value) & 0xFF) << 16);
    }
    VertexFormat getVertexFormat()
    {
        return (VertexFormat) ((misc >> 16) & 0xFF);
    }
    void setMatID(uint _value)
    {
        misc = (misc & ~0xFF000000UL) | ((_value & 0xFF) << 24);
    }
    uint getMatID()
    {
        return (misc >> 24) & 0xFF;
    }
    
    void setWorldMatrix(float4x4 _world)
    {
        world = _world;
    }
    
    float4x4 getWorldMatrix()
    {
        return world;
    }
    
    void setColor(float4 _color)
    {
        color = _color;
    }
    
    float4 getColor()
    {
        return color;
    }
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _ROOTCONSTANTS3D__HLSLI_