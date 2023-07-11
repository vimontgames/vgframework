#include "ImguiPlatform.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPlatform::display()
    {
        if (ImGui::IconBegin(Editor::Icon::Platform, "Platform", &m_isVisible))
        {
            ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

            ImGui::Text("Platform:");
            ImGui::Text("Configuration:");
            ImGui::Text("Graphics API:");
            ImGui::NextColumn();
            ImGui::Text(Plugin::getPlatform().c_str());
            ImGui::Text(Plugin::getConfiguration().c_str());
            ImGui::Text(asString(Device::get()->getDeviceParams().api).c_str());
        }
        ImGui::End();
    }
}