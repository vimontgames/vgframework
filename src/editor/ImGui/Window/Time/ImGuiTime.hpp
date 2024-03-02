#include "ImGuiTime.h"
#include "engine/IEngine.h"
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
            auto * mainWorld = engine->GetMainWorld();

            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                if (mainWorld)
                {
                    bool fixedDT = mainWorld->IsUsingFixedDeltaTime();
                    if (ImGui::Checkbox("Fixed", &fixedDT))
                        mainWorld->SetUseFixedDeltaTime(fixedDT);

                    float timeScale = mainWorld->GetTimeScale(true);  // TODO: time should be per world or global, but match playing/paused!
                  
                    if (ImGui::SliderFloat("TimeScale", &timeScale, 0.0f, 10.0, "%.3f", ImGuiSliderFlags_None))
                        mainWorld->SetTimeScale(timeScale);

                    ImGui::SameLine();

                    if (TooltipButton(style::icon::Reload, timeScale != 1.0f, true, "Reset", style::button::SizeSmall))
                        mainWorld->SetTimeScale(1.0);                 
              
                    ImGui::Separator();

                    ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                    {
                        ImGui::Text("Real DeltaTime");

                        ImGui::BeginDisabled(mainWorld->IsPaused() || !mainWorld->IsPlaying());
                        {
                            ImGui::Text("Scaled DeltaTime");
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::Text("%.3f ms", engine->GetTime().m_realDeltaTime * 1000.0f);

                        ImGui::BeginDisabled(mainWorld->IsPaused());
                        {
                            ImGui::Text("%.3f ms", engine->GetTime().m_scaledDeltaTime * 1000.0f);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::Columns(1);
                }
            }
            ImGui::EndChild();
        }
        End();
    }
}