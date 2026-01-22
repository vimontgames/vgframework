#ifndef _MATERIAL_CONSTS__HLSLI_
#define _MATERIAL_CONSTS__HLSLI_

#include "device_consts.hlsli"

vg_enum_class_global(SurfaceType, u8,
    Opaque = 0,
    AlphaTest,
    AlphaBlend,
    Decal
);

vg_enum_class_global(SurfaceTypeFlags, u8,
    Opaque      =  1U << (uint)SurfaceType::Opaque,
    AlphaTest   =  1U << (uint)SurfaceType::AlphaTest,
    AlphaBlend  =  1U << (uint)SurfaceType::AlphaBlend,
    Decal       =  1U << (uint)SurfaceType::Decal
);

vg_enum_class_global(AlphaSource, u8,
    None = 0,
    Albedo,
    Emissive
);

vg_enum_class_global(UVSource, u8,
    UV0 = 0,
    UV1,
    PlanarX,
    PlanarY,
    PlanarZ,
    FlipBook
);

vg_enum_class_global(InstanceColorMask, u8,
    Albedo   = 0x01,
    Emissive = 0x02
);

vg_enum_class_global(VertexColorMask, u8,
    Albedo   = 0x01,
    Emissive = 0x02
);

#endif