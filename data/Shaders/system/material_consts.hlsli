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

#endif