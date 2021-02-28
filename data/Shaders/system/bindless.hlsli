#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

#ifndef __cplusplus

#ifdef DX12
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(t##offset, space##bind);
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(u##offset, space##bind);
#elif defined(VULKAN)
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D, Texture1DTable, 0, 0);
DECL_DESCRIPTOR_RANGE_RO(Texture2D, Texture2DTable, 0, 0);
DECL_DESCRIPTOR_RANGE_RO(Texture3D, Texture3DTable, 0, 0);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer, BufferTable, 1, 16384);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>, RWTexture2DTable, 2, 32768);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer, RWBufferTable, 3, 49152);

// Constant buffers
//DECL_CONSTANT_BUFFER(name, type)

//#ifdef DX12
//ConstantBuffer<UniformBufferTest> ubo[] : register(b65500, space4);
//#else
//[[vk::binding(4, 0)]] ConstantBuffer<UniformBufferTest> ubo[];
//#endif

//Texture1D<float4> getTexture1D(uint _handle) { return Texture1DTable[_handle]; }
//Texture2D<float4> getTexture2D(uint _handle) { return Texture2DTable[_handle]; }
//Texture3D<float4> getTexture3D(uint _handle) { return Texture3DTable[_handle]; }

// use #define here because of a Vulkan shader compiler crash :(
#define getTexture2D(_handle) Texture2DTable[_handle]

ByteAddressBuffer getBuffer(uint _handle) { return BufferTable[_handle - 16384]; }

#endif // _DRIVER__HLSLI_

#endif // __cplusplus