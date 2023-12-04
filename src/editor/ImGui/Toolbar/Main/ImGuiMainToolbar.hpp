#include "ImGuiMainToolbar.h"
#include "engine/IEngine.h"
#include "renderer/IRenderer.h"
#include "editor/ImGui/Extensions/ImGuiExtensions.h"

namespace vg::editor
{
    using namespace vg::engine;

    //--------------------------------------------------------------------------------------
    ImGuiMainToolbar::ImGuiMainToolbar(const core::string & _name, Flags _flags) :
        ImGuiToolbar(_name, _flags)
    {

    }

    //--------------------------------------------------------------------------------------
    void ImGuiMainToolbar::DrawButtons()
    {
        IEngine * engine = Editor::get()->getEngine();

        const bool playing = engine->IsPlaying();
        const bool paused = engine->IsPaused();

        if (ImGui::TooltipButton(style::icon::Play, !playing, !playing, "Play (F5)", getButtonSize()))
            engine->Play();

        nextItem();

        if (ImGui::TooltipButton(style::icon::Pause, playing && !paused, playing, "Pause (PAUSE)", getButtonSize()))
        {
            if (engine->IsPaused())
                engine->Resume();
            else
                engine->Pause();
        }
        nextItem();

        if (ImGui::TooltipButton(style::icon::Stop, playing, playing, "Stop (F5)", getButtonSize()))
            engine->Stop();
        
        nextItem();

        renderer::IRenderer * renderer = engine->GetRenderer();

        const bool fullscreen = renderer->IsFullscreen();
        if (ImGui::TooltipButton(style::icon::Maximize, true, true, "Fullscreen (F11)", getButtonSize()))
            renderer->SetFullscreen(!fullscreen);
    }
}