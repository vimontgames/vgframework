#ifndef _PICKING__HLSLI_
#define _PICKING__HLSLI_

#include "packing.hlsli"
#include "buffer.hlsli"

struct PickingConstants
{
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {
        mousePos = LoadU2(_buffer, _offset);
    }
    
    uint2 mousePos;
};

#endif // _FORWARDPASSCONSTANTS__HLSLI_