#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

// Slots 0..15 are reserved for dynamic buffers
#define DYNAMIC_BUFFER_START            0       // [0..15]

// Bindless resource ranges (must be litteral constants)
#define BINDLESS_TEXTURE_SRV_START      16      // [16..32767]      (15 bits)
#define BINDLESS_BUFFER_SRV_START       32768   // [32768..49151]   (14 bits)
#define BINDLESS_TEXTURE_UAV_START      49152   // [49152..57343]   (13 bits)
#define BINDLESS_BUFFER_UAV_START       57344   // [40960..65535]   (13 bits)

#define BINDLESS_TABLE_RANGE_MIN        16
#define BINDLESS_TABLE_RANGE_MAX        65536
#define BINDLESS_TABLE_SIZE             (BINDLESS_TABLE_RANGE_MAX-BINDLESS_TABLE_RANGE_MIN)

// Read-only textures
#define BINDLESS_TEXTURE_SRV_COUNT      (BINDLESS_BUFFER_SRV_START - BINDLESS_TABLE_RANGE_MIN)
#define BINDLESS_TEXTURE_SRV_INVALID    (BINDLESS_TEXTURE_SRV_START + BINDLESS_TEXTURE_SRV_COUNT - 1)
#define BINDLESS_TEXTURE_SRV_BINDING    0

// Read-only buffers
#define BINDLESS_BUFFER_SRV_COUNT       (BINDLESS_TEXTURE_UAV_START - BINDLESS_BUFFER_SRV_START)
#define BINDLESS_BUFFER_SRV_INVALID     (BINDLESS_BUFFER_SRV_START + BINDLESS_BUFFER_SRV_COUNT - 1)
#define BINDLESS_BUFFER_SRV_BINDING     1

// Read/Write textures
#define BINDLESS_TEXTURE_UAV_COUNT      (BINDLESS_BUFFER_UAV_START - BINDLESS_TEXTURE_UAV_START)
#define BINDLESS_TEXTURE_UAV_INVALID    (BINDLESS_TEXTURE_UAV_START + BINDLESS_TEXTURE_UAV_COUNT - 1)
#define BINDLESS_TEXTURE_UAV_BINDING    2

// Read/Write buffers
#define BINDLESS_BUFFER_UAV_COUNT       (BINDLESS_TABLE_RANGE_MAX - BINDLESS_BUFFER_UAV_START)
#define BINDLESS_BUFFER_UAV_INVALID     (BINDLESS_BUFFER_UAV_START + BINDLESS_BUFFER_UAV_COUNT - 1)
#define BINDLESS_BUFFER_UAV_BINDING     3

#ifndef __cplusplus

#ifdef VULKAN
#define USE_BYTEADDRESSBUFFER 0
#define ByteAddressBuffer Buffer<uint>
#else
#define USE_BYTEADDRESSBUFFER 1
#endif

#define PASTE(a,b) a##b

#ifdef DX12
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, offset), PASTE(space, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, offset), PASTE(space, bind));
#define DECL_DESCRIPTOR_RANGE_CB(type, name, bind, offset) type name[] : register(PASTE(b, offset), PASTE(space, bind));
#elif defined(VULKAN)
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D, g_Texture1DTable, BINDLESS_TEXTURE_SRV_BINDING, BINDLESS_TEXTURE_SRV_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D, g_Texture2DTable, BINDLESS_TEXTURE_SRV_BINDING, BINDLESS_TEXTURE_SRV_START);
DECL_DESCRIPTOR_RANGE_RO(Texture3D, g_Texture3DTable, BINDLESS_TEXTURE_SRV_BINDING, BINDLESS_TEXTURE_SRV_START);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer, g_BufferTable, BINDLESS_BUFFER_SRV_BINDING, BINDLESS_BUFFER_SRV_START);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>, g_RWTexture2DTable, BINDLESS_TEXTURE_UAV_BINDING, BINDLESS_TEXTURE_UAV_START);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer, g_RWBufferTable, BINDLESS_BUFFER_UAV_BINDING, BINDLESS_BUFFER_UAV_START);

// use #define here because of a Vulkan shader compiler crash :(
#define getTexture2D(_handle)       (g_Texture2DTable[_handle - BINDLESS_TEXTURE_SRV_START])
#define getBuffer(_handle)          (g_BufferTable[_handle - BINDLESS_BUFFER_SRV_START])
#define getRWTexture2D(_handle)     (g_RWTexture2DTable[_handle BINDLESS_TEXTURE_UAV_START])
#define getRWBuffer(_handle)        (g_RWBufferTable[_handle - BINDLESS_BUFFER_UAV_START])

#endif // _DRIVER__HLSLI_

#endif // __cplusplus