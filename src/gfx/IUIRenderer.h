#pragma once

#include "gfx/IViewport.h"

// This is not ideal, maybe those values should be part of the interface or just passe opaque index type?
namespace vg::renderer
{
    struct UIElement;
    enum class Font : core::u8;
    enum class FontStyle : core::u8;
    using PickingID = core::uint;
}

namespace vg::gfx
{
    class ITexture;

    vg_enum_class( HorizontalAligment , core::u8,
        Left,
        Center,
        Right
    );

    vg_enum_class(VerticalAligment , core::u8,
        Top,
        Center,
        Bottom
    );

    vg_enum_class(UIItemFlags , core::u32,
        AutoResize      = 0x00000001,
        KeepAspectRatio = 0x00000002
    );    

    vg_enum_class(CanvasType, core::u8,
        CanvasType_2D = 0x00000002,
        CanvasType_3D = 0x00000003
    );

    struct UIItem
    {
        UIItem()
        {
        }

        UIItem(renderer::PickingID _pickingID, const core::float4x4 & _matrix, const core::float3 & _offset, const core::uint2 & _size, HorizontalAligment _alignX, VerticalAligment _alignY, const core::float4 & _color, UIItemFlags _flags) :
            m_pickingID(_pickingID),
            m_matrix(_matrix),
            m_offset(_offset),
            m_size(_size),
            m_alignX(_alignX),
            m_alignY(_alignY),
            m_color(_color),
            m_flags(_flags)
        {

        }

        renderer::PickingID     m_pickingID         = (renderer::PickingID)0;
        core::float4x4          m_matrix            = core::float4x4::identity();
        core::float3            m_offset            = core::float3(0,0,0);
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
        CanvasType              m_canvasType        = CanvasType::CanvasType_2D;
        bool                    m_useViewMask       = true;
        gfx::ViewMask           m_viewMask          = gfx::ViewMask_All;
        core::uint2             m_resolution        = core::uint2(1280, 720);
    };

    class IUIRenderer
    {
    public:
        virtual ~IUIRenderer() {};

        virtual void Add                (const renderer::UIElement & _desc) = 0;

        virtual void Clear              () = 0;

        virtual void RenderFullscreen   () = 0;
        virtual void RenderWindowed     () = 0;
    };
}