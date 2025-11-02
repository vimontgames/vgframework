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

        registerProperty(HUDComponent, m_fpsText, "FPS");
        registerProperty(HUDComponent, m_frameText, "Frame");
        registerProperty(HUDComponent, m_cpuText, "CPU");
        registerProperty(HUDComponent, m_gpuText, "GPU");

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
        const auto time = Engine::get()->getTime();

        if (auto * gameobject = m_fpsText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                uiTextComponent->SetText(fmt::sprintf("%.0f FPS", time.smoothed.m_fps));
        }

        if (auto * gameobject = m_frameText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                uiTextComponent->SetText(fmt::sprintf("%.2f ms", time.smoothed.m_dt * 1000.0f));
        }

        if (auto * gameobject = m_cpuText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                uiTextComponent->SetText(fmt::sprintf("%.2f ms", time.smoothed.m_dt * 1000.0f - time.smoothed.m_gpuWait));
        }

        if (auto * gameobject = m_gpuText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                uiTextComponent->SetText(fmt::sprintf("%.2f ms", time.smoothed.m_gpu));
        }
    }
}