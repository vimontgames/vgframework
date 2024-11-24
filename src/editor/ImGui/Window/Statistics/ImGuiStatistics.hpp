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
        auto renderer = Editor::get()->getRenderer();

        auto drawViewDetail = [=](renderer::IView * _view)
        {
            if (ImGui::TreeNodeEx(_view->GetName().c_str(), ImGuiTreeNodeFlags_None))
            {
                ImGui::Columns(2, "mycolumns2", false);
                {
                    ImGui::BeginDisabled(!_view->IsRender());
                    ImGui::Text("Render");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_view->GetSize() == 0));
                    ImGui::Text("Size");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_view->GetOffset() == 0));
                    ImGui::Text("Offset");
                    ImGui::EndDisabled();
                }
                ImGui::NextColumn();
                {
                    ImGui::BeginDisabled(!_view->IsRender());
                    ImGui::Text("%s", _view->IsRender() ? "True" : "False");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_view->GetSize() == 0));
                    ImGui::Text("%u x %u", (uint)_view->GetSize().x, (uint)_view->GetSize().y);
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_view->GetOffset() == 0));
                    ImGui::Text("%u, %u", (uint)_view->GetOffset().x, (uint)_view->GetOffset().y);
                    ImGui::EndDisabled();
                }
                ImGui::Columns(1);

                if (ImGui::TreeNodeEx(getObjectLabel("Instance", _view).c_str(), ImGuiTreeNodeFlags_None))
                {
                    ImGui::Columns(2, "mycolumns2", false);
                    {
                        const auto & stats = _view->GetViewCullingStats();
                        ImGui::BeginDisabled(!stats.opaque);
                        ImGui::Text("Opaque");
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.transparent);
                        ImGui::Text("Transparent");
                        ImGui::EndDisabled();
                    }
                    ImGui::NextColumn();
                    {
                        const auto & stats = _view->GetViewCullingStats();

                        ImGui::BeginDisabled(!stats.opaque);
                        ImGui::Text("%u", stats.opaque);
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.transparent);
                        ImGui::Text("%u", stats.transparent);
                        ImGui::EndDisabled();
                    }
                    ImGui::Columns(1);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx(getObjectLabel("Lights", _view).c_str(), ImGuiTreeNodeFlags_None))
                {
                    ImGui::Columns(2, "mycolumns2", false);
                    {
                        const auto & stats = _view->GetViewCullingStats();

                        ImGui::BeginDisabled(!stats.directional);
                        ImGui::Text("Directional");
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.omni);
                        ImGui::Text("Omni");
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.spot);
                        ImGui::Text("Spot");
                        ImGui::EndDisabled();
                    }
                    ImGui::NextColumn();
                    {
                        const auto & stats = _view->GetViewCullingStats();

                        ImGui::BeginDisabled(!stats.directional);
                        ImGui::Text("%u", stats.directional);
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.omni);
                        ImGui::Text("%u", stats.omni);
                        ImGui::EndDisabled();

                        ImGui::BeginDisabled(!stats.spot);
                        ImGui::Text("%u", stats.spot);
                        ImGui::EndDisabled();
                    }
                    ImGui::Columns(1);
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }            
        };

        auto drawViewportDetails = [=](renderer::IViewport * _viewport)
        {
            if (ImGui::TreeNodeEx(_viewport->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);
                {
                    ImGui::BeginDisabled(!_viewport->AnyFocused());
                    ImGui::Text("Focus");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(!_viewport->AnyVisible());
                    ImGui::Text("Visible");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_viewport->GetRenderTargetSize() == 0));
                    ImGui::Text("Size");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(!_viewport->GetRenderTarget());
                    ImGui::Text("Format");
                    ImGui::EndDisabled();
                }
                ImGui::NextColumn();
                {
                    ImGui::BeginDisabled(!_viewport->AnyFocused());
                    ImGui::Text("%s", _viewport->AnyFocused() ? "True" : "False");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(!_viewport->AnyVisible());
                    ImGui::Text("%s", _viewport->AnyVisible() ? "True" : "False");
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(all(_viewport->GetRenderTargetSize() == 0));
                    ImGui::Text("%u x %u", (uint)_viewport->GetRenderTargetSize().x, (uint)_viewport->GetRenderTargetSize().y);
                    ImGui::EndDisabled();

                    if (auto * rt = _viewport->GetRenderTarget())
                        ImGui::Text("%s", asString(rt->GetPixelFormat()).c_str());
                    else
                        ImGui::Text("");
                }
                ImGui::Columns(1);

                for (auto & pair : _viewport->GetViewIDs())
                {
                    if (auto * view = renderer->GetView(pair.second))
                    {
                        if (view)
                            drawViewDetail(view);
                    }
                }

                ImGui::TreePop();
            }
        };

        if (ImGui::IconBegin(getIcon().c_str(), "Statistics", &m_isVisible))
        {
            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
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

                                if (viewport)
                                    drawViewportDetails(viewport);
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