#pragma once

#define SKINNING_THREADGROUP_SIZE_X 16
#define SKINNING_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"
#include "system/vertex.hlsli"

struct ComputeSkinningConstants
{
    uint vtxfmt_flags;
    uint vtx_count;
    uint src_handle_unused;
    uint src_offset;
    uint dst_handle_unused;
    uint dst_offset;
    
    void            setVertexFormat (VertexFormat _format, uint _flags = 0) { vtxfmt_flags = packUint16(uint2((uint)_format, _flags)); }
    VertexFormat    getVertexFormat ()                                      { return (VertexFormat)((uint)(unpackUint16(vtxfmt_flags).x)); }
    
    void            setVertexCount  (uint _vertexCount)                     { vtx_count = _vertexCount; }
    uint            getVertexCount  ()                                      { return vtx_count; }
    
    void            setSource       (uint _handle, uint _offset)            { src_handle_unused = packUint16(uint2(_handle, 0)); src_offset = _offset;}
    
    uint            getSourceHandle ()                                      { return unpackUint16(src_handle_unused).x; }
    uint            getSourceOffset ()                                      { return src_offset; /*unpackUint16(src_handle_unused).y;*/ }
    
    void            setDest         (uint _handle, uint _offset)            { dst_handle_unused = packUint16(uint2(_handle, 0)); dst_offset = _offset;}
    
    uint            getDestHandle   ()                                      { return unpackUint16(dst_handle_unused).x; }
    uint            getDestOffset   ()                                      { return dst_offset; /*unpackUint16(dst_handle_offset).y;*/ }
};

#define ComputeSkinningConstantsCount sizeof(ComputeSkinningConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(ComputeSkinningConstants, computeSkinningConstants, 0, 0);
#endif