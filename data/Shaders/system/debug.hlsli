#ifndef _DEBUG__HLSLI_
#define _DEBUG__HLSLI_

//--------------------------------------------------------------------------------------
// GPU "printf"
// e.g.
// VG_DEBUGPRINT("Hello World");
// VG_DEBUGPRINT("A = int2(%u, %u)", coords.x, coords.y);
//
// Know limitation:
// - Only float, int and uint args are supported
// - Max height args
//--------------------------------------------------------------------------------------       

#define GPUDEBUG_TOTAL_BUFFER_SIZE  4096
#define GPUDEBUG_MAX_ARG_COUNT      8

enum GPUDebugMsgType
{
    None = 0,
    Info,
    Warning,
    Error,
    Assert
};

#ifndef __cplusplus

static uint g_File = 0;
static uint g_ArgOffset = 0;
    
template<uint SIZE> bool DebugPrint(uint _msg[SIZE], GPUDebugMsgType _type, uint _file, uint _line, uint _argCount)
{
    RWByteAddressBuffer buffer = getRWBuffer(RESERVEDSLOT_RWBUFFER_GPUDEBUG);

    uint stringSize = ((SIZE + 3) & ~3) + 4; // string size rounded up to 4 bytes
    uint elementSize = 4 + 4 + stringSize + _argCount * 4;
    
    uint offset;
    buffer.InterlockedAdd(0, elementSize, offset);  // header + message size (rounded up to 4 bytes) + 4 bytes per arg
    offset += 4; // skip global counter
    
    if (offset + elementSize > GPUDEBUG_TOTAL_BUFFER_SIZE)
        return false;
    
    // Save debug message header
    uint header1 = ((uint)_type & 0x000F) | (_argCount << 8) | ((stringSize & 0xFFFF) <<16);
    buffer.Store(offset, header1); 
    offset += 4;
    
    uint header2 = _line | ((_file & 0xFFFF) << 16);
    buffer.Store(offset, header2); 
    offset += 4;
    
    for (uint i = 0; i < stringSize / 4; ++i)
    {
        uint offset0 = i*4 + 0;
        uint offset1 = i*4 + 1;
        uint offset2 = i*4 + 2;
        uint offset3 = i*4 + 3;

        uint char0 = offset0 < SIZE ? _msg[offset0] : 0;
        uint char1 = offset1 < SIZE ? _msg[offset1] : 0;
        uint char2 = offset2 < SIZE ? _msg[offset2] : 0;
        uint char3 = offset3 < SIZE ? _msg[offset3] : 0;

        uint value = char3<<24 | char2<<16 | char1<<8 | char0;

        buffer.Store(offset, value);  // +4 for counter at the beginning of the buffer, +4 for header type before message
        offset += 4;
    }
    
    g_ArgOffset = offset;
    
    // clear args for debug (not strictly necessary)
    //for (uint i = 0; i < _argCount; ++i)
    //    buffer.Store(offset + i * 4,  0xFFFFFFFF); 
    return true;
}

void StoreArg(uint _value)
{    
    RWByteAddressBuffer buffer = getRWBuffer(RESERVEDSLOT_RWBUFFER_GPUDEBUG);
    buffer.Store(g_ArgOffset, _value);
    g_ArgOffset += 4;
}

void DebugPrintArg(uint value)
{
    StoreArg(value);
}

void DebugPrintArg(int value)
{
    StoreArg(asuint(value));
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

template<typename T0, typename T1, typename T2, typename T3, typename T4> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
    DebugPrintArgs(arg3);
    DebugPrintArgs(arg4);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
    DebugPrintArgs(arg3);
    DebugPrintArgs(arg4);
    DebugPrintArgs(arg5);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
    DebugPrintArgs(arg3);
    DebugPrintArgs(arg4);
    DebugPrintArgs(arg5);
    DebugPrintArgs(arg6);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> void DebugPrintArgs(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
    DebugPrintArgs(arg0);
    DebugPrintArgs(arg1);
    DebugPrintArgs(arg2);
    DebugPrintArgs(arg3);
    DebugPrintArgs(arg4);
    DebugPrintArgs(arg5);
    DebugPrintArgs(arg6);
    DebugPrintArgs(arg7);
}

#define VARIADIC_ARG_COUNT(...) VARIADIC_ARG_COUNT_IMPL(_0, ##__VA_ARGS__, 8,7,6,5,4,3,2,1,0)
#define VARIADIC_ARG_COUNT_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_N,...) _N

#define VG_GPUDEBUGMSG(_type, _file, _line, _str, ...) do {                         \
    if (DebugPrint(_str, _type, _file, _line, VARIADIC_ARG_COUNT(__VA_ARGS__)))     \
        DebugPrintArgs(__VA_ARGS__);                                                \
}                                                                                   \
while (0)

#ifdef _TOOLMODE
#define VG_DEBUGPRINT(_str, ...)    VG_GPUDEBUGMSG(GPUDebugMsgType::None,    g_File, __LINE__, _str, __VA_ARGS__)
#define VG_INFO(_str, ...)          VG_GPUDEBUGMSG(GPUDebugMsgType::Info,    g_File, __LINE__, _str, __VA_ARGS__)
#define VG_WARNING(_str, ...)       VG_GPUDEBUGMSG(GPUDebugMsgType::Warning, g_File, __LINE__, _str, __VA_ARGS__)
#define VG_ERROR(_str, ...)         VG_GPUDEBUGMSG(GPUDebugMsgType::Error,   g_File, __LINE__, _str, __VA_ARGS__)
#define VG_ASSERT(_cond, _str, ...) do { if (_cond == false) { VG_GPUDEBUGMSG(GPUDebugMsgType::Assert, g_File, __LINE__, _str, __VA_ARGS__); } } while (0)

#else
#define HLSL_NOOP do {} while(0)

#define VG_DEBUGPRINT(_str, ...)    HLSL_NOOP   
#define VG_INFO(_str, ...)          HLSL_NOOP
#define VG_WARNING(_str, ...)       HLSL_NOOP
#define VG_ERROR(_str, ...)         HLSL_NOOP
#define VG_ASSERT(_cond, _str, ...) HLSL_NOOP

#endif

#endif // !__cplusplus

#endif // _DEBUG__HLSLI_