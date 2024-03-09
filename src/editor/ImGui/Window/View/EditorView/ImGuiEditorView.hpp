#include "ImGuiEditorView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiEditorView::ImGuiEditorView(uint _index) :
        ImGuiView(style::icon::EditorView, "View", "Editor", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewTarget::Editor, _index)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiEditorView::~ImGuiEditorView()
    {

    }

    //--------------------------------------------------------------------------------------
    bool ImGuiEditorView::ShowToolbar() const
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEditorView::DrawToolbar()
    {
        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();
        
        auto * worldRes = engine->GetWorldResource();
        
        ImGui::BeginDisabled(nullptr == worldRes->getObject());
        if (ImGui::TooltipButton(fmt::sprintf("%s", editor::style::icon::Save).c_str(), true, true, "Save World & Scenes", style::button::SizeSmall))
        {
            const uint sceneCount = worldRes->GetSceneResourceCount(BaseSceneType::Scene);
            for (uint i = 0; i < sceneCount; ++i)
            {
                auto sceneRes = worldRes->GetSceneResource(i, BaseSceneType::Scene);
                engine->SaveScene(sceneRes);
            }
        
            engine->SaveWorld();
        }
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiEditorView::DrawTitlebarMenu()
    {
        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();
        
        auto * worldRes = engine->GetWorldResource();

        if (ImGui::BeginPopupContextItem("ImGuiEditorViewTitlebarMenu"))
        {
            ImGui::PushStyle(Style::Bold);
            ImGui::Text(fmt::sprintf("Scene", style::icon::Scene).c_str());
            ImGui::PopStyle();
            ImGui::Spacing();
        
            ImGui::BeginDisabled(nullptr == worldRes->getObject());

            if (ImGui::MenuItem(fmt::sprintf("%s Save World", style::icon::World).c_str()))
            {
                engine->SaveWorld();
            }

            if (ImGui::MenuItem(fmt::sprintf("%s Save Scenes", style::icon::Scene).c_str()))
            {
                const uint sceneCount = worldRes->GetSceneResourceCount(BaseSceneType::Scene);
                for (uint i = 0; i < sceneCount; ++i)
                {
                    auto sceneRes = worldRes->GetSceneResource(i, BaseSceneType::Scene);
                    engine->SaveScene(sceneRes);
                }
            }
        
            if (ImGui::MenuItem(fmt::sprintf("%s Save All", style::icon::Save).c_str(), "Ctrl-S"))
            {
                engine->SaveWorld();

                const uint sceneCount = worldRes->GetSceneResourceCount(BaseSceneType::Scene);
                for (uint i = 0; i < sceneCount; ++i)
                {
                    auto sceneRes = worldRes->GetSceneResource(i, BaseSceneType::Scene);
                    engine->SaveScene(sceneRes);
                }
            }
        
            ImGui::EndDisabled();
            ImGui::EndPopup();
        }
    }
}