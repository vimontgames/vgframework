#pragma once

#define POSTPROCESS_THREADGROUP_SIZE_X 16
#define POSTPROCESS_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PostProcessConstants
{
    #ifdef __cplusplus
    PostProcessConstants()
    {
        m_data = (uint4)0;

        setScreenSize(uint2(0,0));
        setColor(BINDLESS_TEXTURE_INVALID);
        setRWBufferOut(BINDLESS_RWTEXTURE_INVALID);
        setDepth(BINDLESS_TEXTURE_INVALID);
        setStencil(BINDLESS_TEXTURE_INVALID);
        setLinearDepth(BINDLESS_TEXTURE_INVALID);
    }
    #endif

    void setScreenSize  (uint2 _size)   { m_data.x = packUint16(_size.xy);}
    uint2 getScreenSize ()              { return unpackUint16(m_data.x); }

    void setColor       (uint _color)   { m_data.y = packUint16low(m_data.y, _color); }
    uint getColor       ()              { return unpackUint16low(m_data.y); }

    void setRWBufferOut (uint _rwbuffer){ m_data.y = packUint16high(m_data.y, _rwbuffer); }
    uint getRWBufferOut ()              { return unpackUint16high(m_data.y); }

    void setDepth       (uint _depth)   { m_data.z = packUint16low(m_data.z, _depth); }
    uint getDepth       ()              { return unpackUint16low(m_data.z); }

    void setStencil     (uint _stencil) { m_data.z = packUint16high(m_data.z, _stencil); }
    uint getStencil     ()              { return unpackUint16high(m_data.z); }

    void setLinearDepth (uint _linearZ) { m_data.w = packUint16high(m_data.w, _linearZ); }
    uint getLinearDepth ()              { return unpackUint16high(m_data.w); }

    uint4 m_data;
};

#define PostProcessConstantsCount sizeof(PostProcessConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PostProcessConstants, postProcessConstants, 0, 0);
#endif