#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

#ifdef __cplusplus
#error bindless.hlsli should not be included from C++
#else
#include "table.hlsli"

#define PASTE(a,b) a##b

#ifdef VG_DX12
#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, offset), PASTE(space, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, offset), PASTE(space, bind));
#elif defined(VG_VULKAN)

#define DECL_DESCRIPTOR_RANGE_RO(type, name, bind, offset) type name[] : register(PASTE(t, bind));
#define DECL_DESCRIPTOR_RANGE_RW(type, name, bind, offset) type name[] : register(PASTE(u, bind));
#endif

// Read-only textures
DECL_DESCRIPTOR_RANGE_RO(Texture1D,             g_Texture1DTable,       BINDLESS_TEXTURE_BINDING_1D, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D,             g_Texture2DTable,       BINDLESS_TEXTURE_BINDING_2D, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(TextureCube,           g_TextureCubeTable,     BINDLESS_TEXTURE_BINDING_CUBE, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2DMS<float4>,   g_Texture2DMSTable,     BINDLESS_TEXTURE_BINDING_2DMS, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture2D<uint2>,      g_Texture2DTable_UInt2, BINDLESS_TEXTURE_BINDING_2D_UINT2, BINDLESS_TEXTURE_START);
DECL_DESCRIPTOR_RANGE_RO(Texture3D,             g_Texture3DTable,       BINDLESS_TEXTURE_BINDING_3D, BINDLESS_TEXTURE_START);

// Read-only buffers
DECL_DESCRIPTOR_RANGE_RO(ByteAddressBuffer,     g_BufferTable,          BINDLESS_BUFFER_BINDING, BINDLESS_BUFFER_START);

// Read-Only buffers
DECL_DESCRIPTOR_RANGE_RW(RWTexture1D<float4>,   g_RWTexture1DTable,     BINDLESS_RWTEXTURE_BINDING_1D, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture2D<float4>,   g_RWTexture2DTable,     BINDLESS_RWTEXTURE_BINDING_2D, BINDLESS_RWTEXTURE_START);
DECL_DESCRIPTOR_RANGE_RW(RWTexture3D<float4>,   g_RWTexture3DTable,     BINDLESS_RWTEXTURE_BINDING_3D, BINDLESS_RWTEXTURE_START);

// Read-Write buffers
DECL_DESCRIPTOR_RANGE_RW(RWByteAddressBuffer, g_RWBufferTable, BINDLESS_RWBUFFER_BINDING, BINDLESS_RWBUFFER_START);

// TLAS is a different descriptor type on Vulkan, hence the separate bindless table
// The table cannot be declared in shader (even if unused) when RayTracing is not allowed (not supported or Renderdoc capture) otherwise Vulkan will complain about SPIR-V Extension being used
#ifdef _RAYTRACING
DECL_DESCRIPTOR_RANGE_RO(RaytracingAccelerationStructure, g_TLASTable, BINDLESS_TLAS_BINDING, BINDLESS_TLAS_START);
#endif

// Bindless resources access

#define getTexture1D(_handle)       (g_Texture1DTable[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D(_handle)       (g_Texture2DTable[_handle - BINDLESS_TEXTURE_START])
#define getTextureCube(_handle)     (g_TextureCubeTable[_handle - BINDLESS_TEXTURE_START])
#define getTexture2DMS(_handle)     (g_Texture2DMSTable[_handle - BINDLESS_TEXTURE_START])
#define getTexture2D_UInt2(_handle) (g_Texture2DTable_UInt2[_handle - BINDLESS_TEXTURE_START])
#define getTexture3D(_handle)       (g_Texture3DTable[_handle - BINDLESS_TEXTURE_START])
#define getBuffer(_handle)          (g_BufferTable[_handle - BINDLESS_BUFFER_START])
#define getRWTexture1D(_handle)     (g_RWTexture1DTable[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture2D(_handle)     (g_RWTexture2DTable[_handle - BINDLESS_RWTEXTURE_START])
#define getRWTexture3D(_handle)     (g_RWTexture3DTable[_handle - BINDLESS_RWTEXTURE_START])
#define getRWBuffer(_handle)        (g_RWBufferTable[_handle - BINDLESS_RWBUFFER_START])
#define getTLAS(_handle)            (g_TLASTable[_handle - BINDLESS_TLAS_START])

// error: generated SPIR-V is invalid: OpLoad Pointer <id> '113[%113]' is not a logical pointer. %115 = OpLoad %type_2d_image_0 %113

//Texture1D getTexture1D(uint _handle)                    { return g_Texture1DTable[_handle - BINDLESS_TEXTURE_START]; }
//Texture2D getTexture2D(uint _handle)                    { return g_Texture2DTable[_handle - BINDLESS_TEXTURE_START]; }
//Texture3D getTexture3D(uint _handle)                    { return g_Texture3DTable[_handle - BINDLESS_TEXTURE_START]; }
//
//RWTexture1D<float4> getRWTexture1D(uint _handle)        { return g_RWTexture1DTable[_handle - BINDLESS_RWTEXTURE_START]; }
//RWTexture2D<float4> getRWTexture2D(uint _handle)        { return g_RWTexture2DTable[_handle - BINDLESS_RWTEXTURE_START]; }
//RWTexture3D<float4> getRWTexture3D(uint _handle)        { return g_RWTexture3DTable[_handle - BINDLESS_RWTEXTURE_START]; }
//
//ByteAddressBuffer getBuffer(uint _handle)               { return g_BufferTable[_handle - BINDLESS_BUFFER_START]; }
//RWByteAddressBuffer getRWBuffer(uint _handle)           { return g_RWBufferTable[_handle - BINDLESS_RWBUFFER_START]; }
//
//RaytracingAccelerationStructure getTLAS(uint _handle)   { return g_TLASTable[_handle - BINDLESS_TLAS_START]; }

#endif // __cplusplus

#endif // _BINDLESS__HLSLI_