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
    void UITextComponent::Update(float _dt)
    {
        if (IWorld * world = GetGameObject()->GetWorld())
        {
            auto * renderer = Engine::get()->GetRenderer();
            if (gfx::IView * view = renderer->GetView(gfx::ViewTarget::Game, world))
            {
                if (auto * gui = view->GetViewGUI())
                {
                    gui->AddText(getRect(), getDepth(), m_text, getColor());
                }
            }
        }
    }
}