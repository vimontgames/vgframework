#include "engine/Precomp.h"
#include "HUDComponent.h"
#include "core/IGameObject.h"
#include "engine/IUITextComponent.h"
#include "core/string/string.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "renderer/IRenderer.h"
#include "renderer/IRendererOptions.h"

using namespace vg::core;

#include "editor/Editor_Consts.h"

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(HUDComponent, "HUD", "Debug", "Display FPS in HUD", editor::style::icon::Script, 1);

    //--------------------------------------------------------------------------------------
    bool HUDComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // FPS
        registerProperty(HUDComponent, m_fpsText, "FPS");
        registerProperty(HUDComponent, m_frameText, "Frame");
        registerProperty(HUDComponent, m_cpuText, "CPU");
        registerProperty(HUDComponent, m_gpuText, "GPU");

        // Memory
        registerProperty(HUDComponent, m_ramText, "RAM");
        registerProperty(HUDComponent, m_vramText, "VRAM");        

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

        // TODO: get screen frequency and display fps/ms in red when over budget
        //const auto * renderer = Engine::get()->GetRenderer();
        //const auto & rendererOptions = renderer->GetOptions();
        //const float targetFPS = rendererOptions->

        // FPS
        if (auto * gameobject = m_fpsText.get<IGameObject>())
        {
            if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
            {
                uiTextComponent->SetText(fmt::sprintf("%.0f FPS", time.smoothed.m_fps));
                //uiTextComponent->SetColor(float4(1, 0, 0, 1));
            }
        }
        else
        {
            VG_WARNING("[HUD] Could not find m_fpsText");
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

        // Memory
        const auto * options = EngineOptions::get();
        if (nullptr != options)
        {
            const CPUMemoryInfo & cpuMem = options->GetCpuMemoryInfo();
            const GPUMemoryInfo & gpuMem = options->GetGpuMemoryInfo();
        
            if (auto * gameobject = m_ramText.get<IGameObject>())
            {
                if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                    uiTextComponent->SetText(fmt::sprintf("%.2f GB", (float)cpuMem.m_usedPhys/1000.0f));
            }
        
            if (auto * gameobject = m_vramText.get<IGameObject>())
            {
                if (auto * uiTextComponent = gameobject->GetComponentT<IUITextComponent>())
                    uiTextComponent->SetText(fmt::sprintf("%.2f GB", (float)gpuMem.m_currentUsageLocal/1000.0f)); 
            }
        }
    }
}