#ifndef _OUTLINEMASK__HLSLI_
#define _OUTLINEMASK__HLSLI_

#include "types.hlsli"

vg_enum_class(OutlineMaskFlags, uint,
    DepthFail = 0x8000
);

struct VS_Output_Outline
{
    float4 pos  : Position;
    float4 vpos : ViewPos;
};

struct PS_Output_Outline
{
    uint4 id : Color0;
};

#endif // _OUTLINEMASK__HLSLI_