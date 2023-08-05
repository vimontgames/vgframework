#include "imguiExtensions.h"

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
    const vg::core::string IconWithText(const char* icon, const char* text)
    {
        auto temp = ((vg::core::string)icon + " " + (vg::core::string)text);
        return temp;
    }
}