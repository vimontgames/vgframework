#include "ImGuiTime.h"
#include "engine/IEngine.h"
#include "engine/IEngineOptions.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiTime::ImGuiTime() :
        ImGuiWindow(style::icon::Clock, "", "Time", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiTime::DrawGUI()
    {
        if (IconBegin(getIcon().c_str(), getName().c_str(), &m_isVisible))
        {
            auto * engine = Editor::get()->getEngine();

            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                engine::IEngineOptions * engineOptions = engine->GetOptions();

                ImGui::BeginDisabled(true);

                float fixedDT;
                bool useFixedDT = engineOptions->TryGetFixedDT(fixedDT);
                ImGui::Checkbox("Fixed", &useFixedDT);

                float timeScale = 1.0f;
                bool customTimescale = engineOptions->TryGetTimeScale(timeScale); 
                  
                ImGui::SliderFloat("TimeScale", &timeScale, 0.0f, 16.0, "%.3f", ImGuiSliderFlags_None);
               
                ImGui::Separator();

                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                {
                    ImGui::Text("Real DeltaTime");
                    ImGui::Text("Scaled DeltaTime");                    
                }
                ImGui::NextColumn();
                {
                    ImGui::Text("%.3f ms", engine->GetTime().m_realDeltaTime * 1000.0f);
                    ImGui::Text("%.3f ms", engine->GetTime().m_scaledDeltaTime * 1000.0f);
                }
                ImGui::Columns(1);

                ImGui::EndDisabled();
            }
            ImGui::EndChild();
        }
        End();
    }
}