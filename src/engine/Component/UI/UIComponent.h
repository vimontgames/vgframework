#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class UIComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(UIComponent, core::Component);
        UIComponent(const core::string & _name, core::IObject * _parent);
        ~UIComponent();

        VG_INLINE core::float4 getRect() const { return m_useRect ? m_rect : core::float4(0,0,0,0); }
        VG_INLINE float getDepth() const { return m_useDepth ? m_depth : 0.0f; }
        VG_INLINE core::float4 getColor() const { return m_useColor ? m_color : core::float4(1, 1, 1, 1); }

    protected:
        bool            m_useRect = true;
        core::float4    m_rect = core::float4(0.0f, 0.0f, 1.0f, 1.0f);

        bool            m_useDepth = false;
        float           m_depth = 0.0f;

        bool            m_useColor = false;
        core::float4    m_color = core::float4(1, 1, 1, 1);
    };
}