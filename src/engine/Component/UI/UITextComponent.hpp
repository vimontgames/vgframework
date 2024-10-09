#include "UITextComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UITextComponent, "UIText", "UI", "UI Text Element", editor::style::icon::Font, 2);

    //--------------------------------------------------------------------------------------
    bool UITextComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerPropertyEnumEx(UITextComponent, renderer::Font, m_font, "Font", PropertyFlags::AlphabeticalOrder);
        registerPropertyEnum(UITextComponent, renderer::Font, m_font, "Font");
        registerPropertyEnum(UITextComponent, renderer::FontStyle, m_style, "Style");
        registerProperty(UITextComponent, m_text, "Text");

        return true;
    }

    //--------------------------------------------------------------------------------------
    UITextComponent::UITextComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    UITextComponent::~UITextComponent()
    {

    }   

    //--------------------------------------------------------------------------------------
    void UITextComponent::SetText(const core::string & _text)
    {
        m_text = _text;
    }

    //--------------------------------------------------------------------------------------
    void UITextComponent::Update(const Context & _context)
    {
        getUIManager()->AddText(getCanvas(), getUIItem(), m_text, m_font, m_style, _context.m_world);
    }
}