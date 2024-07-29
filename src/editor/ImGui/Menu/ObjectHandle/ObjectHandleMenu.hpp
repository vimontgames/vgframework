#include "ObjectHandleMenu.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    bool ImGuiObjectHandleMenu::SelectUID(core::UID * _uid, IGameObject * _gameobject)
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

        static UID editTemp = 0x0;

        if (openPopup)
        {
            ImGui::OpenPopup("Select object to reference");
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            openPopup = false;
            editTemp = *_uid;
        }

        if (ImGui::BeginPopupModal("Select object to reference", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                ImGui::SetKeyboardFocusHere(0);

            bool edited = false;
           
            if (ImGui::InputScalar("UID", ImGuiDataType_U32, &editTemp, nullptr, nullptr, "%08X", ImGuiInputTextFlags_EnterReturnsTrue))
                edited = true;

            auto * factory = Kernel::getFactory();
            const auto & uidObjects = factory->GetUIDObjects();

            if (ImGui::BeginListBox("Objects"))
            {
                vector<IObject*> sortedObjects;

                for (auto & pair : uidObjects)
                {
                    IObject * obj = pair.second;

                    if (!obj->getName().empty())
                    {
                        const IGameObject * go = dynamic_cast<IGameObject *>(obj);
                        const bool isGameObject = go != nullptr;

                        if (isGameObject)
                        {
                            const bool isPrefabChild = nullptr != go->GetParentPrefab();

                            if (isPrefabChild)
                                continue;

                            //if (go->IsPrefab())
                            //    continue;

                            if (_gameobject->GetScene()->GetSceneType() == BaseSceneType::Scene)
                            {
                                if (go->GetScene()->GetSceneType() == BaseSceneType::Prefab)
                                    continue;
                            }

                            if (_gameobject->GetWorld() != go->GetWorld())
                                continue;

                            sortedObjects.push_back(obj);
                        }
                    }
                }

                sort(sortedObjects.begin(), sortedObjects.end(), [](IObject * a, IObject * b)
                {
                    return a->getName() < b->getName();
                }
                );

                for (uint i = 0; i < sortedObjects.size(); ++i)
                {
                    auto & obj = sortedObjects[i];

                    const bool is_selected = obj->GetUID() == editTemp;
                    const IGameObject * go = dynamic_cast<IGameObject *>(obj);
                                         
                    if (ImGui::Selectable(fmt::sprintf("%s###%u", obj->getName(), obj->GetUID()).c_str(), is_selected))
                        editTemp = obj->GetUID();

                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s\n0x%08X", go->GetFullName().c_str(), go->GetUID());

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();

                ImGui::Text("%u GameObject(s)", sortedObjects.size());
            }

            if (ImGui::Button("OK", style::button::SizeMedium))// || enterPressed)
            {
                *_uid = editTemp;
                ImGui::CloseCurrentPopup();
                changed = true;
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