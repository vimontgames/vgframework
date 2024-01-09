#pragma once

#define GPUDEBUGBUFFER_SIZE 1024

#ifndef __cplusplus

//--------------------------------------------------------------------------------------
// #include "system/debug.hlsli"
// 
// uint hello[] = { 'B','e','n', 'u','a','l','d','o' }; 
// DebugPrint(hello);
//--------------------------------------------------------------------------------------
    
template<uint SIZE> void DebugPrint(uint _msg[SIZE])
{
    #if _TOOLMODE

    RWByteAddressBuffer buffer = getRWBuffer(RESERVEDSLOT_RWBUFFER_GPUDEBUG);

    uint alignedSize = ((SIZE + 3) & ~3) + 4;

    uint offset;
    buffer.InterlockedAdd(0, alignedSize, offset);

    for (uint i = 0; i < alignedSize / 4; ++i)
    {
        uint offset0 = i*4+0;
        uint offset1 = i*4+1;
        uint offset2 = i*4+2;
        uint offset3 = i*4+3;

        uint char0 = offset0 < SIZE ? _msg[offset0] : 0;
        uint char1 = offset1 < SIZE ? _msg[offset1] : 0;
        uint char2 = offset2 < SIZE ? _msg[offset2] : 0;
        uint char3 = offset3 < SIZE ? _msg[offset3] : 0;

        uint value = char3<<24 | char2<<16 | char1<<8 | char0;

        buffer.Store(offset + 4 + 4*i, value);
    }

    buffer.Store(offset + 4 + alignedSize + 4, 0); // '\0'x4

    #endif // _TOOLMODE
}

#endif // !__cplusplus