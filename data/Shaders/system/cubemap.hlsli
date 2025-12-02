#ifndef _CUBEMAP__HLSLI_
#define _CUBEMAP__HLSLI_

#include "types.hlsli"

vg_enum_class_global(CubemapFace, u8,
	PositiveX = 0,
	NegativeX,
	PositiveY,
	NegativeY,
    PositiveZ,
    NegativeZ
);

#endif // _CUBEMAP__HLSLI_