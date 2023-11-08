#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

// Bindless resource ranges (must be litteral constants)
#define BINDLESS_TEXTURE_START      0      // [0..16383]       (14 bits)
#define BINDLESS_BUFFER_START       16384  // [16384..32767]   (14 bits)
#define BINDLESS_RWTEXTURE_START    32768  // [32768..40959]   (13 bits)
#define BINDLESS_RWBUFFER_START     40960  // [40960..49151]   (13 bits)

#define BINDLESS_TABLE_RANGE_MIN    0
#define BINDLESS_TABLE_RANGE_MAX    49152
#define BINDLESS_TABLE_SIZE         (BINDLESS_TABLE_RANGE_MAX-BINDLESS_TABLE_RANGE_MIN)

#define  BINDLESS_TEXTURE_COUNT      (BINDLESS_BUFFER_START - BINDLESS_TABLE_RANGE_MIN)
#define  BINDLESS_TEXTURE_INVALID    (BINDLESS_TEXTURE_START + BINDLESS_TEXTURE_COUNT - 1)
#define  BINDLESS_TEXTURE_BINDING    0

#define  BINDLESS_BUFFER_COUNT      (BINDLESS_RWTEXTURE_START - BINDLESS_BUFFER_START)
#define  BINDLESS_BUFFER_INVALID    (BINDLESS_BUFFER_START + BINDLESS_BUFFER_COUNT - 1)
#define  BINDLESS_BUFFER_BINDING    1

#define  BINDLESS_RWTEXTURE_COUNT   (BINDLESS_RWBUFFER_START - BINDLESS_RWTEXTURE_START)
#define  BINDLESS_RWTEXTURE_INVALID (BINDLESS_RWTEXTURE_START + BINDLESS_RWTEXTURE_COUNT - 1)
#define  BINDLESS_RWTEXTURE_BINDING 2

#define  BINDLESS_RWBUFFER_COUNT    (BINDLESS_TABLE_RANGE_MAX - BINDLESS_RWBUFFER_START)
#define  BINDLESS_RWBUFFER_INVALID  (BINDLESS_RWBUFFER_START + BINDLESS_RWBUFFER_COUNT - 1)
#define  BINDLESS_RWBUFFER_BINDING  3

// Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)

// Texture SRV
#define RESERVEDSLOT_TEXSRV_IMGUIFONTTEX       (BINDLESS_TEXTURE_INVALID - 1)

// Buffer SRV
#define RESERVEDSLOT_BUFSRV_VIEWCONSTANTS      (BINDLESS_BUFFER_INVALID - 1)

#ifndef __cplusplus

#include "system/buffer.hlsli"

#define PASTE(a,b) a##b

#ifdef VG_DX12
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, offset), PASTE(space, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, offset), PASTE(space, bind));
#elif defined(VG_VULKAN)
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) [[vk::binding(bind, 0)]] type name[];
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D, g_Texture1DTable, BINDLESS_TEXTURE_BINDING, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D, g_Texture2DTable, BINDLESS_TEXTURE_BINDING, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture3D, g_Texture3DTable, BINDLESS_TEXTURE_BINDING, BINDLESS_TEXTURE_START);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer, g_BufferTable, BINDLESS_BUFFER_BINDING, BINDLESS_BUFFER_START);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>, g_RWTexture2DTable, BINDLESS_RWTEXTURE_BINDING, BINDLESS_RWTEXTURE_START);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer, g_RWBufferTable, BINDLESS_RWBUFFER_BINDING, BINDLESS_RWBUFFER_START);

// use #define here because of a Vulkan shader compiler crash :(
#define getTexture1D(_handle)       (g_Texture1DTable[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D(_handle)       (g_Texture2DTable[_handle - BINDLESS_TEXTURE_START])
#define getTexture3D(_handle)       (g_Texture3DTable[_handle - BINDLESS_TEXTURE_START])
#define getBuffer(_handle)          (g_BufferTable[_handle - BINDLESS_BUFFER_START])
#define getRWTexture2D(_handle)     (g_RWTexture2DTable[_handle - BINDLESS_RWTEXTURE_START])
#define getRWBuffer(_handle)        (g_RWBufferTable[_handle - BINDLESS_RWBUFFER_START])

#endif // __cplusplus

#endif // _BINDLESS__HLSLI_