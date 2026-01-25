#ifndef _OUTLINEMASK__HLSLI_
#define _OUTLINEMASK__HLSLI_

#include "types.hlsli"

#define OUTLINE_MASK_RESERVED_CATEGORIES 3
#define OUTLINE_MASK_CATEGORIES_MAX 16

// Bits 0-3 are used for Category
// The following values are reserved:
// - 0 (None)
// - 1 (Selected Object) 
// - 2 (Selected Prefab) 

vg_enum_class_global(OutlineMaskFlags, u16,
    Default         = 0x0000,
    CategoryMask    = 0x000F,
    DepthFail       = 0x0080
);

struct OutlineCategoryConstants
{
    float4 zPassOutlineColor;
    float4 zFailOutlineColor;
    float4 zFailFillColor;
};

#ifndef __cplusplus
struct VS_Output_Outline
{
    float4 pos  : Position;
    float4 vpos : ViewPos;
};

struct PS_Output_Outline
{
    uint2 value : Color0;  // .x = UID // .y = flags
};
#endif

#endif // _OUTLINEMASK__HLSLI_