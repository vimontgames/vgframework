#pragma once

#include "imgui/imgui.h"
#include "core/Types/Types.h"

namespace ImGui
{
    bool IconBegin(const char * icon, const char * name, bool * p_open = nullptr, ImGuiWindowFlags flags = 0x0);
    bool IconMenuItem(const char * icon, const char * label, const char * shortcut = nullptr, bool selected = false, bool enabled = true);

    const vg::core::string IconWithText(const char* icon, const char* text);
}