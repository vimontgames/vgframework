#include "ImguiFPS.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiFPS::DrawGUI()
    {
        static const uint smoothDtTime = 1000; // 1.0s

        m_accum += Editor::get()->getEngine()->GetTime().m_dt;
        m_frame++;

        if (m_accum > (double)smoothDtTime)
        {
            m_dt = (float)(m_accum / (float)m_frame);
            m_fps = (float)1000.0f / m_dt;
            m_accum = 0.0;
            m_frame = 0;
        }

        bool captureInProgress = VG_PROFILE_CAPTURE_IN_PROGRESS();

        if (ImGui::IconBegin(style::icon::FPS, "FPS", &m_isVisible))
        {
            ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
            {
                ImGui::Text("FPS: ");
                ImGui::Text("Frame Time: ");
                ImGui::Text("Capture");
            }
            ImGui::NextColumn();
            {
                ImGui::Text("%.0f img/sec", m_fps);
                ImGui::Text("%.4f ms", m_dt);

                if (ImGui::ButtonEx("Start", !captureInProgress, "Press 'F1' to start capture"))
                    VG_PROFILE_START();
                ImGui::SameLine();
                if (ImGui::ButtonEx("Stop", captureInProgress, "Press 'F1' to stop capture"))
                    VG_PROFILE_START();
            }
            ImGui::Columns(1);

            if (VG_PROFILE_CAPTURE_IN_PROGRESS())
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Capture in progress ... %u", m_captureFrameCounter);
                m_captureFrameCounter++;
            }
            else if (0 != m_captureFrameCounter)
            {
                m_captureFrameCounter = 0;
            }
        }
        ImGui::End();
    }
}