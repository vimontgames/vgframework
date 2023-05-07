#include "ImguiGameObjectMenu.h"
#include "core/IGameObject.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImguiMenu::Status ImguiGameObjectMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IGameObject * gameObject = dynamic_cast<IGameObject*>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushID("GameObjectMenu");
            if (ImGui::MenuItem("Add Child GameObject"))
            {
                m_selected = MenuOption::AddChild;
                m_popup = "Add Child GameObject";
                openPopup = true;
                ImGui::OpenPopup("Add Child GameObject");
            }
            if (ImGui::MenuItem("Delete GameObject"))
            {
                m_selected = MenuOption::Delete;
                m_popup = "Delete GameObject";
                openPopup = true;
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup);
            openPopup = false;
        }
       
        switch ((MenuOption)m_selected)
        {
            case MenuOption::AddChild:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char nameTmp[1024] = { '\0' };

                    if (nameTmp[0] == '\0')
                        strcpy(nameTmp, "New GameObject");

                    ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll);

                    string newName = nameTmp;

                    if (ImGui::Button("Add", ImVec2(80, 0)))
                    {
                        IGameObject * newGameObject = (IGameObject*)CreateFactoryObject(GameObject, newName.c_str(), gameObject);
                        gameObject->AddChild(newGameObject);
                        newGameObject->Release();
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", ImVec2(80, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }
                    ImGui::EndPopup();
                }
            }
            break;

            case MenuOption::Delete:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to delete \"%s\?", gameObject->getName().c_str());

                    if (ImGui::Button("Yes", ImVec2(80, 0)))
                    {
                        IGameObject * parentGameObject = (IGameObject*)gameObject->getParent();
                        if (nullptr != parentGameObject)
                        {
                            parentGameObject->RemoveChild(gameObject);
                            status = Status::Removed;
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("No", ImVec2(80, 0)))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
            break;
        }
        return status;
    }
}