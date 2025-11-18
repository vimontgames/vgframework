#ifndef _POSTPROCESS_HLSLI__H_
#define _POSTPROCESS_HLSLI__H_

#define POSTPROCESS_THREADGROUP_SIZE_X 16
#define POSTPROCESS_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

#if !_SHADER_COMPILER
// The following #defines here have no effect on compilation and are only used for syntax highlighting
#define _TOOLMODE 1
#define _RAYTRACING 1
#endif

struct PostProcessConstants
{
    #ifdef __cplusplus
    PostProcessConstants()
    {
        setScreenSize(uint2(0,0));
        setSource(BINDLESS_TEXTURE_INVALID);
        setRWBufferOut(BINDLESS_RWTEXTURE_INVALID);
        setDepth(BINDLESS_TEXTURE_INVALID);
        setStencil(BINDLESS_TEXTURE_INVALID);
        setLinearDepth(BINDLESS_TEXTURE_INVALID);
        setOutlineMask(BINDLESS_TEXTURE_INVALID);
    }
    #endif

    void setScreenSize          (uint2 _size)   { m_screensize = packUint16(_size.xy);}
    uint2 getScreenSize         ()              { return unpackUint16(m_screensize); }

    void setSource              (uint _color)   { m_src_dst = packUint16low(m_src_dst, _color); }
    uint getSource              ()              { return unpackUint16low(m_src_dst); }

    void setRWBufferOut         (uint _rwbuffer){ m_src_dst = packUint16high(m_src_dst, _rwbuffer); }
    uint getRWBufferOut         ()              { return unpackUint16high(m_src_dst); }

    void setDepth               (uint _depth)   { m_depth_stencil = packUint16low(m_depth_stencil, _depth); }
    uint getDepth               ()              { return unpackUint16low(m_depth_stencil); }

    void setStencil             (uint _stencil) { m_depth_stencil = packUint16high(m_depth_stencil, _stencil); }
    uint getStencil             ()              { return unpackUint16high(m_depth_stencil); }

    void setLinearDepth         (uint _linearZ) { m_depth_temp = packUint16low(m_depth_temp, _linearZ); }
    uint getLinearDepth         ()              { return unpackUint16low(m_depth_temp); }

    void setTemp                (uint _temp)    { m_depth_temp = packUint16high(m_depth_temp, _temp); }
    uint getTemp                ()              { return unpackUint16high(m_depth_temp); }

    void setOutlineMask         (uint _outline) { m_outline_unused = packUint16low(m_outline_unused, _outline); }
    uint getOutlineMask         ()              { return unpackUint16low(m_outline_unused); }
    
    uint m_screensize;
    uint m_src_dst;
    uint m_depth_stencil;
    uint m_depth_temp;
    uint m_outline_unused;
};

#define PostProcessConstantsCount sizeof(PostProcessConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PostProcessConstants, postProcessConstants, 0, 0);
#endif

#endif // _POSTPROCESS_HLSLI__H_