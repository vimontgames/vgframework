#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "core/ISelection.h"
#include "core/IInput.h"
#include "engine/IEngine.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/Editor.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiGameObjectSceneEditorMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IGameObject * gameObject = dynamic_cast<IGameObject*>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;
        bool doDelete = false;
        bool doRename = false;

        ISelection * selection = Editor::get()->getSelection();

        // The copy here is *intentional*
        auto selectedObjects = selection->GetSelectedObjects(); 

        // Add the clicked object to "selection" if not already selected
        if (!gameObject->IsRoot() && !selection->IsSelectedObject(gameObject))
            selectedObjects.push_back(gameObject);

        vector<IGameObject *> gameObjectsToDelete = Editor::get()->getSelection()->RemoveChildGameObjectsWithParents(selectedObjects);

        // Root GameObject cannot be deleted
        const bool canDelete = gameObjectsToDelete.size() > 0;
        const bool canRename = selection->GetSelectedObjects().size() <= 1;

        if (m_RenamingGameObject == nullptr)
        {
            if (selection->IsSelectedObject(gameObject))
            {
                if (ImGui::IsKeyPressed(ImGuiKey_Delete))
                    doDelete = true;
                else if (canRename && ImGui::IsKeyPressed(ImGuiKey_R) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                    doRename = true;
            }
        }

        if (ImGui::BeginPopupContextItem())
        {
            // if menu called on non-selected object then it becomes the selection
            if (!selection->IsSelectedObject(_object))
                selection->SetSelectedObject(_object);

            if (ImGui::MenuItem("Add GameObject"))
            {
                m_selected = MenuOption::AddChild;
                m_popup = "Add GameObject";
                openPopup = true;
                ImGui::OpenPopup("Add Child");
            }

            ImGui::BeginDisabled(!canRename);
            {
                if (ImGui::MenuItem("Rename", "Ctrl-R"))
                    doRename = true;
            }
            ImGui::EndDisabled();

            ImGui::BeginDisabled(!canDelete);
            {
                if (ImGui::MenuItem("Delete", "Del"))
                    doDelete = true;
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();            
        }

        if (doDelete)
        {
            ImGui::OnMsgBoxClickedFunc deleteGameObject = [=]() mutable
            {
                for (uint i = 0; i < gameObjectsToDelete.size(); ++i)
                {
                    IGameObject * gameObjectToDelete = gameObjectsToDelete[i];
                    IGameObject * parentGameObject = dynamic_cast<IGameObject *>(gameObjectToDelete->getParent());
                    if (nullptr != parentGameObject)
                    {
                        parentGameObject->RemoveChild(gameObjectToDelete);
                        status = Status::Removed;
                    }
                }
                return true;
            };

            string msg;
            if (gameObjectsToDelete.size() > 1)
            {
                msg = "Are you sure you want to delete " + to_string(gameObjectsToDelete.size()) + " GameObjects and their children?";
            }
            else
            { 
                msg = "Are you sure you want to delete " + (string)gameObject->getClassName() + " \"" + gameObject->getName() + "\"?";
            }
            ImGui::MessageBox(MessageBoxType::YesNo, "Delete GameObject", msg.c_str(), deleteGameObject);
        }

        if (doRename)
            m_RenamingGameObject = gameObject;

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

                    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                        ImGui::SetKeyboardFocusHere(0);

                    bool enterPressed = ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

                    string newName = nameTmp;          

                    if (ImGui::Button("Add", style::button::SizeMedium) || enterPressed)
                    {
                        IGameObject * newGameObject = (IGameObject*)CreateFactoryObject(GameObject, newName.c_str(), gameObject);
                        gameObject->AddChild(newGameObject);
                        newGameObject->Release();
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", style::button::SizeMedium))
                    {
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }
                    ImGui::EndPopup();
                }
            }
            break;
        }
        return status;
    }
}