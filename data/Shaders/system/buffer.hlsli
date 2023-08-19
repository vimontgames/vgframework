#ifndef _BUFFER__HLSLI_
#define _BUFFER__HLSLI_

#ifndef __cplusplus

#ifdef VULKAN
#define USE_BYTEADDRESSBUFFER 0
#define ByteAddressBuffer Buffer<uint>
#else
#define USE_BYTEADDRESSBUFFER 1
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
#endif

#endif // __cplusplus

#endif // _BUFFER__HLSLI_