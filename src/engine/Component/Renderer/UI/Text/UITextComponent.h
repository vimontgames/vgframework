#pragma once
#include "engine/IUITextComponent.h"
#include "renderer/ImGui_consts.h"

namespace vg::engine
{
    class UITextComponent : public IUITextComponent
    {
    public:
        VG_CLASS_DECL(UITextComponent, IUITextComponent);
        UITextComponent(const core::string & _name, core::IObject * _parent);
        ~UITextComponent();

        void    SetText     (const core::string & _text) final override;
        void    Update      (const Context & _context) final override;

    private:
        renderer::Font      m_font = renderer::Font::UbuntuMono;
        renderer::FontStyle m_style = renderer::FontStyle::Regular;
        renderer::FontSize  m_fontSize = (renderer::FontSize)14.0f;
        core::string        m_text;
    };
}