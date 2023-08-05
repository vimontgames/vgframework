#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "renderer/Imgui/Editors/ImGuiEditor.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ImguiMenu::Status ImguiGameObjectSceneEditorMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IGameObject * gameObject = dynamic_cast<IGameObject*>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushID("GameObjectInspectorMenu");
            if (ImGui::MenuItem("Add GameObject"))
            {
                m_selected = MenuOption::AddChild;
                m_popup = "Add GameObject";
                openPopup = true;
                ImGui::OpenPopup("Add GameObject");
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

                    if (ImGui::Button("Add", Editor::ButtonSize))
                    {
                        IGameObject * newGameObject = (IGameObject*)CreateFactoryObject(GameObject, newName.c_str(), gameObject);
                        gameObject->AddChild(newGameObject);
                        newGameObject->Release();
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", Editor::ButtonSize))
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
                    ImGui::Text("Are you sure you want to delete %s \"%s\"?", gameObject->getClassName(), gameObject->getName().c_str());

                    if (ImGui::Button("Yes", Editor::ButtonSize))
                    {
                        IGameObject * parentGameObject = (IGameObject*)gameObject->getParent();
                        if (nullptr != parentGameObject)
                        {
                            // Unselect if currently selected
                            ImguiEditor::removeFromSelection(gameObject);
                            parentGameObject->RemoveChild(gameObject);
                            VG_SAFE_RELEASE(gameObject);
                            status = Status::Removed;
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("No", Editor::ButtonSize))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
            break;
        }
        return status;
    }
}