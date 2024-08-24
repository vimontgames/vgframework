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
        auto drawViewDetail = [=](IView * _view)
        {
            if (ImGui::TreeNodeEx(_view->getName().c_str(), ImGuiTreeNodeFlags_None))
            {
                if (auto * rt = _view->GetRenderTarget())
                {
                    ImGui::Columns(2, "mycolumns2", false);
                    {
                        ImGui::BeginDisabled(!_view->IsActive());
                        ImGui::Text("Active");
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!_view->IsVisible());
                        ImGui::Text("Visible");
                        ImGui::EndDisabled();

                        ImGui::Text("Size");
                        ImGui::Text("Format");
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::BeginDisabled(!_view->IsActive());
                        ImGui::Text("%s", _view->IsActive() ? "True" : "False");
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!_view->IsVisible());
                        ImGui::Text("%s", _view->IsVisible() ? "True" : "False");
                        ImGui::EndDisabled();

                        ImGui::Text("%u x %u", rt->GetWidth(), rt->GetHeight());
                        ImGui::Text("%s", asString(rt->GetPixelFormat()).c_str());
                    }
                    ImGui::Columns(1);
                }

                ImGui::TreePop();
            }            
        };

        if (ImGui::IconBegin(getIcon().c_str(), "Statistics", &m_isVisible))
        {
            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                auto renderer = Editor::get()->getRenderer();

                if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::TreeNodeEx("Viewports", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        for (uint i = 0; i < enumCount<gfx::ViewportTarget>(); ++i)
                        {
                            const auto target = (gfx::ViewportTarget)i;
                            const auto & viewports = renderer->GetViewports(target);
                        
                            for (uint j = 0; j < viewports.size(); ++j)
                            {
                                const auto viewport = viewports[j];
                                if (ImGui::TreeNodeEx(viewport->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    for (auto & pair : viewport->GetViewIDs())
                                    {
                                        if (auto * view = renderer->GetView(pair.second))
                                        {
                                            if (view)
                                                drawViewDetail(view);
                                        }
                                    }

                                    ImGui::TreePop();
                                }
                            }
                        }

                        ImGui::TreePop();
                    }

                    //if (ImGui::TreeNodeEx("Views", ImGuiTreeNodeFlags_DefaultOpen))
                    //{
                    //    for (uint i = 0; i < enumCount<gfx::ViewTarget>(); ++i)
                    //    {
                    //        const auto target = (gfx::ViewTarget)i;
                    //        const auto & views = renderer->GetViews(target);
                    //
                    //        if (views.size() > 0)
                    //        {
                    //            for (uint j = 0; j < views.size(); ++j)
                    //            {
                    //                const auto & view = views[j];
                    //                if (nullptr != view)
                    //                    drawViewDetail(view);
                    //            }                            
                    //        }
                    //    }
                    //    ImGui::TreePop();
                    //}

                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}