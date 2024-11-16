#ifndef _ROOTCONSTANTS3D__HLSLI_
#define _ROOTCONSTANTS3D__HLSLI_

#include "constants.hlsli"
#include "displaymodes.hlsli"

enum RootConstantsFlags : uint
{
    Wireframe = 0x0001
};

struct RootConstants3D
{
    float4x4 world;             // world matrix (TODO: pass float3x4?)

    uint stream0;               // unused (16) | stream0 address (16)
    uint stream0Offset;         // stream0 offset (might need more than 16 bits because of skinning)
    uint picking;               // PickingID
    uint misc;                  // flags (16) | format (8) | matID (8)
    uint instanceDataOffset;    // Offset in Buffer RESERVEDSLOT_BUFSRV_INSTANCEDATA with Instance data (slot 0 holds default instance data)

    #ifdef __cplusplus
    RootConstants3D() :
        world(float4x4::identity()),
        stream0(0x0000FFFF),
        stream0Offset(0),
        picking(0),
        misc(0),
        instanceDataOffset(0)
    {

    }
    #endif

    void setWorldMatrix(float4x4 _world)
    {
        world = _world;
    }
    
    float4x4 getWorldMatrix()
    {
        return world;
    }

    void setGPUInstanceDataOffset(uint _offset)
    {
        instanceDataOffset = _offset;
    }

    uint getGPUInstanceDataOffset()
    {
        return instanceDataOffset;
    }

    // vertex buffer
    // stream0 address : 16;
    // stream0 offset  : 16
    void setVertexBufferHandle(uint _stream0, uint _offset = 0)
    {
        stream0 = packUint16low(stream0, _stream0);
        stream0Offset = _offset;
    }

    uint getVertexBufferHandle()
    {
        return unpackUint16low(stream0);
    }
    
    uint getVertexBufferOffset()
    {
        return stream0Offset;
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
};

#define RootConstants3DCount sizeof(RootConstants3D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants3D, rootConstants3D, 0, 0);
#endif

#endif // _ROOTCONSTANTS3D__HLSLI_