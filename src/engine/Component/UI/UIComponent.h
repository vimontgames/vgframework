#pragma once

#include "engine/IUIComponent.h"
#include "gfx/IUIRenderer.h"

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
    class IUIManager;
    using PickingID = core::uint;
}

namespace vg::engine
{
    class UIComponent : public IUIComponent
    {
    public:
        VG_CLASS_DECL(UIComponent, IUIComponent);

        UIComponent(const core::string & _name, core::IObject * _parent);
        ~UIComponent();

        core::uint2                         GetSize         () const override;
        void                                SetSize         (const core::uint2 _size) override;

        VG_INLINE core::uint2               getSize         () const;
        VG_INLINE void                      setSize         (core::uint2 _size);

        VG_INLINE core::float3              getOffset       () const;
        VG_INLINE core::float4x4            getMatrix       () const; 
        VG_INLINE core::float4              getColor        () const;
        VG_INLINE renderer::IUIManager *    getUIManager    () const;
        VG_INLINE core::float2              getCenter       () const;

        const gfx::UICanvas *               getCanvas       () const;
        gfx::UIItem                         getUIItem       () const;

    private:
        renderer::IUIManager *              m_uiManager     = nullptr;

    protected:
        renderer::PickingID                 m_pickingID;
        gfx::UIItemFlags                    m_UIFlags = (gfx::UIItemFlags)0x0;
        gfx::HorizontalAligment             m_horizontal;
        gfx::VerticalAligment               m_vertical;
        core::uint2                         m_size = core::uint2(16, 16);
        bool                                m_useCenter = false;
        core::float2                        m_center = core::float2(0.5f, 0.5f);
        bool                                m_useOffset;
        core::float3                        m_offset;
        bool                                m_useColor;
        core::float4                        m_color;
    };
}

#if VG_ENABLE_INLINE
#include "UIComponent.inl"
#endif