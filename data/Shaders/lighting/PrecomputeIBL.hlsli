#pragma once

#define PRECOMPUTE_IBL_THREADGROUP_SIZE_X 16
#define PRECOMPUTE_IBL_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PrecomputeIBLConstants
{
    #ifdef __cplusplus
    PrecomputeIBLConstants()
    {
        textureUAV = 0x0;
    }
    #endif

    void setSpecularBRDF    (uint _rwtex2D)     { textureUAV = packUint16low(_rwtex2D, _rwtex2D); }
    uint getSpecularBRDF    ()                  { return unpackUint16low(textureUAV); }

    void setSize            (uint2 _size)       { size = _size.x | (_size.y << 16); }
    uint2 getSize           ()                  { return uint2(size & 0xFFFF, size >> 16); }

    uint textureUAV;
    uint size;
};

#define PrecomputeIBLConstantsCount sizeof(PrecomputeIBLConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PrecomputeIBLConstants, precomputeIBLConstants, 0, 0);
#endif