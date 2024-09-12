#pragma once

#include "gfx/IViewGUI.h"

namespace vg::renderer
{
    using PickingID = core::uint;

    enum class Font : core::u8;
    enum class Style : core::u8;

    enum class UIElementType : core::u8
    {
        Canvas = 0,
        Text,
        Image
    };

    class ViewGUI : public gfx::IViewGUI
    {
    public:
        ViewGUI(gfx::IViewport * _viewport, gfx::IView * _view);
        ~ViewGUI();

        void            AddCanvas           (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc) final override;
        void            AddText             (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const core::string & _text, Font _font, Style _style) final override;
        void            AddImage            (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const gfx::ITexture * _texture) final override;

        void            RenderFullscreen    () final override;
        void            RenderWindowed      () final override;

        void            Clear               () final override;

    protected:
        void            render              ();
        core::uint2     getSize             () const;
        core::float2    getScale            () const;
        core::float2    getOffset           () const;
        
    private:
        struct UIElement
        {
            UIElement(const gfx::UICanvas * _canvas, const gfx::UIItem & _elemDesc) :
                m_type(UIElementType::Canvas),
                m_canvas(_canvas),
                m_item(_elemDesc)
            {

            }

            UIElement(const gfx::UICanvas * _canvas, const gfx::UIItem & _elemDesc, const core::string & _text, Font _font, Style _style) :
                m_type(UIElementType::Text),
                m_canvas(_canvas),
                m_item(_elemDesc),
                m_text(_text),
                m_font(_font),
                m_style(_style)
            {

            }

            UIElement(const gfx::UICanvas * _canvas, const gfx::UIItem & _elemDesc, const gfx::ITexture * _texture) :
                m_type(UIElementType::Image),
                m_canvas(_canvas),
                m_item(_elemDesc),
                m_texture((gfx::ITexture *)_texture)
            {

            }

            const UIElement & operator =(const UIElement & _other)
            {
                m_canvas = _other.m_canvas; 
                m_item = _other.m_item;
                m_type = _other.m_type;
                m_text = _other.m_text;
                m_texture = _other.m_texture;
                return *this;
            }

            const gfx::UICanvas *   m_canvas = nullptr;
            UIElementType           m_type = (UIElementType)-1;
            gfx::UIItem             m_item;   

            // Text
            core::string            m_text;
            Font                    m_font = (Font)0;
            Style                   m_style = (Style)0;

            // Texture
            gfx::ITexture *         m_texture = nullptr;
        };

        gfx::IViewport *            m_viewport = nullptr;
        gfx::IView *                m_view = nullptr;
        core::vector<UIElement>     m_uiElements;
    };
}