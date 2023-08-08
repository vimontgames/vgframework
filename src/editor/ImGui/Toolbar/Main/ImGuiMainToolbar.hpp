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

        if (ImGui::ButtonEx(style::icon::Play, !engine->IsPlaying(), "Play", getButtonSize()))
        {
            engine->Play();
        }
        nextItem();

        if (ImGui::ButtonEx(style::icon::Pause, engine->IsPlaying(), "Pause", getButtonSize()))
        {
            if (engine->IsPaused())
                engine->Play();
            else
                engine->Pause();
        }
        nextItem();

        if (ImGui::ButtonEx(style::icon::Stop, engine->IsPlaying(), "Stop", getButtonSize()))
        {
            engine->Stop();
        }
        nextItem();
    }
}