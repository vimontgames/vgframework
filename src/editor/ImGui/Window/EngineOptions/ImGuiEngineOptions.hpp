#include "ImGuiEngineOptions.h"
#include "engine/IEngineOptions.h"
#include "engine/IEngine.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEngineOptions::ImGuiEngineOptions() :
        ImGuiWindow(style::icon::Engine, "", "Engine", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEngineOptions::DrawGUI()
    {
        auto * engineOptions = Editor::get()->getEngine()->GetOptions();
        if (engineOptions)
        {
            if (ImGui::IconBegin(style::icon::Engine, getName().c_str(), &m_isVisible))
                displayObject(engineOptions);

            ImGui::End();
        }
    }
}