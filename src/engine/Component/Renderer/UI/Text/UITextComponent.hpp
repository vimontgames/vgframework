#include "UITextComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UITextComponent, "UIText", "Renderer", "UI Text Element", editor::style::icon::Font, 2);

    //--------------------------------------------------------------------------------------
    bool UITextComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerPropertyEnumEx(UITextComponent, renderer::Font, m_font, "Font", PropertyFlags::AlphabeticalOrder);
        registerPropertyEnum(UITextComponent, renderer::Font, m_font, "Font");
        registerPropertyEnum(UITextComponent, renderer::FontStyle, m_style, "Font Style");
        registerProperty(UITextComponent, m_fontSize, "Font Size");
        setPropertyRange(UITextComponent, m_fontSize, uint2(5, 64));
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
        getUIManager()->AddText(getCanvas(), getUIItem(), m_text, m_font, m_style, m_fontSize, _context.m_world);
    }
}