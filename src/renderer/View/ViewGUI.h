#pragma once

#include "gfx/IViewGUI.h"

namespace vg::renderer
{
    using PickingID = core::uint;

    enum class UIElementType : core::u8
    {
        Text = 0,
        Image
    };

    class ViewGUI : public gfx::IViewGUI
    {
    public:
        ViewGUI(gfx::IView * _view);
        ~ViewGUI();

        void    AddText             (renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, gfx::HorizontalAligment _alignX, gfx::VerticalAligment _alignY, const core::string & _text) final override;
        void    AddImage            (renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, gfx::HorizontalAligment _alignX, gfx::VerticalAligment _alignY, const gfx::ITexture * _texture) final override;

        void    RenderFullscreen    () final override;
        void    RenderWindowed      () final override;

    protected:
        void    render              ();

    private:
        struct UIElement
        {
            UIElement(PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, gfx::HorizontalAligment _alignX, gfx::VerticalAligment _alignY, const core::string & _text) :
                m_pickingID(_pickingID),
                m_type(UIElementType::Text),
                m_matrix(_matrix),
                m_color(_color),
                m_alignX(_alignX),
                m_alignY(_alignY),
                m_text(_text)
            {

            }

            UIElement(PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, gfx::HorizontalAligment _alignX, gfx::VerticalAligment _alignY, const gfx::ITexture * _texture) :
                m_pickingID(_pickingID),
                m_type(UIElementType::Image),
                m_matrix(_matrix),
                m_color(_color),
                m_alignX(_alignX),
                m_alignY(_alignY),
                m_texture(_texture)
            {

            }
            PickingID               m_pickingID = (PickingID)0;
            UIElementType           m_type = (UIElementType)-1;
            core::float4x4          m_matrix;
            core::float4            m_color;
            gfx::HorizontalAligment m_alignX = gfx::HorizontalAligment::Center;
            gfx::VerticalAligment   m_alignY = gfx::VerticalAligment::Center;
            core::string            m_text;
            const gfx::ITexture *   m_texture = nullptr;
        };

        gfx::IView * m_view = nullptr;
        core::vector<UIElement> m_uiElements;
    };
}