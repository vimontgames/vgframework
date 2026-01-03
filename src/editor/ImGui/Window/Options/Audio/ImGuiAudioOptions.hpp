#include "ImGuiAudioOptions.h"
#include "audio/IAudio.h"
#include "audio/IAudioOptions.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiAudioOptions::ImGuiAudioOptions() :
        super(style::icon::Sound, "", "Audio", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiAudioOptions::GetOptions() const
    {
        return Editor::get()->getEngine()->GetAudio()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAudioOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}