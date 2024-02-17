#include "ImGuiMainToolbar.h"
#include "engine/IEngine.h"
#include "renderer/IRenderer.h"
#include "editor/ImGui/Extensions/ImGuiExtensions.h"
#include "core/ISelection.h"
#include "editor/Options/EditorOptions.h"

namespace vg::editor
{
    using namespace vg::engine;

    //--------------------------------------------------------------------------------------
    ImGuiMainToolbar::ImGuiMainToolbar(const core::string & _name, Flags _flags) :
        ImGuiToolbar(_name, _flags)
    {

    }

    //--------------------------------------------------------------------------------------
    core::uint ImGuiMainToolbar::GetButtonCount(Align _align) const
    {
        switch (_align)
        {
            default:
                return 0;

            case Align::Center:
                return 5;
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiMainToolbar::DrawButtons(Align _align)
    {
        IEngine * engine = Editor::get()->getEngine();

        switch (_align)
        {
            case Align::Left:
            {
                core::ISelection * selection = engine->GetSelection();
                bool anySel = selection->GetSelectedObjects().size() > 0;

                EditorOptions * options = EditorOptions::get();
                auto & gizmoOptions = options->getGizmoOptions();

                if (ImGui::TooltipButton(style::icon::Translate, GizmoType::Translate == gizmoOptions.m_type, true, "Translate (Ctrl-T)", style::button::SizeSmall))
                    options->setGizmoType(GizmoType::Translate);

                nextItem();

                if (ImGui::TooltipButton(style::icon::Rotate, GizmoType::Rotate == gizmoOptions.m_type, true, "Rotate (Ctrl-R)", style::button::SizeSmall))
                    options->setGizmoType(GizmoType::Rotate);

                nextItem();

                if (ImGui::TooltipButton(style::icon::Scale, GizmoType::Scale == gizmoOptions.m_type, true, "Scale (Ctrl-S)", style::button::SizeSmall))
                    options->setGizmoType(GizmoType::Scale);

                nextItem();

                bool * snap = nullptr;
                const char * tooltip = nullptr;
                switch (gizmoOptions.m_type)
                {
                    case GizmoType::Translate:
                        snap = (bool *)&gizmoOptions.m_snapTranslate;
                        tooltip = "Snap Align Translate (A)";
                        break;

                    case GizmoType::Rotate:
                        snap = (bool *)&gizmoOptions.m_snapRotate;
                        tooltip = "Snap Align Rotate (A)";
                        break;

                    case GizmoType::Scale:
                        snap = (bool *)&gizmoOptions.m_snapScale;
                        tooltip = "Snap Align Scale (A)";
                        break;
                }

                if (ImGui::TooltipButton(style::icon::Snap, *snap, true, tooltip, style::button::SizeSmall))
                    *snap = !(*snap);

                nextItem();

                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_Button));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
                if (ImGui::BeginCombo(ImGui::getObjectLabel("", this).c_str(), core::asString(gizmoOptions.m_space).c_str(), ImGuiComboFlags_HeightSmall | ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton))
                {
                    for (core::uint e = 0; e < core::enumCount<GizmoSpace>(); ++e)
                    {
                        auto value = core::asString((GizmoSpace)e);
                        if (ImGui::Selectable(value.c_str()))
                            gizmoOptions.m_space = (GizmoSpace)e;
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopStyleColor(2);
            }
            break;

            case Align::Center:
            {
                auto * world = engine->GetMainWorld();

                const bool playing = world && world->IsPlaying();
                const bool paused = world && world->IsPaused();

                if (ImGui::TooltipButton(style::icon::Restart, playing, playing, "Restart (Shift-F5)", style::button::SizeSmall) && world)
                {
                    world->Stop();
                    world->Play();
                }
                nextItem();

                if (ImGui::TooltipButton(style::icon::Play, !playing, !playing, "Play (F5)", style::button::SizeSmall) && world)
                {
                    world->Play();
                }
                nextItem();

                if (ImGui::TooltipButton(style::icon::Pause, playing && !paused, playing, "Pause (PAUSE)", style::button::SizeSmall) && world)
                {
                    if (world->IsPaused())
                        world->Resume();
                    else
                        world->Pause();
                }
                nextItem();

                if (ImGui::TooltipButton(style::icon::Stop, playing, playing, "Stop (ESC)", style::button::SizeSmall) && world)
                {
                    world->Stop();
                }
                nextItem();

                renderer::IRenderer * renderer = engine->GetRenderer();
                const bool fullscreen = renderer->IsFullscreen();
                if (ImGui::TooltipButton(style::icon::Maximize, true, true, "Fullscreen (F11)", style::button::SizeSmall))
                    renderer->SetFullscreen(!fullscreen);
                nextItem();
            }
            break;
        }
    }
}