#ifndef _VERTEX__HLSLI_
#define _VERTEX__HLSLI_

#include "buffer.hlsli"

#define ToolmodeBufferHandle (BINDLESS_BUFFER_SRV_START+0)

struct ToolmodeConstants
{
    void Load(ByteAddressBuffer _buffer)
    {
        mouse.xy = LoadF2(_buffer, 0);
    }
    
    float2 mouse;
};

#endif // _VERTEX__HLSLI_