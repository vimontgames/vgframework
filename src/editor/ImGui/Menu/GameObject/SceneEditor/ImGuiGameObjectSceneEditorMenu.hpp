#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "core/string/string.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"

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
                ImGui::OnMsgBoxClickedFunc deleteGameObject = [=]() mutable
                {
                    IGameObject * parentGameObject = (IGameObject *)gameObject->getParent();
                    if (nullptr != parentGameObject)
                    {
                        ImGuiWindow::removeFromSelection(gameObject);
                        parentGameObject->RemoveChild(gameObject);
                        VG_SAFE_RELEASE(gameObject);
                        status = Status::Removed;
                    }
                    return true;
                };

                string msg = "Are you sure you want to delete " + (string)gameObject->getClassName() + " \"" + gameObject->getName() + "\"";
                ImGui::MessageBox(MessageBoxType::YesNo, "Delete GameObject", msg.c_str(), deleteGameObject);
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

                    if (ImGui::Button("Add", style::button::Size))
                    {
                        IGameObject * newGameObject = (IGameObject*)CreateFactoryObject(GameObject, newName.c_str(), gameObject);
                        gameObject->AddChild(newGameObject);
                        newGameObject->Release();
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = MenuOption::None;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", style::button::Size))
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