#include "editor/Precomp.h"
#include "ImGuiOptionsWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/ImGuiEditorOptions.hpp"
#include "engine/ImGuiEngineOptions.hpp"
#include "physics/ImGuiPhysicsOptions.hpp"
#include "renderer/ImGuiRendererOptions.hpp"

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
            if (ImGui::IconBegin(getIcon().c_str(), getName().c_str(), &m_isVisible))
            {
                if (ImGui::TooltipButton(editor::style::icon::Reload, true, true, "Reload"))
                    GetOptions()->Load();

                ImGui::SameLine();

                if (ImGui::TooltipButton(editor::style::icon::Save, true, true, "Save"))
                    GetOptions()->Save();

                ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
                {
                    ObjectContext context;
                    displayObject(options, context);
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }
    }
}