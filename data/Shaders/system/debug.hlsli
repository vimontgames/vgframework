#pragma once

#define GPUDEBUG_TOTAL_BUFFER_SIZE     4096
#define GPUDEBUG_STRING_SIZE            256

enum GPUDebugMsgType
{
    None,
    Info,
    Warning,
    Error,
    Assert
};

#ifndef __cplusplus

static uint g_File = 0;
    
template<uint SIZE> void DebugPrint(uint _msg[SIZE], GPUDebugMsgType _type, uint _file, uint _line)
{
    RWByteAddressBuffer buffer = getRWBuffer(RESERVEDSLOT_RWBUFFER_GPUDEBUG);

    uint alignedSize = ((SIZE + 3) & ~3) + 4;

    uint offset;
    buffer.InterlockedAdd(0, alignedSize+4+4, offset);  // +4 for message type, +4 for ending (uint)'\0' to end string 

    // Save message type (4 bits, [0..16], file guid (12 bits, [0..4096] files), and line (16 bits, [0..65535] lines)
    uint header = (uint)_type | (_file<<4) | (_line<<16);
    buffer.Store(offset+4, header); 

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

        buffer.Store(offset + 4 + 4 + 4*i, value);  // +4 for counter at the beginning of the buffer, +4 for message type before message
    }

    buffer.Store(offset + 4 + 4 + alignedSize, 0); // (uint)'\0'
}

void StoreArg(uint _value)
{
    RWByteAddressBuffer buffer = getRWBuffer(RESERVEDSLOT_RWBUFFER_GPUDEBUG);
    uint offset;
    buffer.InterlockedAdd(0, 4, offset);
    buffer.Store(offset, _value);
}

void DebugPrintArg(uint value)
{
    StoreArg(value);
}

void DebugPrintArg(float value)
{
    StoreArg(asuint(value));
}

void DebugPrintArgs()
{
}

template<typename T0> void DebugPrintArgs(T0 arg0)
{
    DebugPrintArg(arg0);
}

template<typename T0, typename T1> void DebugPrintArgs(T0 arg0, T1 arg1)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
}

template<typename T0, typename T1, typename T2> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
}

template<typename T0, typename T1, typename T2, typename T3> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2, T3 arg3)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
    DebugPrintArgs(arg3);
}

#define VG_GPUDEBUGMSG(_type, _file, _line, _str, ...) do { \
    DebugPrint(_str, _type, _file, _line);                  \
    DebugPrintArgs(__VA_ARGS__);                            \
}                                                           \
while (0)

#define VG_DEBUGPRINT(_str, ...)    VG_GPUDEBUGMSG(GPUDebugMsgType::None,    g_File, __LINE__, _str, __VA_ARGS__)
#define VG_INFO(_str, ...)          VG_GPUDEBUGMSG(GPUDebugMsgType::Info,    g_File, __LINE__, _str, __VA_ARGS__)
#define VG_WARNING(_str, ...)       VG_GPUDEBUGMSG(GPUDebugMsgType::Warning, g_File, __LINE__, _str, __VA_ARGS__)
#define VG_ERROR(_str, ...)         VG_GPUDEBUGMSG(GPUDebugMsgType::Error,   g_File, __LINE__, _str, __VA_ARGS__)
#define VG_ASSERT(_cond, _str, ...) do { if (_cond == false) { VG_GPUDEBUGMSG(GPUDebugMsgType::Assert, g_File, __LINE__, _str, __VA_ARGS__); } } while (0)

#endif // !__cplusplus