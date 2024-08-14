#include "UITextComponent.h"
#include "editor/Editor_Consts.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "core/IGameObject.h"
#include "gfx/IViewGUI.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UITextComponent, "UIText", "UI", "UI Text Element", editor::style::icon::Font, 2);

    //--------------------------------------------------------------------------------------
    bool UITextComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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
        if (auto * gui = getViewGUI(_context.m_world))
            gui->AddText(getCanvas(), gfx::UIItem(m_pickingID, getMatrix(), m_size, m_horizontal, m_vertical, getColor(), m_UIFlags), m_text);
    }
}