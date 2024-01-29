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
        ImGuiGameObjectTree(style::icon::Prefab, "", "Prefab", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPrefab::DrawGUI()
    {
        ImGui::PushID("ImGuiPrefab");
        {
            if (ImGui::IconBegin(getIcon().c_str(), fmt::sprintf("%s###Prefabs", "Prefab").c_str(), &m_isVisible))
            {
                if (ImGui::TooltipButton(editor::style::icon::New, true, true, "New Prefab"))
                {
                }

                ImGui::SameLine();

                if (ImGui::TooltipButton(editor::style::icon::Load, true, true, "Load Prefab"))
                {
                }

                ImGui::Separator();

                ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
                {
                }
                ImGui::EndChild();
            }
            ImGui::End();           
        }
        ImGui::PopID();
    }
}