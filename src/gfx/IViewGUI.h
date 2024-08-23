#pragma once

#include "gfx/IViewport.h"

namespace vg::renderer
{
    using PickingID = core::uint;
}

namespace vg::gfx
{
    class ITexture;

    enum class HorizontalAligment : core::u8
    {
        Left,
        Center,
        Right
    };

    enum class VerticalAligment : core::u8
    {
        Top,
        Center,
        Bottom
    };

    enum class UIItemFlags : core::u32
    {
        AutoResize      = 0x00000001,
        KeepAspectRatio = 0x00000002
    };    

    struct UIItem
    {
        UIItem()
        {
        }

        UIItem(renderer::PickingID _pickingID, const core::float4x4 & _matrix, const core::uint2 & _size, HorizontalAligment _alignX, VerticalAligment _alignY, const core::float4 & _color, UIItemFlags _flags) :
            m_pickingID(_pickingID),
            m_matrix(_matrix),
            m_size(_size),
            m_alignX(_alignX),
            m_alignY(_alignY),
            m_color(_color),
            m_flags(_flags)
        {

        }

        renderer::PickingID     m_pickingID         = (renderer::PickingID)0;
        core::float4x4          m_matrix            = core::float4x4::identity();
        core::uint2             m_size              = core::uint2(16, 16);
        HorizontalAligment      m_alignX            = HorizontalAligment::Left;
        VerticalAligment        m_alignY            = VerticalAligment::Top;
        core::float4            m_color             = core::float4(1,1,1,1);
        UIItemFlags             m_flags             = (UIItemFlags)0x0;
    };

    struct UICanvas : public UIItem
    {
        UICanvas()
        {
        }

        ViewportTarget          m_viewportTarget    = ViewportTarget::Game;
        ViewportIndex           m_viewportIndex     = (ViewportIndex)0;
        core::u8                m_viewIndex         = 0;
        core::uint2             m_resolution        = core::uint2(1280, 720);
    };

    class IViewGUI
    {
    public:
        virtual ~IViewGUI() {};

        virtual void AddCanvas          (const UICanvas * _canvas, const UIItem & _desc) = 0;
        virtual void AddText            (const UICanvas * _canvas, const UIItem & _desc, const core::string & _text) = 0;
        virtual void AddImage           (const UICanvas * _canvas, const UIItem & _desc, const gfx::ITexture * _texture) = 0;

        virtual void RenderFullscreen   () = 0;
        virtual void RenderWindowed     () = 0;
    };
}