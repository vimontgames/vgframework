#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "core/ISelection.h"
#include "core/string/string.h"
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

        ISelection * selection = Editor::get()->getSelection();
        const auto & selectedObjects = selection->GetSelectedObjects();

        // Build a list of GameObjects to be deleted
        vector<IGameObject *> gameObjectsToDelete;
        {
            if (!gameObject->IsRoot())
                gameObjectsToDelete.push_back(gameObject);

            for (uint i = 0; i < selectedObjects.size(); ++i)
            {
                IObject * obj = selectedObjects[i];
                IGameObject * go = dynamic_cast<IGameObject *>(obj);
                if (go && !go->IsRoot() && go != gameObject)
                    gameObjectsToDelete.push_back(go);
            }

            // Remove objects with parents already in the list
            RESTART:
            for (uint i = 0; i < gameObjectsToDelete.size(); ++i)
            {
                IGameObject * goi = gameObjectsToDelete[i];

                for (uint j = 0; j < gameObjectsToDelete.size(); ++j)
                {
                    IGameObject * goj = gameObjectsToDelete[j];

                    if (i != j)
                    {
                        if (goj->HasAncestor(goi))
                        {
                            gameObjectsToDelete.remove(goj);
                            goto RESTART;
                        }
                    }
                }
            }
        }

        // Root GameObject cannot be deleted
        const bool canDelete = gameObjectsToDelete.size() > 0;

        //if (Kernel::getInput()->IsKeyPressed(Key::DEL))
        if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            doDelete = true;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Add GameObject"))
            {
                m_selected = MenuOption::AddChild;
                m_popup = "Add GameObject";
                openPopup = true;
                ImGui::OpenPopup("Add GameObject");
            }

            ImGui::BeginDisabled(!canDelete);
            {
                if (ImGui::MenuItem("Delete GameObject"))
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
                        VG_SAFE_RELEASE(gameObjectToDelete);
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