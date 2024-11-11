#pragma once

#define PRECOMPUTE_IBL_THREADGROUP_SIZE_X 8
#define PRECOMPUTE_IBL_THREADGROUP_SIZE_Y 8
#define PRECOMPUTE_IBL_THREADGROUP_SIZE_Z 1

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PrecomputeIBLConstants
{
    #ifdef __cplusplus
    PrecomputeIBLConstants()
    {
        handle = 0;
        mips = 0;
    }
    #endif

    void setSource                      (uint _tex2D)       { handle = packUint16low(handle, _tex2D); }
    uint getSource                      ()                  { return unpackUint16low(handle); }

    void setSourceMipLevel              (uint _mipLevel)    { mips = packR8(mips, _mipLevel); }
    uint getSourceMipLevel              ()                  { return unpackR8(mips); }    
            
    void setSourceMipLevelCount         (uint _count)       { mips = packG8(mips, _count); }
    uint getSourceMipLevelCount         ()                  { return unpackG8(mips); }  

    void setDestination                 (uint _rwtex2D)     { handle = packUint16high(handle, _rwtex2D); }
    uint getDestination                 ()                  { return unpackUint16high(handle); }

    void setDestinationMipLevel         (uint _mipLevel)    { mips = packB8(mips, _mipLevel); }
    uint getDestinationMipLevel         ()                  { return unpackB8(mips); }    

    void setDestinationMipLevelCount    (uint _count)       { mips = packA8(mips, _count); }
    uint getDestinationMipLevelCount    ()                  { return unpackA8(mips); }  

    uint handle;
    uint mips;
};

#define PrecomputeIBLConstantsCount sizeof(PrecomputeIBLConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PrecomputeIBLConstants, precomputeIBLConstants, 0, 0);
#endif