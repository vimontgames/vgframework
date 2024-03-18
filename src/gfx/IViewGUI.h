#pragma once

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

    class IViewGUI
    {
    public:
        virtual ~IViewGUI() {};

        virtual void AddText(renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, HorizontalAligment _alignX, VerticalAligment _alignY, const core::string & _text) = 0;
        virtual void AddImage(renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, HorizontalAligment _alignX, VerticalAligment _alignY, const gfx::ITexture * _texture) = 0;

        virtual void RenderFullscreen() = 0;
        virtual void RenderWindowed() = 0;
    };
}