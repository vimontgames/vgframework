#pragma once

#define PRECOMPUTE_IBL_THREADGROUP_SIZE_X 16
#define PRECOMPUTE_IBL_THREADGROUP_SIZE_Y 16
#define PRECOMPUTE_IBL_THREADGROUP_SIZE_Z 1

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PrecomputeIBLConstants
{
    #ifdef __cplusplus
    PrecomputeIBLConstants()
    {
        src_dst = 0x0;
    }
    #endif

    void setSource              (uint _tex2D)       { src_dst = packUint16low(src_dst, _tex2D); }
    uint getSource              ()                  { return unpackUint16low(src_dst); }

    void setDestination         (uint _rwtex2D)     { src_dst = packUint16high(src_dst, _rwtex2D); }
    uint getDestination         ()                  { return unpackUint16high(src_dst); }

    void setDestinationSize     (uint2 _size)       { size = _size.x | (_size.y << 16); }
    uint2 getDestinationSize    ()                  { return uint2(size & 0xFFFF, size >> 16); }

    uint src_dst;
    uint size;
};

#define PrecomputeIBLConstantsCount sizeof(PrecomputeIBLConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PrecomputeIBLConstants, precomputeIBLConstants, 0, 0);
#endif