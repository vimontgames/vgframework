#include "ImGuiMainToolbar.h"
#include "engine/IEngine.h"
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

        if (ImGui::ButtonEx(style::icon::Play, !playing, !playing, playing ? "Engine is Playing" : "Start Engine", getButtonSize()))
            engine->Play();

        nextItem();

        if (ImGui::ButtonEx(style::icon::Pause, playing && !paused, playing, playing ? (paused ? "Engine is Paused" : "Pause Engine") : "Engine is not Playing", getButtonSize()))
        {
            if (engine->IsPaused())
                engine->Play();
            else
                engine->Pause();
        }
        nextItem();

        if (ImGui::ButtonEx(style::icon::Stop, playing, playing, playing ? "Stop Engine" : "Engine is not Playing", getButtonSize()))
            engine->Stop();
        
        nextItem();
    }
}