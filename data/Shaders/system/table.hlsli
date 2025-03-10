#ifndef _TABLE_HLSLI_
#define _TABLE_HLSLI_

#include "types.hlsli"

// Bindless resource ranges (must be litteral constants)
#define BINDLESS_TEXTURE_START      0       // [0..16383]       (14 bits)
#define BINDLESS_BUFFER_START       16384   // [16384..32767]   (14 bits)
#define BINDLESS_RWTEXTURE_START    32768   // [32768..40959]   (13 bits)
#define BINDLESS_RWBUFFER_START     40960   // [40960..49151]   (13 bits)
#define BINDLESS_TLAS_START         49152   // [49152..49167]   (4 bits)

#define BINDLESS_TABLE_RANGE_MIN    0
#define BINDLESS_TABLE_RANGE_MAX    49168
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

#define  BINDLESS_RWBUFFER_COUNT    (BINDLESS_TLAS_START - BINDLESS_RWBUFFER_START)
#define  BINDLESS_RWBUFFER_INVALID  (BINDLESS_RWBUFFER_START + BINDLESS_RWBUFFER_COUNT - 1)
#define  BINDLESS_RWBUFFER_BINDING  3

#define  BINDLESS_TLAS_COUNT        (BINDLESS_TABLE_RANGE_MAX - BINDLESS_TLAS_START)
#define  BINDLESS_TLAS_INVALID      (BINDLESS_TLAS_START + BINDLESS_TLAS_COUNT - 1)
#define  BINDLESS_TLAS_BINDING      4

// Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)

vg_enum_class_global(ReservedSlot, uint,
    // Texture
    ImGuiFontTexSrv         = BINDLESS_TEXTURE_INVALID - 5,
    DefaultPBRTexSrv        = BINDLESS_TEXTURE_INVALID - 4,
    DefaultNormalTexSrv     = BINDLESS_TEXTURE_INVALID - 3,
    DefaultAlbedoTexSrv     = BINDLESS_TEXTURE_INVALID - 2,
    InvalidTextureCube      = BINDLESS_TEXTURE_INVALID - 1,
    InvalidTexture2D        = BINDLESS_TEXTURE_INVALID,

    FirstTexture            = ImGuiFontTexSrv,
    LastTexture             = InvalidTexture2D,

    // Buffer
    EditorPassBufSrv        = BINDLESS_BUFFER_INVALID - 7,
    TransparentPassBufSrv   = BINDLESS_BUFFER_INVALID - 6,
    MaterialDataBufSrv      = BINDLESS_BUFFER_INVALID - 5,
    InstanceDataBufSrv      = BINDLESS_BUFFER_INVALID - 4,
    SkinningMatricesBufSrv  = BINDLESS_BUFFER_INVALID - 3,
    LightsConstantBufSrv    = BINDLESS_BUFFER_INVALID - 2,
    ViewConstantsBufSrv     = BINDLESS_BUFFER_INVALID - 1,
    InvalidBuffer           = BINDLESS_BUFFER_INVALID,

    FirstBuffer             = InstanceDataBufSrv,
    LastBuffer              = InvalidBuffer,

    // RWTexture
    InvalidRWTexture2D      = BINDLESS_RWTEXTURE_INVALID,

    FirstRWTexture          = InvalidRWTexture2D,
    LastRWTexture           = InvalidRWTexture2D,

    // RWBuffer
    GPUDebugBufRW           = BINDLESS_RWBUFFER_INVALID - 1,
    InvalidRWBuffer         = BINDLESS_RWBUFFER_INVALID,

    FirstRWBuffer           = GPUDebugBufRW,
    LastRWBuffer            = InvalidRWBuffer,

    // Automatic
    None                    = 0x80000000
);

//// Texture SRV
#define RESERVEDSLOT_TEXSRV_DEFAULT_PBR         (BINDLESS_TEXTURE_INVALID - 4)
#define RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL      (BINDLESS_TEXTURE_INVALID - 3)
#define RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO      (BINDLESS_TEXTURE_INVALID - 2)
#define RESERVEDSLOT_TEXSRV_IMGUIFONTTEX        (BINDLESS_TEXTURE_INVALID - 1)

// Buffer SRV
#define RESERVEDSLOT_BUFSRV_TRANSPARENTPASS     (BINDLESS_BUFFER_INVALID - 6)
#define RESERVEDSLOT_BUFSRV_MATERIALDATA        (BINDLESS_BUFFER_INVALID - 5)
#define RESERVEDSLOT_BUFSRV_INSTANCEDATA        (BINDLESS_BUFFER_INVALID - 4)
#define RESERVEDSLOT_BUFSRV_SKINNINGMATRICES    (BINDLESS_BUFFER_INVALID - 3)
#define RESERVEDSLOT_BUFSRV_LIGHTSCONSTANTS     (BINDLESS_BUFFER_INVALID - 2)
#define RESERVEDSLOT_BUFSRV_VIEWCONSTANTS       (BINDLESS_BUFFER_INVALID - 1)

// RWBuffer
#define RESERVEDSLOT_RWBUFFER_GPUDEBUG          (BINDLESS_RWBUFFER_INVALID - 1)

#ifdef _DX12

// DX12 requires aliasing texture types to use different spaces

#define BINDLESS_TEXTURE_BINDING_1D              10  
#define BINDLESS_TEXTURE_BINDING_2D              20  
#define BINDLESS_TEXTURE_BINDING_2DMS            21  
#define BINDLESS_TEXTURE_BINDING_2D_UINT         22  
#define BINDLESS_TEXTURE_BINDING_2D_UINT2        23  
#define BINDLESS_TEXTURE_BINDING_2D_UINT4        24  
#define BINDLESS_TEXTURE_BINDING_2DMS_UINT2      25
#define BINDLESS_TEXTURE_BINDING_CUBE            26   
#define BINDLESS_TEXTURE_BINDING_3D              30  

#define BINDLESS_RWTEXTURE_BINDING_1D           210 
#define BINDLESS_RWTEXTURE_BINDING_2D           220 
#define BINDLESS_RWTEXTURE_BINDING_2D_FLOAT2    221 
#define BINDLESS_RWTEXTURE_BINDING_2D_ARRAY     222
#define BINDLESS_RWTEXTURE_BINDING_3D           230 

#elif defined(_VULKAN)

#define BINDLESS_TEXTURE_BINDING_1D               0 // <=> BINDLESS_TEXTURE_BINDING
#define BINDLESS_TEXTURE_BINDING_2D               0   
#define BINDLESS_TEXTURE_BINDING_CUBE             0   
#define BINDLESS_TEXTURE_BINDING_2DMS             0  
#define BINDLESS_TEXTURE_BINDING_2D_UINT          0  
#define BINDLESS_TEXTURE_BINDING_2D_UINT2         0  
#define BINDLESS_TEXTURE_BINDING_2D_UINT4         0 
#define BINDLESS_TEXTURE_BINDING_2DMS_UINT2       0 
#define BINDLESS_TEXTURE_BINDING_3D               0   

#define BINDLESS_RWTEXTURE_BINDING_1D             2 // <=> BINDLESS_RWTEXTURE_BINDING
#define BINDLESS_RWTEXTURE_BINDING_2D             2 
#define BINDLESS_RWTEXTURE_BINDING_2D_FLOAT2      2 
#define BINDLESS_RWTEXTURE_BINDING_2D_ARRAY       2
#define BINDLESS_RWTEXTURE_BINDING_3D             2 
#endif

#endif // _TABLE_HLSLI_