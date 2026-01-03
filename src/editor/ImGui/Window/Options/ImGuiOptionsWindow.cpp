#include "editor/Precomp.h"
#include "ImGuiOptionsWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/ImGuiEditorOptions.hpp"
#include "engine/ImGuiEngineOptions.hpp"
#include "physics/ImGuiPhysicsOptions.hpp"
#include "renderer/ImGuiRendererOptions.hpp"
#include "audio/ImGuiAudioOptions.hpp"
#include "game/ImGuiGameOptions.hpp"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiOptionsWindow::ImGuiOptionsWindow(const core::string & _icon, const core::string & _path, const core::string & _name, ImGuiWindow::Flags _flags) :
        ImGuiWindow(_icon, _path, _name, _flags)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiOptionsWindow::DrawGUI()
    {
        auto * options = GetOptions();
        if (options)
        {
            if (ImGui::IconBegin(getIcon().c_str(), GetName().c_str(), &m_isVisible))
            {
                if (ImGui::TooltipButton(fmt::sprintf("%s Reload", editor::style::icon::Reload).c_str(), true, true, "Reload", style::button::SizeLarge))
                    GetOptions()->Load(true); 

                ImGui::SameLine();

                if (ImGui::TooltipButton(fmt::sprintf("%s Save", editor::style::icon::Save).c_str(), true, true, "Save", style::button::SizeLarge))
                    GetOptions()->Save();

                ImGui::Separator();

                ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
                {
                    ObjectContext objectContext;
                    displayObject(options, objectContext, nullptr);
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }
    }
}