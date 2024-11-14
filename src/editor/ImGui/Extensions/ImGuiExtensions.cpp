#include "editor/Precomp.h"
#include "imguiExtensions.h"
#include "core/Math/Math.h"
#include "core/IObject.h"
#include "editor/Editor.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "core/File/File.h"

using namespace vg::core;
using namespace vg::editor;
using namespace vg::renderer;

namespace ImGui
{
    //--------------------------------------------------------------------------------------
    bool PersistentTreeNode(vg::core::IObject * _object, const vg::core::IProperty * _prop, ImGuiTreeNodeFlags _flags)
    {
        const auto label = ImGui::getObjectPropertyLabel(_object, _prop);

        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        auto & customData = imGuiAdapter->GetCustomData(label);

        customData.isOpen = ImGui::TreeNodeEx(label.c_str(), _flags | (customData.isOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0));
        
        return customData.isOpen;
    }

    //--------------------------------------------------------------------------------------
    bool PersistentCollapsingHeader(vg::core::IObject * _object, const vg::core::IProperty * _prop)
    {
        return PersistentTreeNode(_object, _prop, ImGuiTreeNodeFlags_CollapsingHeader);
    }

    //--------------------------------------------------------------------------------------
    bool IconBegin(const char * icon, const char * name, bool * p_open, ImGuiWindowFlags flags)
    {
        char temp[1024];
        sprintf(temp, "%s %s", icon, name);
        return Begin(temp, p_open, flags);
    }

    //--------------------------------------------------------------------------------------
    bool IconMenuItem(const char * icon, const char * label, const char* shortcut, bool selected, bool enabled)
    {
        char temp[1024];
        sprintf(temp, "%s %s", icon, label);
        return MenuItem(temp, shortcut, selected, enabled);
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
            ImGui::PushStyleColor(ImGuiCol_Text, textColorDisabled);

            if (!_clickable)
                ImGui::PushStyleColor(ImGuiCol_Button, buttonColorDisabled);
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

        if (!_enabled)
        {
            ImGui::PopStyleColor();
            if (!_clickable)
                ImGui::PopStyleColor();
        }

        if (!_tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip(_tooltip.c_str());

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
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            static unordered_set<string> doNotAskDialogTitles;

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

                    if (!item.label.empty() && (ImGui::Button(item.label.c_str(), ImVec2(fixedButtonWidth,0))) || (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)) )
                    {
                        if (item.action != nullptr)
                            close = item.action();
                        ImGui::CloseCurrentPopup();
                    }
                    if (!isLastButton)
                        ImGui::SameLine();
                }

                static bool doNotAsk = false;
                if (ImGui::Checkbox("Do not ask", &doNotAsk))
                    doNotAskDialogTitles.insert(g_ModalMsgBox.m_title);

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
        auto origin = ImGui::GetCursorPos();
        float y0 = ImGui::GetCursorScreenPos().y + (float)(int)item_offset_y;

