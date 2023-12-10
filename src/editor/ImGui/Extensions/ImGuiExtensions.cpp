#include "editor/Precomp.h"
#include "imguiExtensions.h"
#include "core/Math/Math.h"
#include "core/IObject.h"
#include "imgui/imgui_internal.h"

using namespace vg::core;
using namespace vg::editor;

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
    ImVec4 getDisabledButtonColor()
    {
        ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 buttonColorDisabled = ImVec4(buttonColor.x, buttonColor.y, buttonColor.z, buttonColor.w * 0.5f);
        return buttonColorDisabled;
    }

    //--------------------------------------------------------------------------------------
    ImVec4 getDisabledTextColor()
    {
        ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImVec4 textColorDisabled = ImVec4(textColor.x, textColor.y, textColor.z, 0.25f);
        return textColorDisabled;
    }

    //--------------------------------------------------------------------------------------
    bool ButtonInternal(const string & _textLabel, const string & _buttonLabel, bool _enabled = true, bool _clickable = true, const string & _tooltip = {}, ImVec2 _buttonSize = ImVec2(0, 0))
    {
        bool clicked = false;

        ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 buttonColorDisabled = getDisabledButtonColor();

        ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImVec4 textColorDisabled = getDisabledTextColor();

        if (!_enabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColorDisabled);

            if (!_clickable)
                ImGui::PushStyleColor(ImGuiCol_Text, textColorDisabled);
        }

        if (!_textLabel.empty())
        {
            auto y = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(y + 4);
            ImGui::Text(_textLabel.c_str());
            ImGui::SameLine();
            ImGui::SetCursorPosY(y);
        }

        if (ImGui::Button(_buttonLabel.c_str(), _buttonSize) && _clickable)
        {
            clicked = true;
        }

        if (!_tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip(_tooltip.c_str());

        if (!_enabled)
        {
            ImGui::PopStyleColor(); 
            if (!_clickable)
                ImGui::PopStyleColor();
        }

        return clicked;
    }

    //--------------------------------------------------------------------------------------
    bool TooltipButton(const string & _label, bool _enabled, bool _clickable, const string & _tooltip, ImVec2 _buttonSize)
    {
        return ButtonInternal("", _label, _enabled, _clickable, _tooltip, _buttonSize);
    }

    //--------------------------------------------------------------------------------------
    bool TextButton(const string & _textLabel, const string & _buttonLabel, bool _enabled, const string & _tooltip)
    {
        return ButtonInternal(_textLabel, _buttonLabel, _enabled, _enabled, _tooltip);
    }

    //--------------------------------------------------------------------------------------
    // ModalMsgBox
    //--------------------------------------------------------------------------------------
    struct ModalMsgBox
    {
        struct Button
        {
            string label;
            OnMsgBoxClickedFunc action;
        };
        vector<Button> m_items;
        string m_title;
        string m_message;
        bool m_needOpen = false;
    };
    static ModalMsgBox g_ModalMsgBox;

    //--------------------------------------------------------------------------------------
    template <uint N> void CustomMessageBox(const string & _title, const string & _message, ModalMsgBox::Button(&_buttons)[N])
    {
        g_ModalMsgBox.m_title = _title;
        g_ModalMsgBox.m_message = _message;
        g_ModalMsgBox.m_needOpen = true;
        g_ModalMsgBox.m_items.clear();

        for (uint i = 0; i < N; ++i)
            g_ModalMsgBox.m_items.push_back(_buttons[i]);
    }

    //--------------------------------------------------------------------------------------
    void CustomMessageBox(const string & _title, const string & _message, const string & _label0, OnMsgBoxClickedFunc _onClick0)
    {
        ModalMsgBox::Button buttons[] =
        {
            { _label0, _onClick0 }
        };
        CustomMessageBox(_title, _message, buttons);
    }

    //--------------------------------------------------------------------------------------
    void CustomMessageBox(const string & _title, const string & _message, const string & _label0, OnMsgBoxClickedFunc _onClick0, const string & _label1, OnMsgBoxClickedFunc _onClick1)
    {
        ModalMsgBox::Button buttons[] =
        {
            { _label0, _onClick0 },
            { _label1, _onClick1 }
        };
        CustomMessageBox(_title, _message, buttons);
    }

    //--------------------------------------------------------------------------------------
    void MessageBox(MessageBoxType _msgBoxType, const string & _title, const string & _message, OnMsgBoxClickedFunc _onClick0, OnMsgBoxClickedFunc _onClick1, OnMsgBoxClickedFunc _onClick2)
    {
        switch (_msgBoxType)
        {
            default:
                VG_ASSERT(false, "Unhandled MessageBoxType \"%s\" (%u)", asString(_msgBoxType).c_str(), _msgBoxType);

            case OK:
            {
                ModalMsgBox::Button buttons[] =
                {
                    { "OK",     _onClick0 }
                };
                CustomMessageBox(_title, _message, buttons);
            }
            break;

            case YesNo:
            {
                ModalMsgBox::Button buttons[] =
                {
                    { "Yes",    _onClick0 },
                    { "No",     _onClick1 }
                };
                CustomMessageBox(_title, _message, buttons);
            }
            break;

            case YesNoCancel:
            {
                ModalMsgBox::Button buttons[] =
                {
                    { "Yes",    _onClick0 },
                    { "No",     _onClick1 },
                    { "Cancel", _onClick2 },
                };
                CustomMessageBox(_title, _message, buttons);
            }
            break;
        }
    }

    //--------------------------------------------------------------------------------------
    // cf. https://github.com/ocornut/imgui/discussions/3862
    //--------------------------------------------------------------------------------------
    void AlignForWidth(float width, float alignment = 0.5f)
    {
        ImGuiStyle & style = ImGui::GetStyle();
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - width) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

    //--------------------------------------------------------------------------------------
    bool ProcessMessageBox()
    {
        bool close = false;
        bool init = false;
        
        if (g_ModalMsgBox.m_needOpen)
        {
            ImGui::OpenPopup(g_ModalMsgBox.m_title.c_str());
            g_ModalMsgBox.m_needOpen = false;
            init = true;
        }

        if (g_ModalMsgBox.m_items.size() > 0)
        {
            if (ImGui::BeginPopupModal(g_ModalMsgBox.m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(g_ModalMsgBox.m_message.c_str());

                // First pass to compute button + space width
                float fixedButtonWidth = 64;
                ImGuiStyle & style = ImGui::GetStyle();
                float width = 0;
                for (uint i = 0; i < g_ModalMsgBox.m_items.size(); ++i)
                {
                    const auto & item = g_ModalMsgBox.m_items[i];
                    width += max(fixedButtonWidth, ImGui::CalcTextSize(item.label.c_str()).x);
                    if (i != 0)
                        width += style.ItemSpacing.x;
                }
                AlignForWidth(width);

                // Second pass to draw centered buttons
                for (uint i = 0; i < g_ModalMsgBox.m_items.size(); ++i)
                {
                    const auto & item = g_ModalMsgBox.m_items[i];
                    bool isLastButton = i + 1 == g_ModalMsgBox.m_items.size();

                    if (init && isLastButton)
                        ImGui::SetKeyboardFocusHere();

                    if ( !item.label.empty() && (ImGui::Button(item.label.c_str(), ImVec2(fixedButtonWidth,0))) || (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)) )
                    {
                        if (item.action != nullptr)
                            close = item.action();
                        ImGui::CloseCurrentPopup();
                    }
                    if (!isLastButton)
                        ImGui::SameLine();
                }
                ImGui::EndPopup();
            }
        }
        return close;
    }

    //--------------------------------------------------------------------------------------
    void DrawRowsBackground(int row_count, ImU32 col_even, ImU32 col_odd)
    {
        float x1 = ImGui::GetWindowPos().x;
        float x2 = x1 + ImGui::GetWindowSize().x;
        float item_spacing_y = ImGui::GetStyle().ItemSpacing.y;
        float item_offset_y = -item_spacing_y * 0.5f;
        float line_height = ImGui::GetTextLineHeight() + item_spacing_y;

        ImDrawList * draw_list = ImGui::GetWindowDrawList();
        float y0 = ImGui::GetCursorScreenPos().y + (float)(int)item_offset_y;

        int row_display_start;
        int row_display_end;
        ImGui::CalcListClipping(row_count, line_height, &row_display_start, &row_display_end);
        for (int row_n = row_display_start; row_n < row_display_end; row_n++)
        {
            ImU32 col = (row_n & 1) ? col_odd : col_even;
            if ((col & IM_COL32_A_MASK) == 0)
                continue;
            float y1 = y0 + (line_height * row_n);
            float y2 = y1 + line_height;
            draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col);
        }
    }

    //--------------------------------------------------------------------------------------
    string getObjectLabel(const string & _label, const void * _object)
    {
        return fmt::sprintf("%s###%s_%p", _label.c_str(), _label.c_str(), _object);
    }

    //--------------------------------------------------------------------------------------
    string getObjectLabel(const string & _label, const string & _subLabel, const void * _object)
    {
        return fmt::sprintf("%s###%s_%s_%p", _label.c_str(), _subLabel.c_str(), _label.c_str(), _object);
    }    

    //--------------------------------------------------------------------------------------
    ImVec2 GetWindowContentRegionSize()
    {
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        return ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);
    }

    //--------------------------------------------------------------------------------------
    void CollapsingHeaderLabel(const ImVec2 & _headerPos, const string & _label, bool _enabled)
    {
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x * 2 + style::button::SizeSmall.x, _headerPos.y));
        ImGui::BeginDisabled(!_enabled);
        ImGui::Text(_label.c_str());
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------------------
    bool ImGui::CollapsingHeaderCheckbox(const ImVec2 & _headerPos, bool _enabled, IObject * _object, const char * _iconTrue, const char * _iconFalse, const string & _tooltip)
    {
        ImGui::SameLine();

        auto collapsedButtonSize = style::button::SizeSmall;
        if (ImGui::GetStyle().FramePadding.y == 3)
            collapsedButtonSize.y -= 1;

        auto icon = _enabled ? style::icon::Checked : style::icon::Unchecked;

        // begin of line
        ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x, _headerPos.y));

        ImGui::ButtonEx(ImGui::getObjectLabel(icon, "CheckBoxEnable", _object).c_str(), collapsedButtonSize, ImGuiItemFlags_AllowOverlap);

        if (ImGui::IsItemHovered())
        {
            if (!_tooltip.empty())
                ImGui::SetTooltip(_tooltip.c_str());

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool CollapsingHeaderIconButton(const ImVec2 & _headerPos, float _availableWidth, IObject * _object, const char * _icon, const string & _tooltip)
    {
        auto collapsedButtonSize = style::button::SizeSmall;
        if (ImGui::GetStyle().FramePadding.y == 3)
            collapsedButtonSize.y -= 1;

        ImGui::SameLine();

        ImGui::SetCursorPos(ImVec2(_availableWidth - style::button::SizeSmall.x + ImGui::GetStyle().WindowPadding.x + 4, _headerPos.y));

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::ButtonEx(ImGui::getObjectLabel(fmt::sprintf("%s", style::icon::Trashcan).c_str(), _object).c_str(), collapsedButtonSize, ImGuiItemFlags_AllowOverlap);
        ImGui::PopStyleVar(1);

        if (ImGui::IsItemHovered())
        {
            if (!_tooltip.empty())
                ImGui::SetTooltip(_tooltip.c_str());

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                return true;            
        }

        return false;
    }
}