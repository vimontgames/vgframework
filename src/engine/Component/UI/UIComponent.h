#pragma once

#include "core/Component/Component.h"

namespace vg::gfx
{
    enum class HorizontalAligment : core::u8;
    enum class VerticalAligment : core::u8;
    enum class UIItemFlags : core::u32;

    class IUIRenderer;
    struct UICanvas;
}

namespace vg::renderer
{
    using PickingID = core::uint;
}

namespace vg::engine
{
    class UIComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(UIComponent, core::Component);

        UIComponent(const core::string & _name, core::IObject * _parent);
        ~UIComponent();

        VG_INLINE core::float3      getOffset   () const;
        VG_INLINE core::float4x4    getMatrix   () const; 
        VG_INLINE core::float4      getColor    () const;

        gfx::IUIRenderer *             getGUI      (const core::IWorld * _world) const;
        const gfx::UICanvas *       getCanvas   () const;

    protected:
        renderer::PickingID         m_pickingID;

        gfx::UIItemFlags            m_UIFlags = (gfx::UIItemFlags)0x0;

        gfx::HorizontalAligment     m_horizontal;
        gfx::VerticalAligment       m_vertical;

        core::uint2                 m_size = core::uint2(16, 16);

        bool                        m_useOffset;
        core::float3                m_offset;

        bool                        m_useColor;
        core::float4                m_color;
    };
}

#if VG_ENABLE_INLINE
#include "UIComponent.inl"
#endif