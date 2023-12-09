#include "ImGuiPhysicsOptions.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IPhysics.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPhysicsOptions::ImGuiPhysicsOptions() :
        ImGuiWindow(style::icon::Physics, "", "Physics", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPhysicsOptions::DrawGUI()
    {
        auto * physicsOptions = Editor::get()->getEngine()->GetPhysics()->GetPhysicsOptions();
        if (physicsOptions)
        {
            if (ImGui::IconBegin(style::icon::Physics, getName().c_str(), &m_isVisible))
                displayObject(physicsOptions);

            ImGui::End();
        }
    }
}