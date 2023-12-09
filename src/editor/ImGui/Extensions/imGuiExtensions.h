#pragma once

#include "editor/ImGui/ImGui.h"
#include "core/Types/Types.h"
#include "core/Core.h"

namespace vg::core
{
    class IObject;
}

namespace ImGui
{
    ImVec4 getDisabledButtonColor();
    ImVec4 getDisabledTextColor();

    bool IconBegin(const char * icon, const char * name, bool * p_open = nullptr, ImGuiWindowFlags flags = 0x0);
    bool IconMenuItem(const char * icon, const char * label, const char * shortcut = nullptr, bool selected = false, bool enabled = true);
    const vg::core::string IconWithText(const char * icon, const char* text);
    bool TextButton(const vg::core::string & _textLabel, const vg::core::string & _buttonLabel, bool _enabled = true, const vg::core::string & _tooltip = {});
    bool TooltipButton(const vg::core::string & _label, bool _enabled = true, bool _clickable = true, const vg::core::string & _tooltip = {}, ImVec2 _buttonSize = ImVec2(0, 0));
    
    using OnMsgBoxClickedFunc = std::function<bool()>;
    void MessageBox(vg::core::MessageBoxType _msgBoxType, const vg::core::string & _title, const vg::core::string & _message, OnMsgBoxClickedFunc _onClick0 = nullptr, OnMsgBoxClickedFunc _onClick1 = nullptr, OnMsgBoxClickedFunc _onClick2 = nullptr);
    void CustomMessageBox(const vg::core::string & _title, const vg::core::string & _message, const vg::core::string & _label0, OnMsgBoxClickedFunc _onClick0);
    void CustomMessageBox(const vg::core::string & _title, const vg::core::string & _message, const vg::core::string & _label0, OnMsgBoxClickedFunc _onClick0, const vg::core::string & _label1, OnMsgBoxClickedFunc _onClick1);
    bool ProcessMessageBox();

    void DrawRowsBackground(int row_count, ImU32 col_even, ImU32 col_odd = 0);

    vg::core::string getObjectLabel(const vg::core::string & _label, const void * _object);
    vg::core::string getObjectLabel(const vg::core::string & _label, const vg::core::string & _subLabel, const void * _object);

    ImVec2 GetWindowContentRegionSize();

    void CollapsedHeaderLabel(const ImVec2 & _headerPos, const vg::core::string & _label, bool _enabled);
    bool CollapsedHeaderCheckbox(const ImVec2 & _headerPos, bool _enabled, vg::core::IObject * _object, const char * _iconTrue, const char * _iconFalse, const vg::core::string & _tooltip);
    bool CollapsedHeaderIconButton(const ImVec2 & _headerPos, float _availableWidth, vg::core::IObject * _object, const char * _icon, const vg::core::string & _tooltip);
}