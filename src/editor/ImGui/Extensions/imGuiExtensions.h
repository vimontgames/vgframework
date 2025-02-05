#pragma once

#include "editor/ImGui/ImGui.h"
#include "core/Types/Types.h"
#include "core/Core.h"

namespace vg::core
{
    class IObject;
    class IProperty;
}

namespace vg::renderer
{
    enum class Font : vg::core::u8;
    enum class FontStyle : vg::core::u8;
}

namespace ImGui
{
    vg_enum_class(ImGui, InteractionType, vg::core::u8,
        Single = 0,
        Continuous
    );

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

    // This version uses UID to generate a persistent ID
    vg::core::string getObjectPropertyPersistentLabel(const vg::core::string & _displayName, const vg::core::IObject * _object, const vg::core::IProperty * _prop);

    // These versions save the "opened" state in the imgui.ini file
    bool PersistentTreeNode(const vg::core::string & _label, vg::core::IObject * _object, const vg::core::IProperty * _prop, bool * _toggle = nullptr, ImGuiTreeNodeFlags _flags = 0);
    bool PersistentCollapsingHeader(const vg::core::string & _label, vg::core::IObject * _object, const vg::core::IProperty * _prop, bool * _toggle = nullptr);

    ImVec2 GetWindowContentRegionSize();

    void CollapsingHeaderLabel(const ImVec2 & _headerPos, const vg::core::string & _label, bool _enabled);
    bool CollapsingHeaderCheckbox(const ImVec2 & _headerPos, bool _enabled, vg::core::IObject * _object, const char * _iconTrue, const char * _iconFalse, const vg::core::string & _tooltip);
    bool CollapsingHeaderIconButton(const ImVec2 & _headerPos, float _availableWidth, vg::core::IObject * _object, const char * _icon, const vg::core::string & _tooltip, vg::core::uint _index = 0, const ImVec4 * _buttonColor = nullptr);

    void PushDefaultFont();
    void PushFont(vg::renderer::Font _font);
    void PushFont(vg::renderer::Font _font, vg::renderer::FontStyle _style);
    void PopFont();

    void PushStyle(vg::renderer::FontStyle _style);
    void PopStyle();

    void PushDisabledStyle(bool _disabled);
    void PopDisabledStyle();

    bool CustomDragScalarN(InteractionType & _interactionType, const char * label, ImGuiDataType data_type, void * p_data, int components, float v_speed = 1.0f, const void * p_min = NULL, const void * p_max = NULL, const char * format = NULL, ImGuiSliderFlags flags = 0);
    bool CustomDragFloat3(InteractionType & _interactionType, const char * label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char * format = "%.3f", ImGuiSliderFlags flags = 0);
    bool CustomDragFloat4(InteractionType & _interactionType, const char * label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char * format = "%.3f", ImGuiSliderFlags flags = 0);
}