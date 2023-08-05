#include "ImguiFPS.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiFPS::update(double _dt)
    {
        static const uint smoothDtTime = 1000; // 1.0s

        m_accum += _dt;
        m_frame++;

        if (m_accum > (double)smoothDtTime)
        {
            m_dt = (float)(m_accum / (float)m_frame);
            m_fps = (float)1000.0f / m_dt;
            m_accum = 0.0;
            m_frame = 0;
        }
    }

    //--------------------------------------------------------------------------------------
    void ImguiFPS::display()
    {
        if (ImGui::IconBegin(Editor::Icon::FPS, "FPS", &m_isVisible))
        {
            ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
            ImGui::Text("FPS: ");
            ImGui::Text("Frame: ");
            ImGui::NextColumn();
            ImGui::Text("%.0f img/sec", m_fps);
            ImGui::Text("%.4f ms", m_dt);

            ImGui::Columns(1);
            ImGui::Text("Press 'F1' to start/stop profiler");

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