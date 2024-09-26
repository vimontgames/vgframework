#include "ImGuiPlugin.h"
#include "core/IClassDesc.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiPlugin::ImGuiPlugin(const core::string & _name, Flags _flags) :
        ImGuiWindow(style::icon::Plugin, "", _name, _flags)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPlugin::DrawGUI()
    {
        if (ImGui::IconBegin(getIcon().c_str(), "Plugins", &m_isVisible))
        {
            const auto & factory = Editor::get()->getFactory();
            const auto & pluginDescs = factory->GetClassDescriptors(ClassDescFlags::Plugin);

            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("table_scrolly", 3, flags))
            {
                ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
                ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_None);
                ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_None);
                ImGui::TableHeadersRow();

                for (uint i = 0; i < pluginDescs.size(); ++i)
                {
                    const auto pluginDesc = pluginDescs[i];
                    if (nullptr == pluginDesc)
                        continue;

                    const IPlugin * plugin = static_cast<IPlugin *>(factory->GetSingleton(pluginDesc->GetClassName()));

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(pluginDesc->GetClassName());

                    ImGui::TableSetColumnIndex(1);
                    if (plugin != nullptr)
                        ImGui::Text((to_string(plugin->GetVersion().major) + "." + to_string(plugin->GetVersion().minor)).c_str());

                    ImGui::TableSetColumnIndex(2);
                    if (plugin != nullptr)
                        ImGui::Text(plugin->GetName().c_str());
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}