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
        bool deleteComponent = m_componentToDelete && m_componentToDelete == _object;

        //if (ImGui::BeginPopupContextItem())
        //{
        //    ImGui::PushID("ImGuiComponentInspectorMenu");
        //    if (ImGui::MenuItem("Delete Component"))
        //        deleteComponent = true;
        //
        //    ImGui::PopID();
        //    ImGui::EndPopup();
        //}

        if (deleteComponent)
        {
            m_selected = (uint)InspectorMenuOption::RemoveComponent;
            m_popup = "Delete Component";
            openPopup = true;
            ImGui::OpenPopup(m_popup.c_str());
            m_componentToDelete = nullptr;
            m_selectedObject = _object;
        }

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup.c_str());
            openPopup = false;
        }

        if (_object == m_selectedObject)
        {
            switch ((InspectorMenuOption)m_selected)
            {
                case InspectorMenuOption::RemoveComponent:
                {
                    if (ImGui::BeginPopupModal(m_popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::OnMsgBoxClickedFunc deleteComponent = [=]() mutable
                            {
                                IGameObject * go = dynamic_cast<IGameObject *>(component->GetParent());
                                if (nullptr != go)
                                {
                                    go->RemoveComponent(component);
                                    VG_SAFE_RELEASE(component);
                                    status = Status::Removed;
                                }
                                return true;
                            };

                        string msg = fmt::sprintf("Are you sure you want to delete %s component from \"%s\"?", component->GetClassName(), component->GetParent()->getName());
                        ImGui::MessageBox(MessageBoxType::YesNo, "Delete Component", msg.c_str(), deleteComponent);

                        ImGui::EndPopup();
                    }
                }
                break;
            }
        }

        return status;
    }
}