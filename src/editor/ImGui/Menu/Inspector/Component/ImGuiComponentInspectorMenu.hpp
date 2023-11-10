#include "editor/ImGui/Menu/Inspector/Component/ImGuiComponentInspectorMenu.h"
#include "core/IComponent.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiComponentInspectorMenu::Display(IObject * _object)
    {
        auto status = Status::None;

        IComponent * component = dynamic_cast<IComponent *>(_object);
        VG_ASSERT(nullptr != component);

        bool openPopup = false;

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushID("ImGuiComponentInspectorMenu");
            if (ImGui::MenuItem("Remove Component"))
            {
                m_selected = MenuOption::RemoveComponent;
                m_popup = "Remove Component";
                openPopup = true;
                ImGui::OpenPopup(m_popup);
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup);
            openPopup = false;
        }

        switch (m_selected)
        {
            case MenuOption::RemoveComponent:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::OnMsgBoxClickedFunc deleteComponent = [=]() mutable
                        {
                            IGameObject * go = dynamic_cast<IGameObject *>(component->getParent());
                            if (nullptr != go)
                            {
                                go->RemoveComponent(component);
                                VG_SAFE_RELEASE(component);
                                status = Status::Removed;
                            }
                            return true;
                        };

                    string msg = "Are you sure you want to delete " + (string)component->getClassName() + " \"" + component->getName() + "\"?";
                    ImGui::MessageBox(MessageBoxType::YesNo, "Delete Component", msg.c_str(), deleteComponent);

                    ImGui::EndPopup();
                }
            }
            break;
        }

        return status;
    }
}