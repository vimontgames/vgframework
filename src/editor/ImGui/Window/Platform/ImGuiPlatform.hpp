#include "ImguiPlatform.h"
#include "core/Plugin/Plugin.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiPlatform::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Platform, "Platform", &m_isVisible))
        {
            ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

            const auto & deviceParams = Editor::get()->getRenderer()->GetDeviceCreationParams();

            ImGui::Text("Platform:");
            ImGui::Text("Configuration:");
            ImGui::Text("Graphics API:");
            ImGui::Text("Debug Device:");
            
            ImGui::NextColumn();
            
            ImGui::Text(Plugin::getPlatform().c_str());
            ImGui::Text(Plugin::getConfiguration().c_str());
            ImGui::Text(asString(deviceParams.api).c_str());
            ImGui::Text(deviceParams.debugDevice ? "True" : "False");
        }
        ImGui::End();
    }
}