#ifndef _BUFFER__HLSLI_
#define _BUFFER__HLSLI_

#ifdef VG_VULKAN
    #define USE_BYTEADDRESSBUFFER 1
    #define USE_RWBYTEADDRESSBUFFER 1
#else
    #define USE_BYTEADDRESSBUFFER 1
    #define USE_RWBYTEADDRESSBUFFER 1
#endif

#ifdef VG_VULKAN
    #if USE_BYTEADDRESSBUFFER == 1
        #define VK_DESCRIPTOR_TYPE_BYTEADDRESSBUFFER    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 
    #else
        #define VK_DESCRIPTOR_TYPE_BYTEADDRESSBUFFER    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    #endif
    #if USE_RWBYTEADDRESSBUFFER == 1
        #define VK_DESCRIPTOR_TYPE_RWBYTEADDRESSBUFFER  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER 
    #else
        #define VK_DESCRIPTOR_TYPE_RWBYTEADDRESSBUFFER  VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    #endif
#endif

#ifndef __cplusplus

#if !USE_BYTEADDRESSBUFFER
#define ByteAddressBuffer Buffer<uint>
#endif

#if !USE_RWBYTEADDRESSBUFFER
#define RWByteAddressBuffer RWBuffer<uint>
#endif

#if USE_BYTEADDRESSBUFFER 

    uint4 LoadU4    (ByteAddressBuffer _buffer, uint _offset)   { return _buffer.Load4(_offset << 2); }
    uint3 LoadU3    (ByteAddressBuffer _buffer, uint _offset)   { return _buffer.Load3(_offset << 2); }
    uint2 LoadU2    (ByteAddressBuffer _buffer, uint _offset)   { return _buffer.Load2(_offset << 2); }
    uint  LoadU     (ByteAddressBuffer _buffer, uint _offset)   { return _buffer.Load(_offset << 2); }

    float4 LoadF4   (ByteAddressBuffer _buffer, uint _offset)   { return float4(  asfloat( _buffer.Load4(_offset << 2) )  ); }
    float3 LoadF3   (ByteAddressBuffer _buffer, uint _offset)   { return float3(  asfloat( _buffer.Load3(_offset << 2) )  ); }
    float2 LoadF2   (ByteAddressBuffer _buffer, uint _offset)   { return float2(  asfloat( _buffer.Load2(_offset << 2) )  ); }
    float  LoadF    (ByteAddressBuffer _buffer, uint _offset)   { return asfloat( _buffer.Load(_offset << 2) ); }

#else

    uint4 LoadU4    (Buffer<uint> _buffer, uint _offset)        { return uint4( _buffer.Load(_offset), _buffer.Load(_offset + 1), _buffer.Load(_offset + 2), _buffer.Load(_offset + 3) ); }
    uint3 LoadU3    (Buffer<uint> _buffer, uint _offset)        { return uint3( _buffer.Load(_offset), _buffer.Load(_offset + 1), _buffer.Load(_offset + 2)); }
    uint2 LoadU2    (Buffer<uint> _buffer, uint _offset)        { return uint2( _buffer.Load(_offset), _buffer.Load(_offset + 1)); }
    uint  LoadU     (Buffer<uint> _buffer, uint _offset)        { return _buffer.Load(_offset); }

    float4 LoadF4   (Buffer<uint> _buffer, uint _offset)        { return float4(  asfloat(_buffer.Load(_offset) ), asfloat(_buffer.Load(_offset + 1) ), asfloat(_buffer.Load(_offset + 2) ), asfloat( _buffer.Load(_offset + 3) )  ); }
    float3 LoadF3   (Buffer<uint> _buffer, uint _offset)        { return float3(  asfloat(_buffer.Load(_offset) ), asfloat(_buffer.Load(_offset + 1) ), asfloat(_buffer.Load(_offset + 2) )  ); }
    float2 LoadF2   (Buffer<uint> _buffer, uint _offset)        { return float2(  asfloat(_buffer.Load(_offset) ), asfloat(_buffer.Load(_offset + 1) )  ); }
    float  LoadF    (Buffer<uint> _buffer, uint _offset)        { return asfloat( _buffer.Load(_offset) ); }

#endif // !USE_BYTEADDRESSBUFFER

#if USE_RWBYTEADDRESSBUFFER 

    void StoreU     (RWByteAddressBuffer _rwBuffer, uint _offset, uint _value)      { _rwBuffer.Store (_offset, _value); }
    void StoreU2    (RWByteAddressBuffer _rwBuffer, uint _offset, uint2 _value)     { _rwBuffer.Store2(_offset, _value); }
    void StoreU3    (RWByteAddressBuffer _rwBuffer, uint _offset, uint3 _value)     { _rwBuffer.Store3(_offset, _value); }
    void StoreU4    (RWByteAddressBuffer _rwBuffer, uint _offset, uint4 _value)     { _rwBuffer.Store4(_offset, _value); }

#else

    void StoreU     (RWBuffer<uint> _rwBuffer, uint _offset, uint _value)           { _rwBuffer[_offset + 0] = _value.x; }
    void StoreU2    (RWBuffer<uint> _rwBuffer, uint _offset, uint2 _value)          { _rwBuffer[_offset + 0] = _value.x; _rwBuffer[_offset + 1] = _value.y; }
    void StoreU3    (RWBuffer<uint> _rwBuffer, uint _offset, uint3 _value)          { _rwBuffer[_offset + 0] = _value.x; _rwBuffer[_offset + 1] = _value.y; _rwBuffer[_offset + 2] = _value.z; }
    void StoreU4    (RWBuffer<uint> _rwBuffer, uint _offset, uint4 _value)          { _rwBuffer[_offset + 0] = _value.x; _rwBuffer[_offset + 1] = _value.y; _rwBuffer[_offset + 2] = _value.z; _rwBuffer[_offset + 3] = _value.w; }

#endif // !USE_RWBYTEADDRESSBUFFER

#endif // !__cplusplus

#endif // !_BUFFER__HLSLI_