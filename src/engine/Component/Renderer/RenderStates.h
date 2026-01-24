#pragma once

#include "core/Object/Object.h"
#include "core/Misc/BitMask/BitMask.h"
#include "gfx/PipelineState/Graphic/DepthStencilState.h"

namespace vg::engine
{
    class StencilState : public core::Object
    {
    public:
        VG_CLASS_DECL(StencilState, core::Object);

        StencilState(const vg::core::string & _name, vg::core::IObject * _parent);
        ~StencilState();        

        VG_INLINE bool isEnabled() const;
        VG_INLINE core::u8 getGfxStencilRef() const;
        VG_INLINE gfx::StencilState getGfxStencilState() const;

    private:
        bool                m_enable       = false;
        core::BitMask       m_ref          = 0x0;
        gfx::ComparisonFunc m_func         = gfx::ComparisonFunc::Always;
        gfx::StencilOp      m_passOp       = gfx::StencilOp::Keep;
        gfx::StencilOp      m_failOp       = gfx::StencilOp::Keep;
        gfx::StencilOp      m_depthFailOp  = gfx::StencilOp::Keep;
    };
}

#if VG_ENABLE_INLINE
#include "RenderStates.inl"
#endif