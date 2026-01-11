#pragma once

#include "renderer/IView.h"
#include "renderer/IViewport.h"

namespace vg::gfx
{
    class ITexture;
}

namespace vg::renderer
{
    struct UIElement;
    enum class Font : core::u8;
    enum class FontStyle : core::u8;
    using PickingID = core::uint;

    vg_enum_class(vg::renderer,  HorizontalAligment, core::u8,
        Left,
        Center,
        Right
    );

    vg_enum_class(vg::renderer, VerticalAligment, core::u8,
        Top,
        Center,
        Bottom
    );

    vg_enum_class(vg::renderer, UIItemFlags, core::u32,
        AutoResize          = 0x00000001,   // Resize element to fit contents
        KeepAspectRatio     = 0x00000002,   // Preserve element aspect ratio
        ResolutionScalingX  = 0x00000004,   // Scale element according to horizontal screen resolution
        ResolutionScalingY  = 0x00000008    // Scale element according to vertical screen resolution
    );    

    vg_enum_class(vg::renderer, CanvasType, core::u8,
        CanvasType_2D = 0x00000002,
        CanvasType_3D = 0x00000003
    );

    struct UIItem
    {
        UIItem()
        {
        }

        UIItem(renderer::PickingID _pickingID, const core::float4x4 & _matrix, const core::float3 & _offset, const core::uint2 & _size, const core::float2 & _center, HorizontalAligment _alignX, VerticalAligment _alignY, const core::float4 & _color, UIItemFlags _flags) :
            m_pickingID(_pickingID),
            m_matrix(_matrix),
            m_offset(_offset),
            m_size(_size),
            m_center(_center),
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
        core::float2            m_center            = core::float2(0.5f, 0.5f);
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
        
        gfx::ViewportTarget     m_viewportTarget    = gfx::ViewportTarget::Game;
        gfx::ViewportIndex      m_viewportIndex     = (gfx::ViewportIndex)0;
        CanvasType              m_canvasType        = CanvasType::CanvasType_2D;
        bool                    m_useViewMask       = true;
        ViewMask                m_viewMask          = ViewMask_All;
        core::uint2             m_resolution        = core::uint2(1280, 720);
    };

    vg_enum_class(vg::renderer, RenderUIType, core::u8,
        RenderUIType_2D = 0,
        RenderUIType_3D
    );

    class IUIRenderer
    {
    public:
        virtual ~IUIRenderer() {};

        virtual void Add                (const UIElement & _desc) = 0;

        virtual void Clear              () = 0;

        virtual void RenderFullscreen   (RenderUIType _renderUIType) = 0;
        virtual void RenderWindowed     (RenderUIType _renderUIType) = 0;
    };
}