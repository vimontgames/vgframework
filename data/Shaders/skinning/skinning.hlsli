#pragma once

#define SKINNING_THREADGROUP_SIZE_X 16
#define SKINNING_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct ComputeSkinningConstants
{
    uint src_handle_offset;
    uint dst_handle_offset;
    
    void setSource          (uint _handle, uint _offset)    { src_handle_offset = packUint16(uint2(_handle, _offset)); }
    
    uint getSourceHandle    ()                              { return unpackUint16(src_handle_offset).x; }
    uint getSourceOffset    ()                              {  return unpackUint16(src_handle_offset).y; }
    
    void setDest            (uint _handle, uint _offset)    { dst_handle_offset = packUint16(uint2(_handle, _offset)); }
    uint getDestHandle      ()                              { return unpackUint16(dst_handle_offset).x; }
    uint getDestOffset      ()                              { return unpackUint16(dst_handle_offset).y; }
};

#define ComputeSkinningConstantsCount sizeof(ComputeSkinningConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(ComputeSkinningConstants, computeSkinningConstants, 0, 0);
#endif