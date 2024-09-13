#include "ImGuiGameView.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameView::ImGuiGameView(uint _index) :
        ImGuiView(style::icon::GameView, "View", "Game", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry, gfx::ViewportTarget::Game, _index)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiGameView::~ImGuiGameView()
    {

    }

    //--------------------------------------------------------------------------------------
    bool ImGuiGameView::ShowToolbar() const
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiGameView::DrawToolbar()
    {
        if (ImGui::InvisibleButton("FakeImGuiGameViewButton", style::button::SizeSmall))
        {

        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiGameView::DrawTitlebarMenu()
    {
        const auto * factory = Kernel::getFactory();
        auto engine = Editor::get()->getEngine();

        auto * worldRes = engine->GetWorldResource();

        if (ImGui::BeginPopupContextItem("ImGuiGameViewTitlebarMenu"))
        {
            ImGui::PushStyle(renderer::FontStyle::Bold);
            ImGui::Text(fmt::sprintf("Game", style::icon::GameView).c_str());
            ImGui::PopStyle();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::BeginMenu(fmt::sprintf("%s Aspect Ratio", style::icon::GameView).c_str()))
            {
                ImGui::BeginDisabled(true);

                if (ImGui::MenuItem("4/3"))
                {
                }

                if (ImGui::MenuItem("16/9"))
                {
                }

                if (ImGui::MenuItem("16/10"))
                {
                }

                ImGui::EndDisabled();

                if (ImGui::MenuItem(fmt::sprintf("Free", style::icon::Save).c_str()))
                {

                }

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        //ImGui::BeginDisabled(nullptr == worldRes->getObject());
        //
        //if (ImGui::MenuItem(fmt::sprintf("%s Save World & Scenes", style::icon::Save).c_str()))
        //{
        //    const uint sceneCount = worldRes->GetSceneResourceCount(BaseSceneType::Scene);
        //    for (uint i = 0; i < sceneCount; ++i)
        //    {
        //        auto sceneRes = worldRes->GetSceneResource(i, BaseSceneType::Scene);
        //        engine->SaveScene(sceneRes);
        //    }
        //}
        //ImGui::EndDisabled();
    }
}