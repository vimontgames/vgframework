#include "ObjectHandleMenu.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiObjectHandleMenu::~ImGuiObjectHandleMenu()
    {
        m_filter.Clear();
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiObjectHandleMenu::SelectUID(core::UID * _uid, IGameObject * _gameobject, bool _open)
    {
        bool changed = false;
        bool openPopup = _open;

        if (!openPopup)
        {
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
        }

        static UID editTemp = 0x0;
        const char * selectObjectTitle = "Select GameObject";

        if (openPopup)
        {
            ImGui::OpenPopup(selectObjectTitle);
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            openPopup = false;
            editTemp = *_uid;
        }

        if (ImGui::BeginPopupModal(selectObjectTitle, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                ImGui::SetKeyboardFocusHere(0);

            bool edited = false;


            m_filter.Draw("Filter");
           
            if (ImGui::InputScalar("UID", ImGuiDataType_U32, &editTemp, nullptr, nullptr, "%08X", ImGuiInputTextFlags_EnterReturnsTrue))
                edited = true;

            auto * factory = Kernel::getFactory();
            const auto & uidObjects = factory->GetUIDObjects();

            uint gameObjectCount = 0;

            if (ImGui::BeginListBox("GameObjects"))
            {
                vector<IObject*> sortedObjects;

                for (auto & pair : uidObjects)
                {
                    IObject * obj = pair.second;                    

                    const IGameObject * go = dynamic_cast<IGameObject *>(obj);
                    const bool isGameObject = go != nullptr;

                    if (isGameObject)
                    {
                        gameObjectCount++;

                        if (go->GetName().empty())
                            continue;

                        const bool isPrefabChild = nullptr != go->GetParentPrefab();

                        if (isPrefabChild)
                            continue;

                        if (_gameobject->GetScene()->GetSceneType() == BaseSceneType::Scene)
                        {
                            if (go->GetScene()->GetSceneType() == BaseSceneType::Prefab)
                                continue;
                        }

                        if (_gameobject->GetWorld() != go->GetWorld())
                            continue;

                        if (!m_filter.PassFilter(go->GetName().c_str()))
                            continue;

                        sortedObjects.push_back(obj);
                    }
                }

                sort(sortedObjects.begin(), sortedObjects.end(), [](IObject * a, IObject * b)
                {
                    return a->GetName() < b->GetName();
                }
                );

                for (uint i = 0; i < sortedObjects.size(); ++i)
                {
                    auto & obj = sortedObjects[i];

                    const bool is_selected = obj->GetUID() == editTemp;
                    const IGameObject * go = dynamic_cast<IGameObject *>(obj);
                                         
                    if (ImGui::Selectable(fmt::sprintf("%s###%u", obj->GetName(), obj->GetUID()).c_str(), is_selected))
                        editTemp = obj->GetUID();

                    if (ImGui::IsItemHovered())
                    {
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            *_uid = editTemp;
                            ImGui::CloseCurrentPopup();
                            changed = true;
                        }

                        ImGui::SetTooltip("%s\n0x%08X", go->GetFullName().c_str(), go->GetUID());
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();

                ImGui::Text("%u/%u GameObject%s", sortedObjects.size(), gameObjectCount, sortedObjects.size() > 1 ? "s" : "");
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