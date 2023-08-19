#pragma once

#include "editor/ImGui/ImGui.h"
#include "core/Types/Types.h"
#include "core/Core.h"

namespace ImGui
{
    bool IconBegin(const char * icon, const char * name, bool * p_open = nullptr, ImGuiWindowFlags flags = 0x0);
    bool IconMenuItem(const char * icon, const char * label, const char * shortcut = nullptr, bool selected = false, bool enabled = true);
    const vg::core::string IconWithText(const char * icon, const char* text);
    bool ButtonEx(const vg::core::string & _label, bool _enabled = true, bool _clickable = true, const vg::core::string & _tooltip = {}, ImVec2 _buttonSize = ImVec2(0, 0));

    using OnMsgBoxClickedFunc = std::function<bool()>;
    void MessageBox(vg::core::MessageBoxType _msgBoxType, const vg::core::string & _title, const vg::core::string & _message, OnMsgBoxClickedFunc _onClick0 = nullptr, OnMsgBoxClickedFunc _onClick1 = nullptr, OnMsgBoxClickedFunc _onClick2 = nullptr);
    void CustomMessageBox(const vg::core::string & _title, const vg::core::string & _message, const vg::core::string & _label0, OnMsgBoxClickedFunc _onClick0);
    void CustomMessageBox(const vg::core::string & _title, const vg::core::string & _message, const vg::core::string & _label0, OnMsgBoxClickedFunc _onClick0, const vg::core::string & _label1, OnMsgBoxClickedFunc _onClick1);
    bool ProcessMessageBox();

    void DrawRowsBackground(int row_count, ImU32 col_even, ImU32 col_odd = 0);

    ImVec2 GetWindowContentRegionSize();
}