        ImGuiListClipper clipper;
        clipper.Begin(row_count, line_height);  // Use the clipper with row count and line height
        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                ImU32 col = (row_n & 1) ? col_odd : col_even;
                if ((col & IM_COL32_A_MASK) == 0)
                    continue;
                float y1 = y0 + (line_height * row_n);
                float y2 = y1 + line_height;
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col);
            }
        }
        clipper.End();
        ImGui::SetCursorPos(origin);
    }

    //--------------------------------------------------------------------------------------
    string getObjectLabel(const string & _label, const void * _object)
    {
        return fmt::sprintf("%s###%s%p", _label.c_str(), _label.c_str(), _object);
    }

    //--------------------------------------------------------------------------------------
    string getObjectLabel(const string & _label, const string & _subLabel, const void * _object)
    {
        return fmt::sprintf("%s###%s_%p", _label.c_str(), _subLabel.c_str(), _object);
    }    

    //--------------------------------------------------------------------------------------
    vg::core::string getObjectPropertyLabel(const vg::core::IObject * _object, const vg::core::IProperty * _prop)
    {
        return fmt::sprintf("%s###%s_%u", _prop->GetDisplayName(), _prop->GetName(), (uint_ptr)_object->GetUID());
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
        auto scrollY = ImGui::GetScrollY();
        auto pos = ImVec2(ImGui::GetStyle().FramePadding.x, _headerPos.y);
        ImGui::SetCursorPos(pos);

        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 bgColor = style.Colors[ImGuiCol_Header];

        // Draw the rectangle
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddRectFilled(windowPos + ImVec2(6, 4 + pos.y - scrollY), windowPos + collapsedButtonSize + ImVec2(-3, -5 + pos.y - scrollY), ImGui::GetColorU32(bgColor), 0.0f, ImDrawListFlags_None); // 

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(bgColor.x, bgColor.y, bgColor.z, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Optional: Removes spacing between items

        ImGui::ButtonEx(ImGui::getObjectLabel(icon, "CheckBoxEnable", _object).c_str(), collapsedButtonSize, ImGuiItemFlags_AllowOverlap);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();

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
    bool CollapsingHeaderIconButton(const ImVec2 & _headerPos, float _availableWidth, IObject * _object, const char * _icon, const string & _tooltip, vg::core::uint _index, const ImVec4 * _buttonColor)
    {
        auto collapsedButtonSize = style::button::SizeSmall;
        const ImGuiStyle & style = ImGui::GetStyle();
        if (style.FramePadding.y == 3)
            collapsedButtonSize.y -= 1;

        ImGui::SameLine();

        ImGui::SetCursorPos(ImVec2(_availableWidth - (_index+1)*style::button::SizeSmall.x + style.FramePadding.x + 4, _headerPos.y));

        auto bgColor = style.Colors[ImGuiCol_Header];

        if (_buttonColor)
            ImGui::PushStyleColor(ImGuiCol_Text, *_buttonColor);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(bgColor.x, bgColor.y, bgColor.z, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
        ImGui::ButtonEx(ImGui::getObjectLabel(fmt::sprintf("%s", _icon).c_str(), _object).c_str(), collapsedButtonSize, ImGuiItemFlags_AllowOverlap);
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();

        if (_buttonColor)
            ImGui::PopStyleColor();

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
    void PushDefaultFont()
    {
        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        imGuiAdapter->PushDefaultFont();
    }
   
    //--------------------------------------------------------------------------------------
    void PushFont(Font _font)
    {
        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        imGuiAdapter->PushFont(_font);
    }

    //--------------------------------------------------------------------------------------
    void PushFont(vg::renderer::Font _font, vg::renderer::FontStyle _style)
    {
        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        imGuiAdapter->PushFont(_font, _style);
    }

    //--------------------------------------------------------------------------------------
    void PushStyle(FontStyle _style)
    {
        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        imGuiAdapter->PushFontStyle(_style);
    }

    //--------------------------------------------------------------------------------------
    void PopStyle()
    {
        auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
        imGuiAdapter->PopFontStyle();
    }

    static float g_backupAlpha;
    static vector<bool> g_disabledStack;

    //--------------------------------------------------------------------------------------
    void ApplyDisabledStyle(bool _disabled)
    {
        if (_disabled)
            GImGui->Style.Alpha = g_backupAlpha * GImGui->Style.DisabledAlpha;
        else
            GImGui->Style.Alpha = g_backupAlpha;
    }

    //--------------------------------------------------------------------------------------
    void PushDisabledStyle(bool _disabled)
    {
        if (g_disabledStack.size() == 0)
            g_backupAlpha = GImGui->Style.Alpha;

        ApplyDisabledStyle(_disabled);

        g_disabledStack.push_back(_disabled);
    }

    //--------------------------------------------------------------------------------------
    void PopDisabledStyle()
    {
        VG_ASSERT(g_disabledStack.size() > 0);
        if (g_disabledStack.size() > 0)
        {
            g_disabledStack.pop_back();
            bool disabled = g_disabledStack.size() > 0 && g_disabledStack.back();
            ApplyDisabledStyle(disabled);
        }
    }

    //--------------------------------------------------------------------------------------
    // A custom version of ImGui::DragScalarN that do not return 'true" when value is entered using keyboard
    //--------------------------------------------------------------------------------------
    bool CustomDragScalarN(InteractionType & _interactionType, const char * label, ImGuiDataType data_type, void * p_data, int components, float v_speed, const void * p_min, const void * p_max, const char * format, ImGuiSliderFlags flags)
    {
        auto * window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        _interactionType = InteractionType::Continuous;

        ImGuiContext & g = *GImGui;
        bool value_changed = false;
        BeginGroup();
        PushID(label);
        PushMultiItemsWidths(components, CalcItemWidth());
        size_t type_size = DataTypeGetInfo(data_type)->Size;
        for (int i = 0; i < components; i++)
        {
            PushID(i);
            if (i > 0)
                SameLine(0, g.Style.ItemInnerSpacing.x);
            value_changed |= DragScalar("", data_type, p_data, v_speed, p_min, p_max, format, flags);

            const ImGuiID id = ImGui::GetCurrentWindow()->GetID("");
            if (ImGui::TempInputIsActive(id))
                value_changed = false;

            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                value_changed = true;
                _interactionType = InteractionType::Single;
            }

            PopID();
            PopItemWidth();
            p_data = (void *)((char *)p_data + type_size);
        }
        PopID();

        const char * label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0, g.Style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }

        EndGroup();
        return value_changed;
    }

    //--------------------------------------------------------------------------------------
    bool CustomDragFloat3(InteractionType & _interactionType, const char * label, float v[3], float v_speed, float v_min, float v_max, const char * format, ImGuiSliderFlags flags)
    {
        return CustomDragScalarN(_interactionType, label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags);
    }

    //--------------------------------------------------------------------------------------
    bool CustomDragFloat4(InteractionType & _interactionType, const char * label, float v[4], float v_speed, float v_min, float v_max, const char * format, ImGuiSliderFlags flags)
    {
        return CustomDragScalarN(_interactionType, label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, flags);
    }
}