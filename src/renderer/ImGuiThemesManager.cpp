#include "renderer/precomp.h"
#include "ImGuiThemesManager.h"
#include "ImGui.h"

using namespace ImGui;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::set(GUITheme _theme)
    {
        resetStyle();

        switch (_theme)
        {
            case GUITheme::Classic:
                styleColorsClassic();
                break;

            case GUITheme::Dark:
                styleColorsDark();
                break;

            case GUITheme::Light:
                styleColorsLight();
                break;

            case GUITheme::Custom:
                styleColorsCustom();
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::resetStyle()
    {
        ImGui::GetStyle() = ImGuiStyle();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::styleColorsClassic()
    {
        ImGui::StyleColorsClassic();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::styleColorsDark()
    {
        ImGui::StyleColorsDark();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::styleColorsLight()
    {
        ImGui::StyleColorsLight();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiThemesManager::styleColorsCustom()
    {
        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 * colors = style.Colors;

        float rounding = 8.0f;

        style.PopupRounding = rounding;
        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding = ImVec2(4, 4);
        style.ItemSpacing = ImVec2(4, 4);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.ScrollbarSize = 16;
        style.IndentSpacing = 16;
        style.WindowBorderSize = 0;
        style.ChildBorderSize = 0;
        style.PopupBorderSize = 0;
        style.FrameBorderSize = 0;
        style.WindowRounding = rounding;
        style.ChildRounding = rounding;
        style.FrameRounding = rounding;
        style.ScrollbarRounding = rounding;
        style.GrabRounding = rounding;
        style.Alpha = 0.9f;

        #ifdef IMGUI_HAS_DOCK 
        style.TabBorderSize = 0;
        style.TabRounding = rounding;
        #endif

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
        colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.13f, 0.15f, 0.50f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.231f, 0.310f, 0.447f, 1.000f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.231f, 0.310f, 0.447f, 1.000f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.23f, 0.34f, 0.54f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.33f, 0.42f, 0.60f, 0.77f);
        colors[ImGuiCol_Tab] = ImVec4(0.33f, 0.42f, 0.60f, 0.50f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.54f, 0.54f, 0.54f, 0.19f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.54f, 0.54f, 0.54f, 0.38f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.20f, 0.29f, 0.43f, 0.77f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}