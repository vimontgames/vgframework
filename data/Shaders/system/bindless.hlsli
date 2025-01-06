#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

#ifdef __cplusplus
#error bindless.hlsli should not be included from C++
#else
#include "table.hlsli"

#define PASTE(a,b) a##b

#ifdef _DX12
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, offset), PASTE(space, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, offset), PASTE(space, bind));
#elif defined(_VULKAN)

#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, bind));
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D,             g_Texture1D,                BINDLESS_TEXTURE_BINDING_1D, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D,             g_Texture2D,                BINDLESS_TEXTURE_BINDING_2D, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(TextureCube,           g_TextureCube,              BINDLESS_TEXTURE_BINDING_CUBE, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2DMS<float4>,   g_Texture2DMS,              BINDLESS_TEXTURE_BINDING_2DMS, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D<uint>,       g_Texture2D_UInt,           BINDLESS_TEXTURE_BINDING_2D_UINT, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D<uint2>,      g_Texture2D_UInt2,          BINDLESS_TEXTURE_BINDING_2D_UINT2, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D<uint4>,      g_Texture2D_UInt4,          BINDLESS_TEXTURE_BINDING_2D_UINT4, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2DMS<uint2>,    g_Texture2DMS_UInt2,        BINDLESS_TEXTURE_BINDING_2DMS_UINT2, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture3D,             g_Texture3D,                BINDLESS_TEXTURE_BINDING_3D, BINDLESS_TEXTURE_START);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer,     g_Buffer,                   BINDLESS_BUFFER_BINDING, BINDLESS_BUFFER_START);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture1D<float4>,       g_RWTexture1D,          BINDLESS_RWTEXTURE_BINDING_1D, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>,       g_RWTexture2D,          BINDLESS_RWTEXTURE_BINDING_2D, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float2>,       g_RWTexture2D_float2,   BINDLESS_RWTEXTURE_BINDING_2D_FLOAT2, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture2DArray<float4>,  g_RWTexture2DArray,     BINDLESS_RWTEXTURE_BINDING_2D_ARRAY, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture3D<float4>,       g_RWTexture3D,          BINDLESS_RWTEXTURE_BINDING_3D, BINDLESS_RWTEXTURE_START);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer,       g_RWBuffer, BINDLESS_RWBUFFER_BINDING, BINDLESS_RWBUFFER_START);

// TLAS is a different descriptor type on Vulkan, hence the separate bindless table
// The table cannot be declared in shader (even if unused) when RayTracing is not allowed (not supported or Renderdoc capture) otherwise Vulkan will complain about SPIR-V Extension being used
#ifdef _RAYTRACING
DECL_DESCRIPTOR_RANGE_RO(RaytracingAccelerationStructure, g_TLAS, BINDLESS_TLAS_BINDING, BINDLESS_TLAS_START);
#endif

// Bindless resources access

#define getTexture1D(_handle)           (g_Texture1D[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D(_handle)           (g_Texture2D[_handle - BINDLESS_TEXTURE_START])
#define getNonUniformTexture2D(_handle) (g_Texture2D[NonUniformResourceIndex(_handle - BINDLESS_TEXTURE_START)])
#define getTextureCube(_handle)         (g_TextureCube[_handle - BINDLESS_TEXTURE_START])
#define getTexture2DMS(_handle)         (g_Texture2DMS[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D_UInt(_handle)      (g_Texture2D_UInt[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D_UInt2(_handle)     (g_Texture2D_UInt2[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D_UInt4(_handle)     (g_Texture2D_UInt4[_handle - BINDLESS_TEXTURE_START])
#define getTexture2DMS_UInt2(_handle)   (g_Texture2DMS_UInt2[_handle - BINDLESS_TEXTURE_START])
#define getTexture3D(_handle)           (g_Texture3D[_handle - BINDLESS_TEXTURE_START])

#define getBuffer(_handle)              (g_Buffer[_handle - BINDLESS_BUFFER_START])
#define getNonUniformBuffer(_handle)    (g_Buffer[NonUniformResourceIndex(_handle - BINDLESS_BUFFER_START)])

#define getRWTexture1D(_handle)         (g_RWTexture1D[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture2D(_handle)         (g_RWTexture2D[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture2D_float2(_handle)  (g_RWTexture2D_float2[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture2DArray(_handle)    (g_RWTexture2DArray[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture3D(_handle)         (g_RWTexture3D[_handle - BINDLESS_RWTEXTURE_START])

#define getRWBuffer(_handle)            (g_RWBuffer[_handle - BINDLESS_RWBUFFER_START])

#define getTLAS(_handle)                (g_TLAS[_handle - BINDLESS_TLAS_START])

#endif // __cplusplus

#endif // _BINDLESS__HLSLI_