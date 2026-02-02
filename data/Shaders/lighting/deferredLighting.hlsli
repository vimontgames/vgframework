#pragma once

#define DEFERRED_LIGHTING_THREADGROUP_SIZE_X 16
#define DEFERRED_LIGHTING_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct DeferredLightingConstants
{
    void setScreenSize              (uint2 _size)   { width_height = packUint16(_size.xy);}
    uint2 getScreenSize             ()              { return unpackUint16(width_height); }

    void setAlbedoGBuffer           (uint _albedo)  { albedo_normal = packUint16low(albedo_normal, _albedo); }
    uint getAlbedoGBuffer           ()              { return unpackUint16low(albedo_normal); }

    void setNormalGBuffer           (uint _normal)  { albedo_normal = packUint16high(albedo_normal, _normal); }
    uint getNormalGBuffer           ()              { return unpackUint16high(albedo_normal); }

    void setPBRGBuffer              (uint _pbr)     { pbr_emissive = packUint16low(pbr_emissive, _pbr); }
    uint getPBRGBuffer              ()              { return unpackUint16low(pbr_emissive); }
    
    void setEmissiveGBuffer         (uint _emissive){ pbr_emissive = packUint16high(pbr_emissive, _emissive); }
    uint getEmissiveGBuffer         ()              { return unpackUint16high(pbr_emissive); }

    void setDepth                   (uint _depth)   { depth_stencil = packUint16low(depth_stencil, _depth); }
    uint getDepth                   ()              { return unpackUint16low(depth_stencil); }

    void setStencil                 (uint _stencil) { depth_stencil = packUint16high(depth_stencil, _stencil); }
    uint getStencil                 ()              { return unpackUint16high(depth_stencil); }

    void setRWBufferOut             (uint _rwbuffer){ rwbuffer_ssa = packUint16low(rwbuffer_ssa, _rwbuffer); }
    uint getRWBufferOut             ()              { return unpackUint16low(rwbuffer_ssa); }
    
    void setScreenSpaceAmbient      (uint _ssa)     { rwbuffer_ssa = packUint16high(rwbuffer_ssa, _ssa); }
    uint getScreenSpaceAmbient      ()              { return unpackUint16high(rwbuffer_ssa); }

    uint width_height;
    uint albedo_normal;
    uint pbr_emissive;
    uint depth_stencil;
    uint rwbuffer_ssa;
};

#define DeferredLightingConstantsCount sizeof(DeferredLightingConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(DeferredLightingConstants, deferredLightingConstants, 0, 0);
#endif