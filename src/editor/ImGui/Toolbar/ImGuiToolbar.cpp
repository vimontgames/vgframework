#include "editor/Precomp.h"
#include "ImGuiToolbar.h"
#include "editor/Editor.h"
#include "Main/ImGuiMainToolbar.hpp"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiToolbar::ImGuiToolbar(const core::string & _name, Flags _flags) :
        ImGuiWindow("", "", _name, _flags)
    {

    }

    //--------------------------------------------------------------------------------------
    ImVec2 ImGuiToolbar::ComputeButtonSize()
    {
        const float font_size = ImGui::GetFontSize();
        return ImVec2(ImFloor(font_size * 1.5f), ImFloor(font_size * 1.5f));
    }

    //--------------------------------------------------------------------------------------
    // https://github.com/ocornut/imgui/issues/2648
    //--------------------------------------------------------------------------------------
    void ImGuiToolbar::DrawGUI()
    {
        // [Option] Automatically update axis based on parent split (inside of doing it via right-click on the toolbar)
        // Pros:
        // - Less user intervention.
        // - Avoid for need for saving the toolbar direction, since it's automatic.
        // Cons: 
        // - This is currently leading to some glitches.
        // - Some docking setup won't return the axis the user would expect.
        const bool TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED = true;

        // ImGuiAxis_X = horizontal toolbar
        // ImGuiAxis_Y = vertical toolbar
        ImGuiAxis toolbar_axis = m_toolbarAxis;

        // 1. We request auto-sizing on one axis
        // Note however this will only affect the toolbar when NOT docked.
        ImVec2 requested_size = (m_toolbarAxis == ImGuiAxis_X) ? ImVec2(-1.0f, 0.0f) : ImVec2(0.0f, -1.0f);
        ImGui::SetNextWindowSize(requested_size);

        // 2. Specific docking options for toolbars.
        // Currently they add some constraint we ideally wouldn't want, but this is simplifying our first implementation
        ImGuiWindowClass window_class;
        window_class.DockingAllowUnclassed = true;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar // FIXME: Will need a working Undock widget for _NoTabBar to work
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingSplit;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverMe;
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverOther;
        if (toolbar_axis == ImGuiAxis_X)
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeY;
        else
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeX;
        ImGui::SetNextWindowClass(&window_class);

        const auto name = getName();

        // 3. Begin into the window
        m_buttonSize = ComputeButtonSize();
        ImGui::Begin(name.c_str(), &m_isVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);        

        // 4. Overwrite node size
        ImGuiDockNode * node = ImGui::GetWindowDockNode();
        if (node != NULL)
        {
            // Overwrite size of the node
            ImGuiStyle & style = ImGui::GetStyle();
            const ImGuiAxis toolbar_axis_perp = (ImGuiAxis)(toolbar_axis ^ 1);
            const float TOOLBAR_SIZE_WHEN_DOCKED = style.WindowPadding[toolbar_axis_perp] * 0.0f + m_buttonSize[toolbar_axis_perp];
            node->WantLockSizeOnce = true;
            node->Size[toolbar_axis_perp] = node->SizeRef[toolbar_axis_perp] = TOOLBAR_SIZE_WHEN_DOCKED;

            if (TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED)
                if (node->ParentNode && node->ParentNode->SplitAxis != ImGuiAxis_None)
                    toolbar_axis = (ImGuiAxis)(node->ParentNode->SplitAxis ^ 1);
        }

        // 5. Dummy populate tab bar
        //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));
        //UndockWidget(icon_size, toolbar_axis);
        //for (int icon_n = 0; icon_n < 10; icon_n++)
        //{
        //    char label[32];
        //    ImFormatString(label, IM_ARRAYSIZE(label), "%02d", icon_n);
        //    if (icon_n > 0 && toolbar_axis == ImGuiAxis_X)
        //        ImGui::SameLine();
        //    ImGui::Button(label, icon_size);
        //}

        // Compute Window content size
        ImVec2 size = ImGui::GetWindowContentRegionSize();
        ImGuiStyle & style = ImGui::GetStyle();

        float buttonSize = ImGui::CalcTextSize(style::icon::GameView).x + style.FramePadding.x*4.0f;
        int buttonCount = GetButtonCount();

        float offset;
        if (buttonCount & 1)
            offset = size.x/2.0f - buttonCount / 2 * buttonSize - buttonSize*0.5f;
        else
            offset = size.x/2.0f - buttonCount / 2 * buttonSize;

        ImGui::SetCursorPos(ImVec2(offset,0));

        DrawButtons();

        ImGui::PopStyleVar(1);

        // 6. Context-menu to change axis
        if (node == NULL || !TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED)
        {
            if (ImGui::BeginPopupContextWindow())
            {
                ImGui::TextUnformatted(name.c_str());
                ImGui::Separator();
                if (ImGui::MenuItem("Horizontal", "", (toolbar_axis == ImGuiAxis_X)))
                    toolbar_axis = ImGuiAxis_X;
                if (ImGui::MenuItem("Vertical", "", (toolbar_axis == ImGuiAxis_Y)))
                    toolbar_axis = ImGuiAxis_Y;
                ImGui::EndPopup();
            }
        }

        ImGui::End();

        // Output user stored data
        m_toolbarAxis = toolbar_axis;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiToolbar::nextItem()
    {
        if (m_toolbarAxis == ImGuiAxis_X)
            ImGui::SameLine();
    }
}
