#ifndef _DEVICE_CONSTS__HLSLI_
#define _DEVICE_CONSTS__HLSLI_

#include "types.hlsli"

vg_enum_class_global(CullMode, u8,
    None   = 0x00,
    Front  = 0x01,
    Back   = 0x02
);

#endif // _DEVICE_CONSTS__HLSLI_