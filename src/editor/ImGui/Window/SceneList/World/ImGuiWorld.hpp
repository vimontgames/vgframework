#include "ImGuiWorld.h"
#include "editor/ImGui/ImGui.h"
#include "engine/IWorldResource.h"
#include "core/IGameObject.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiWorld::ImGuiWorld() :
        ImGuiSceneList(BaseSceneType::Scene, style::icon::World, "", "World", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
      
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWorld::DrawGUI()
    {
        display();
    }    
}