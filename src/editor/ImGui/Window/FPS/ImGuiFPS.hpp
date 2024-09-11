#include "ImguiFPS.h"
#include "editor/Editor_Consts.h"

#pragma optimize("", off)

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiFPS::ImGuiFPS() :
        ImGuiWindow(style::icon::FPS, "", "FPS", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiFPS::DrawGUI()
    {
        const auto & time = Editor::get()->getEngine()->GetTime();

        static const uint smoothDtTime = 1; // 1.0s

        bool captureInProgress = VG_PROFILE_CAPTURE_IN_PROGRESS();

        if (ImGui::IconBegin(getIcon().c_str(), "FPS", &m_isVisible))
        {
            if (ImGui::TooltipButton(fmt::sprintf("%s Start Profiling", style::icon::FPS).c_str(), !captureInProgress, !captureInProgress, "Start Profiling (F1)", style::button::SizeLarge))
                VG_PROFILE_START();

            ImGui::SameLine();

            if (ImGui::TooltipButton(fmt::sprintf("%s Stop Profiling", style::icon::FPS).c_str(), captureInProgress, captureInProgress, "Stop Profiling (F1)", style::button::SizeLarge))
                VG_PROFILE_STOP();

            ImGui::Separator();

            const auto fps = time.smoothed.m_fps;
            const auto dt = time.smoothed.m_dt;
            const auto cpuMs = dt * 1000.0f - time.smoothed.m_gpuWait;
            const auto gpuMs = time.smoothed.m_gpu;

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                {
                    ImGui::Text("FPS");
                    ImGui::Text("Frame");
                    ImGui::Text("CPU");
                    ImGui::Text("GPU");
                }
                ImGui::NextColumn();
                {
                    ImGui::Text("%.0f fps", fps);
                    ImGui::Text("%.2f ms", dt * 1000.0f); 
                    ImGui::Text("%.2f ms", cpuMs); 
                    ImGui::Text("%.2f ms", gpuMs);
                }
                ImGui::Columns(1);

                if (VG_PROFILE_CAPTURE_IN_PROGRESS())
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Profiling Frame %u", m_captureFrameCounter);
                    m_captureFrameCounter++;
                }
                else if (0 != m_captureFrameCounter)
                {
                    m_captureFrameCounter = 0;
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}