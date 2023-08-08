#include "editor/Precomp.h"
#include "imguiExtensions.h"

using namespace vg::core;

namespace ImGui
{
    //--------------------------------------------------------------------------------------
    bool IconBegin(const char * icon, const char * name, bool * p_open, ImGuiWindowFlags flags)
    {
        return Begin(IconWithText(icon, name).c_str(), p_open, flags);
    }

    //--------------------------------------------------------------------------------------
    bool IconMenuItem(const char* icon, const char* label, const char* shortcut, bool selected, bool enabled)
    {
        return MenuItem(IconWithText(icon, label).c_str(), shortcut, selected, enabled);
    }

    //--------------------------------------------------------------------------------------
    const string IconWithText(const char* icon, const char* text)
    {
        auto temp = ((string)icon + " " + (string)text);
        return temp;
    }

    //--------------------------------------------------------------------------------------
    bool ButtonEx(const string & _label, bool _enabled, const string & _tooltip, ImVec2 _buttonSize)
    {
        bool clicked = false;

        ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 buttonColorDisabled = ImVec4(buttonColor.x, buttonColor.y, buttonColor.z, buttonColor.w * 0.5f);

        ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImVec4 textColorDisabled = ImVec4(textColor.x, textColor.y, textColor.z, textColor.w * 0.5f);

        if (!_enabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColorDisabled);
            ImGui::PushStyleColor(ImGuiCol_Text, textColorDisabled);
        }

        if (ImGui::Button(_label.c_str(), _buttonSize) && _enabled)
        {
            clicked = true;
        }

        if (!_tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip(_tooltip.c_str());

        if (!_enabled)
            ImGui::PopStyleColor(2);

        return clicked;
    }
}