#include "ImGuiPhysicsOptions.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IPhysics.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPhysicsOptions::ImGuiPhysicsOptions() :
        super(style::icon::Physics, "", "Physics", ImGuiWindow::StartVisible)
    {
    }

    //--------------------------------------------------------------------------------------
    core::IOptions * ImGuiPhysicsOptions::GetOptions() const
    {
        return Editor::get()->getEngine()->GetPhysics()->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPhysicsOptions::DrawGUI()
    {
        super::DrawGUI();
    }
}