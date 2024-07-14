#include "ObjectHandleMenu.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    bool ImGuiObjectHandleMenu::SelectUID(core::UID * _uid)
    {
        bool changed = false;
        bool openPopup = false;

        if (ImGui::BeginPopupContextItem("ImGuiObjectRefMenu", ImGuiPopupFlags_MouseButtonLeft))
        {
            if (ImGui::MenuItem("Select"))
            {
                openPopup = true;
            }

            if (ImGui::MenuItem("Clear"))
            {
                *_uid = 0x0;
                changed = true;
            }

            ImGui::EndPopup();
        }

        if (openPopup)
        {
            ImGui::OpenPopup("Select Object");
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            openPopup = false;
        }

        if (ImGui::BeginPopupModal("Select Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                ImGui::SetKeyboardFocusHere(0);

            bool edited = false;
            UID temp = *_uid;
            if (ImGui::InputScalar("UID", ImGuiDataType_U32, &temp, nullptr, nullptr, "%08X", ImGuiInputTextFlags_EnterReturnsTrue))
                edited = true;

            auto * factory = Kernel::getFactory();
            //const auto & uidObjects = factory->GetUIDObjects();

            //if (ImGui::BeginListBox("Objects"))
            //{
            //    for (auto & pair : uidObjects)
            //    {
            //        const bool is_selected = false; // (item_current_idx == n);
            //
            //        IObject * obj = pair.second;
            //
            //        if (!obj->getName().empty())
            //        {
            //            if (ImGui::Selectable(obj->getName().c_str(), is_selected))
            //            {
            //                //    item_current_idx = n;
            //            }
            //        }
            //
            //        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            //        if (is_selected)
            //            ImGui::SetItemDefaultFocus();
            //    }
            //    ImGui::EndListBox();
            //}

            if (ImGui::Button("OK", style::button::SizeMedium))// || enterPressed)
            {
                *_uid = temp;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", style::button::SizeMedium))
            {
                ImGui::CloseCurrentPopup();
                changed = false;
            }

            ImGui::EndPopup();
        }

        return changed;
    }
}