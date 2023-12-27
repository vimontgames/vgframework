#pragma once

#define POSTPROCESS_THREADGROUP_SIZE_X 16
#define POSTPROCESS_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PostProcessConstants
{
    void setScreenSize  (uint2 _size)   { width_height = packUint16(_size.xy);}
    uint2 getScreenSize ()              { return unpackUint16(width_height); }

    void setColor       (uint _color)   { color_rwbuffer = packUint16low(color_rwbuffer, _color); }
    uint getColor       ()              { return unpackUint16low(color_rwbuffer); }

    void setRWBufferOut (uint _rwbuffer){ color_rwbuffer = packUint16high(color_rwbuffer, _rwbuffer); }
    uint getRWBufferOut ()              { return unpackUint16high(color_rwbuffer); }

    void setDepth       (uint _depth)   { depth_stencil = packUint16low(depth_stencil, _depth); }
    uint getDepth       ()              { return unpackUint16low(depth_stencil); }

    void setStencil     (uint _stencil) { depth_stencil = packUint16high(depth_stencil, _stencil); }
    uint getStencil     ()              { return unpackUint16high(depth_stencil); }

    uint width_height;
    uint color_rwbuffer;
    uint depth_stencil;
};

#define PostProcessConstantsCount sizeof(PostProcessConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PostProcessConstants, postProcessConstants, 0, 0);
#endif