#include "ImguiFPS.h"
#include "editor/Editor_Consts.h"

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
        static const uint smoothDtTime = 1; // 1.0s

        m_accum += Editor::get()->getEngine()->GetTime().m_realDeltaTime;
        m_frame++;

        if (m_accum > (float)smoothDtTime)
        {
            m_dt = (float)(m_accum / (float)m_frame);
            m_fps = (float)1.0f / m_dt;
            m_accum = 0.0;
            m_frame = 0;
        }

        bool captureInProgress = VG_PROFILE_CAPTURE_IN_PROGRESS();

        if (ImGui::IconBegin(getIcon().c_str(), "FPS", &m_isVisible))
        {
            if (ImGui::TooltipButton(fmt::sprintf("%s Start Profiling", style::icon::FPS).c_str(), !captureInProgress, !captureInProgress, "Start Profiling (F1)", style::button::SizeLarge))
                VG_PROFILE_START();

            ImGui::SameLine();

            if (ImGui::TooltipButton(fmt::sprintf("%s Stop Profiling", style::icon::FPS).c_str(), captureInProgress, captureInProgress, "Stop Profiling (F1)", style::button::SizeLarge))
                VG_PROFILE_STOP();

            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                {
                    ImGui::Text("FPS");
                    ImGui::Text("Frame Time");
                }
                ImGui::NextColumn();
                {
                    ImGui::Text("%.0f img/sec", m_fps);
                    ImGui::Text("%.3f ms", m_dt * 1000.0f);                
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