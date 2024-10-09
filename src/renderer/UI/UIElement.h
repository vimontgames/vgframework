#pragma once

#include "gfx/IUIRenderer.h"

namespace vg::gfx
{
    struct UICanvas;
    struct UIItem;
    class ITexture;
}

namespace vg::renderer
{
    using PickingID = core::uint;

    enum class Font : core::u8;
    enum class FontStyle : core::u8;

    enum class UIElementType : core::u8
    {
        Canvas = 0,
        Text,
        Image
    };

    struct UIElement
    {
        UIElement(const gfx::UICanvas * _canvas, const gfx::UIItem & _elemDesc) :
            m_type(UIElementType::Canvas),
            m_canvas(_canvas),
            m_item(_elemDesc)
        {

        }

        UIElement(const gfx::UICanvas * _canvas, const gfx::UIItem & _elemDesc, const core::string & _text, Font _font, FontStyle _style) :
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
            m_type = _other.m_type;
            m_item = _other.m_item;

            m_text = _other.m_text;
            m_font = _other.m_font;
            m_style = _other.m_style;

            m_texture = _other.m_texture;
            return *this;
        }

        const gfx::UICanvas *   m_canvas = nullptr;
        UIElementType           m_type = (UIElementType)-1;
        gfx::UIItem             m_item;

        // Text
        core::string            m_text;
        Font                    m_font = (Font)0;
        FontStyle               m_style = (FontStyle)0;

        // Texture
        gfx::ITexture *         m_texture = nullptr;
    };
}