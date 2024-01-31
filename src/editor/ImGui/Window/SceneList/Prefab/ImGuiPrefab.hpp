#include "ImGuiPrefab.h"
#include "editor/ImGui/ImGui.h"
#include "engine/IWorldResource.h"
#include "core/IGameObject.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPrefab::ImGuiPrefab() :
        ImGuiSceneList(SceneType::Prefab, style::icon::Prefab, "", "Prefab", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {

    }

    //--------------------------------------------------------------------------------------
    void ImGuiPrefab::DrawGUI()
    {
        display(SceneType::Prefab);
    }
}