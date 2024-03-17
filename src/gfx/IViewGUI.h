#pragma once

namespace vg::gfx
{
    class ITexture;

    class IViewGUI
    {
    public:
        virtual ~IViewGUI() {};

        virtual void AddText(const core::float4 & _rect, float _depth, const core::string & _text, core::float4 _color) = 0;
        virtual void AddImage(const core::float4 & _rect, float _depth, const gfx::ITexture * _texture, core::float4 _color) = 0;

        virtual void RenderFullscreen() = 0;
        virtual void RenderWindowed() = 0;
    };
}