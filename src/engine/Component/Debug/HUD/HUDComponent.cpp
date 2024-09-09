#include "engine/Precomp.h"
#include "HUDComponent.h"
#include "core/IGameObject.h"
#include "engine/IUITextComponent.h"
#include "core/string/string.h"
#include "engine/Engine.h"

using namespace vg::core;

#include "editor/Editor_Consts.h"

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(HUDComponent, "HUD", "Debug", "Display FPS in HUD", editor::style::icon::Script, 1);

    //--------------------------------------------------------------------------------------
    bool HUDComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(HUDComponent, m_fpsText, "FPS text");

        return true;
    }

    //--------------------------------------------------------------------------------------
    HUDComponent::HUDComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    HUDComponent::~HUDComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void HUDComponent::Update(const Context & _context)
    {
        if (auto * gameobject = m_fpsText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
            {
                const auto time = Engine::get()->getTime();
                const auto fps = time.smoothed.m_fps;

                uiTextComponent->SetText(fmt::sprintf("%.0f FPS", fps));
            }
        }
    }
}