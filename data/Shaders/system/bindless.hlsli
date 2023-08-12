#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

// Bindless resource ranges (must be litteral constants)
#define BINDLESS_TEXTURE_SRV_START      0      // [0..16383]       (14 bits)
#define BINDLESS_BUFFER_SRV_START       16384  // [16384..32767]   (14 bits)
#define BINDLESS_TEXTURE_UAV_START      32768  // [32768..40959]   (13 bits)
#define BINDLESS_BUFFER_UAV_START       40960  // [40960..49151]   (13 bits)
#define BINDLESS_CONSTANTBUFFER_START   49152  // [49152..57343]   (13 bits)

#define BINDLESS_TABLE_RANGE_MIN        0
#define BINDLESS_TABLE_RANGE_MAX        57344
#define BINDLESS_TABLE_SIZE             (BINDLESS_TABLE_RANGE_MAX-BINDLESS_TABLE_RANGE_MIN)

#define  BINDLESS_TEXTURE_SRV_COUNT      (BINDLESS_BUFFER_SRV_START - BINDLESS_TABLE_RANGE_MIN)
#define  BINDLESS_TEXTURE_SRV_INVALID    (BINDLESS_TEXTURE_SRV_START + BINDLESS_TEXTURE_SRV_COUNT - 1)
#define  BINDLESS_TEXTURE_SRV_BINDING    0

#define  BINDLESS_BUFFER_SRV_COUNT       (BINDLESS_TEXTURE_UAV_START - BINDLESS_BUFFER_SRV_START)
#define  BINDLESS_BUFFER_SRV_INVALID     (BINDLESS_BUFFER_SRV_START + BINDLESS_BUFFER_SRV_COUNT - 1)
#define  BINDLESS_BUFFER_SRV_BINDING     1

#define  BINDLESS_TEXTURE_UAV_COUNT      (BINDLESS_BUFFER_UAV_START - BINDLESS_TEXTURE_UAV_START)
#define  BINDLESS_TEXTURE_UAV_INVALID    (BINDLESS_TEXTURE_UAV_START + BINDLESS_TEXTURE_UAV_COUNT - 1)
#define  BINDLESS_TEXTURE_UAV_BINDING    2

#define  BINDLESS_BUFFER_UAV_COUNT       (BINDLESS_CONSTANTBUFFER_START - BINDLESS_BUFFER_UAV_START)
#define  BINDLESS_BUFFER_UAV_INVALID     (BINDLESS_BUFFER_UAV_START + BINDLESS_BUFFER_UAV_COUNT - 1)
#define  BINDLESS_BUFFER_UAV_BINDING     3

#define  BINDLESS_CONSTANTBUFFER_COUNT   (BINDLESS_TABLE_RANGE_MAX - BINDLESS_CONSTANTBUFFER_START)
#define  BINDLESS_CONSTANTBUFFER_INVALID (BINDLESS_CONSTANTBUFFER_START + BINDLESS_CONSTANTBUFFER_COUNT - 1)
#define  BINDLESS_CONSTANTBUFFER_BINDING 4

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
#elif defined(VULKAN)
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D, g_Texture1DTable, 0, BINDLESS_TEXTURE_SRV_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D, g_Texture2DTable, 0, BINDLESS_TEXTURE_SRV_START);
DECL_DESCRIPTOR_RANGE_RO(Texture3D, g_Texture3DTable, 0, BINDLESS_TEXTURE_SRV_START);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer, g_BufferTable, 1, BINDLESS_BUFFER_SRV_START);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>, g_RWTexture2DTable, 2, BINDLESS_TEXTURE_UAV_START);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer, g_RWBufferTable, 3, BINDLESS_BUFFER_UAV_START);

// use #define here because of a Vulkan shader compiler crash :(
#define getTexture2D(_handle)       (g_Texture2DTable[_handle - BINDLESS_TEXTURE_SRV_START])
#define getBuffer(_handle)          (g_BufferTable[_handle - BINDLESS_BUFFER_SRV_START])

#define getRWTexture2D(_handle)     (g_RWTexture2DTable[_handle BINDLESS_TEXTURE_UAV_START])
#define getRWBuffer(_handle)        (g_RWBufferTable[_handle - BINDLESS_BUFFER_UAV_START])

//#define getConstantBuffer(_handle)  (RWBufferTable[_handle - BINDLESS_BUFFER_UAV_START])

#endif // _DRIVER__HLSLI_

#endif // __cplusplus