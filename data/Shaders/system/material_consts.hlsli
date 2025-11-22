#ifndef _MATERIAL_CONSTS__HLSLI_
#define _MATERIAL_CONSTS__HLSLI_

#include "types.hlsli"

vg_enum_class_global(UVSource, uint,
    UV0 = 0,
    UV1,
    PlanarX,
    PlanarY,
    PlanarZ
);

vg_enum_class_global(SurfaceType, uint,
    Opaque = 0,
    AlphaTest,
    AlphaBlend,
    Decal
);

vg_enum_class_global(SurfaceTypeFlags, uint,
    Opaque      =  1U << (uint)SurfaceType::Opaque,
    AlphaTest   =  1U << (uint)SurfaceType::AlphaTest,
    AlphaBlend  =  1U << (uint)SurfaceType::AlphaBlend,
    Decal       =  1U << (uint)SurfaceType::Decal
);

#endif