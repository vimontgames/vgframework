#include "ImGuiMainToolbar.h"
#include "engine/IEngine.h"

namespace vg::renderer
{
    using namespace vg::engine;

    //--------------------------------------------------------------------------------------
    ImGuiMainToolbar::ImGuiMainToolbar(const core::string & _name, Flags _flags) :
        ImGuiToolbar(_name, _flags)
    {

    }

    //--------------------------------------------------------------------------------------
    void ImGuiMainToolbar::buttons()
    {
        IEngine * engine = ImguiEditor::getEngine();

        if (toolbarButton(Editor::Icon::Play, "Play", !engine->IsPlaying()))
        {
            engine->Play();
        }
        nextItem();

        if (toolbarButton(Editor::Icon::Pause, "Pause", engine->IsPlaying()))
        {
            if (engine->IsPaused())
                engine->Play();
            else
                engine->Pause();
        }
        nextItem();

        if (toolbarButton(Editor::Icon::Stop, "Stop", engine->IsPlaying()))
        {
            engine->Stop();
        }
        nextItem();
    }
}