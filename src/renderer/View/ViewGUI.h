#pragma once

#include "gfx/IViewGUI.h"

namespace vg::renderer
{
    enum class UIElementType : core::u8
    {
        Text = 0,
        Image
    };

    class ViewGUI : public gfx::IViewGUI
    {
    public:
        ViewGUI();
        ~ViewGUI();

        void    AddText             (const core::float4 & _rect, float _depth, const core::string & _text, core::float4 _color) final override;
        void    AddImage            (const core::float4 & _rect, float _depth, const gfx::ITexture * _texture, core::float4 _color) final override;

        void    RenderFullscreen    () final override;
        void    RenderWindowed      () final override;

    protected:
        void    render              ();

    private:
        struct UIElement
        {
            UIElement(const core::float4 & _rect, float _depth, const core::string & _text, core::float4 _color) :
                m_type(UIElementType::Text),
                m_rect(_rect),
                m_depth(_depth),
                m_text(_text),
                m_color(_color)
            {

            }

            UIElement(const core::float4 & _rect, float _depth, const gfx::ITexture * _texture, core::float4 _color) :
                m_type(UIElementType::Image),
                m_rect(_rect),
                m_depth(_depth),
                m_texture(_texture),
                m_color(_color)
            {

            }

            UIElementType           m_type = (UIElementType)-1;
            core::float4            m_rect = core::float4(0, 0, 0, 0);
            float                   m_depth = 0;
            core::string            m_text;
            const gfx::ITexture *   m_texture = nullptr;
            core::float4            m_color = core::float4(1,1,1,1);
        };

        core::vector<UIElement> m_uiElements;
    };
}