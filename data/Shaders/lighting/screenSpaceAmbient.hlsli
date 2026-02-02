#pragma once

#define SCREEN_SPACE_AMBIENT_OCCLUSION_THREADGROUP_SIZE_X 16
#define SCREEN_SPACE_AMBIENT_OCCLUSION_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct ScreenSpaceAmbientConstants
{
    void setScreenSize      (uint2 _size)   { width_height = packUint16(_size.xy);}
    uint2 getScreenSize     ()              { return unpackUint16(width_height); }
    
    void setDepthBuffer     (uint _depth)   { depth_normal = packUint16low(depth_normal, _depth); }
    uint getDepthBuffer     ()              { return unpackUint16low(depth_normal); }
    
    void setNormalGBuffer   (uint _normal)  { depth_normal = packUint16high(depth_normal, _normal); }
    uint getNormalGBuffer   ()              { return unpackUint16high(depth_normal); }

    void setRWBufferOut     (uint _rwbuffer){ rwbuffer = packUint16low(rwbuffer, _rwbuffer); }
    uint getRWBufferOut     ()              { return unpackUint16low(rwbuffer); }

    uint width_height;
    uint depth_normal;
    uint rwbuffer;
};

#define ScreenSpaceAmbientConstantsCount sizeof(ScreenSpaceAmbientConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(ScreenSpaceAmbientConstants, screenSpaceAmbientConstants, 0, 0);
#endif