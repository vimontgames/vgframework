#include "ImGuiStatistics.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiStatistics::ImGuiStatistics() :
        ImGuiWindow(style::icon::Statistics, "", "Statistics", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiStatistics::DrawGUI()
    {
        if (ImGui::IconBegin(getIcon().c_str(), "Statistics", &m_isVisible))
        {
            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                auto renderer = Editor::get()->getRenderer();

                if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::TreeNodeEx("Views", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        for (uint i = 0; i < enumCount<gfx::ViewTarget>(); ++i)
                        {
                            const auto target = (gfx::ViewTarget)i;
                            const auto & views = renderer->GetViews(target);

                            if (views.size() > 0)
                            {
                                //if (ImGui::TreeNodeEx(asString(target).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    for (uint j = 0; j < views.size(); ++j)
                                    {
                                        const auto & view = views[j];
                                        if (nullptr != view)
                                        {
                                            string viewLabel = fmt::sprintf("%s (%s, %s)", view->getName(), view->IsActive() ? "Active" : "Not Active", view->IsVisible() ? "Visible" : "Not Visible");

                                            if (ImGui::TreeNodeEx(viewLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
                                            {
                                                ImGui::TreePop();
                                            }
                                        }
                                    }

                                    //ImGui::TreePop();
                                }                            
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}