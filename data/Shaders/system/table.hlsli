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

//--------------------------------------------------------------------------------------
// Allocate fixed Textures/Buffers SRVs/UAVs slots top-down (dynamic slots are allocated bottom-up)
// Using an enum to ensure unique values across all resource types.
//-------------------------------------------------------------------------------------- 
vg_enum_class_global(ReservedSlot, u32,
    // Texture
    BlurNoiseTexSrv_First   = BINDLESS_TEXTURE_INVALID - 40,
    BlueNoiseTexSrc_Last    = BINDLESS_TEXTURE_INVALID - 9,
    ImGuiFontTexSrv         = BINDLESS_TEXTURE_INVALID - 8,
    DefaultEmissiveTexSrv   = BINDLESS_TEXTURE_INVALID - 7,
    DefaultPBRTexSrv        = BINDLESS_TEXTURE_INVALID - 6,
    DefaultNormalTexSrv     = BINDLESS_TEXTURE_INVALID - 5,
    DefaultAlbedoTexSrv     = BINDLESS_TEXTURE_INVALID - 4,
    DefaultWhiteTexSrv      = BINDLESS_TEXTURE_INVALID - 3,
    DefaultBlackTexSrv      = BINDLESS_TEXTURE_INVALID - 2,
    InvalidTextureCube      = BINDLESS_TEXTURE_INVALID - 1,
    InvalidTexture2D        = BINDLESS_TEXTURE_INVALID,

    FirstTexture            = BlurNoiseTexSrv_First,
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

// Texture SRV
#define RESERVEDSLOT_TEXSRV_BLUE_NOISE_FIRST        (uint)(ReservedSlot::BlurNoiseTexSrv_First)
#define RESERVEDSLOT_TEXSRV_BLUE_NOISE_LAST         (uint)(ReservedSlot::BlueNoiseTexSrc_Last)
#define RESERVEDSLOT_TEXSRV_IMGUIFONTTEX            (uint)(ReservedSlot::ImGuiFontTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_EMISSIVE        (uint)(ReservedSlot::DefaultEmissiveTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_PBR             (uint)(ReservedSlot::DefaultPBRTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL          (uint)(ReservedSlot::DefaultNormalTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO          (uint)(ReservedSlot::DefaultAlbedoTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_WHITE           (uint)(ReservedSlot::DefaultWhiteTexSrv)
#define RESERVEDSLOT_TEXSRV_DEFAULT_BLACK           (uint)(ReservedSlot::DefaultBlackTexSrv)
#define RESERVEDSLOT_TEXSRV_INVALID_TEXTURE_CUBE    (uint)(ReservedSlot::InvalidTextureCube)
#define RESERVEDSLOT_TEXSRV_INVALID_TEXTURE_2D      (uint)(ReservedSlot::InvalidTexture2D)

// Buffer SRV
#define RESERVEDSLOT_BUFSRV_EDITORPASS              (uint)(ReservedSlot::EditorPassBufSrv)      
#define RESERVEDSLOT_BUFSRV_TRANSPARENTPASS         (uint)(ReservedSlot::TransparentPassBufSrv) 
#define RESERVEDSLOT_BUFSRV_MATERIALDATA            (uint)(ReservedSlot::MaterialDataBufSrv)    
#define RESERVEDSLOT_BUFSRV_INSTANCEDATA            (uint)(ReservedSlot::InstanceDataBufSrv)    
#define RESERVEDSLOT_BUFSRV_SKINNINGMATRICES        (uint)(ReservedSlot::SkinningMatricesBufSrv)
#define RESERVEDSLOT_BUFSRV_LIGHTSCONSTANTS         (uint)(ReservedSlot::LightsConstantBufSrv)  
#define RESERVEDSLOT_BUFSRV_VIEWCONSTANTS           (uint)(ReservedSlot::ViewConstantsBufSrv)   
#define RESERVEDSLOT_BUFSRV_INVALID                 (uint)(ReservedSlot::InvalidBuffer)        

// RWBuffer
#define RESERVEDSLOT_RWBUFFER_GPUDEBUG              (uint)(ReservedSlot::GPUDebugBufRW)
#define RESERVEDSLOT_RWBUFFER_INVALID               (uint)(ReservedSlot::InvalidRWBuffer) 
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