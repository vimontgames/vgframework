#ifndef _OUTLINEMASK__HLSLI_
#define _OUTLINEMASK__HLSLI_

#include "types.hlsli"

vg_enum_class(OutlineMaskFlags, uint,
    DepthFail = 0x8000
);

#endif // _OUTLINEMASK__HLSLI_