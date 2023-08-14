#ifndef _BUFFER__HLSLI_
#define _BUFFER__HLSLI_

#ifndef __cplusplus

#ifdef VULKAN
#define USE_BYTEADDRESSBUFFER 0
#define ByteAddressBuffer Buffer<uint>
#else
#define USE_BYTEADDRESSBUFFER 1
#endif

template <typename T> void Load(out T _value, ByteAddressBuffer _buffer, uint _offset = 0);

#if USE_BYTEADDRESSBUFFER 

template<> void Load(out uint4 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = _buffer.Load4(_offset << 2); }
template<> void Load(out uint3 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = _buffer.Load3(_offset << 2); }
template<> void Load(out uint2 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = _buffer.Load2(_offset << 2); }
template<> void Load(out uint  _value, ByteAddressBuffer _buffer, uint _offset)   { _value = _buffer.Load(_offset << 2); }

template<> void Load(out float4 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = float4(asfloat(_buffer.Load4(_offset << 2))); }
template<> void Load(out float3 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = float3(asfloat(_buffer.Load3(_offset << 2))); }
template<> void Load(out float2 _value, ByteAddressBuffer _buffer, uint _offset)   { _value = float2(asfloat(_buffer.Load2(_offset << 2))); }
template<> void Load(out float  _value, ByteAddressBuffer _buffer, uint _offset)   { _value = asfloat(_buffer.Load(_offset << 2)); }

#else

template<> void Load(out uint4 _value, Buffer<uint> _buffer, uint _offset)	        { _value = uint4(_buffer.Load(_offset), _buffer.Load(_offset + 1), _buffer.Load(_offset + 2), _buffer.Load(_offset + 3)); }
template<> void Load(out uint3 _value, Buffer<uint> _buffer, uint _offset)	        { _value = uint3(_buffer.Load(_offset), _buffer.Load(_offset + 1), _buffer.Load(_offset + 2)); }
template<> void Load(out uint2 _value, Buffer<uint> _buffer, uint _offset)	        { _value = uint2(_buffer.Load(_offset), _buffer.Load(_offset + 1)); }
template<> void Load(out uint  _value, Buffer<uint> _buffer, uint _offset)	        { _value = _buffer.Load(_offset); }

template<> void Load(out float4 _value, Buffer<uint> _buffer, uint _offset)	        { _value = float4(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2)), asfloat( _buffer.Load(_offset + 3)) ); }
template<> void Load(out float3 _value, Buffer<uint> _buffer, uint _offset)	        { _value = float3(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)), asfloat(_buffer.Load(_offset + 2))); }
template<> void Load(out float2 _value, Buffer<uint> _buffer, uint _offset)	        { _value = float2(asfloat(_buffer.Load(_offset)), asfloat(_buffer.Load(_offset + 1)) ); }
template<> void Load(out float  _value, Buffer<uint> _buffer, uint _offset)	        { _value = asfloat(_buffer.Load(_offset)); }
#endif

#endif // __cplusplus

#endif // _BUFFER__HLSLI_