#include "editor/ImGui/Menu/Inspector/GameObject/ImGuiGameObjectInspectorMenu.h"
#include "core/IComponent.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiGameObjectInspectorMenu::Display(IObject * _object)
    {
        auto status = Status::None;

        IGameObject * gameObject = dynamic_cast<IGameObject *>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;   

        ImGui::PushID("ImGuiGameObjectInspectorMenu");

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Add Component"))
            {
                m_selected = MenuOption::AddComponent;
                m_popup = "Add Component";
                openPopup = true;
                ImGui::OpenPopup(m_popup);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup);
            openPopup = false;
        }

        switch (m_selected)
        {
            case MenuOption::AddComponent:
            {
                if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    // List the available components
                    auto factory = Kernel::getFactory();
                    auto componentClassDescs = factory->getClassDescriptors(IClassDesc::Flags::Component);

                    if (ImGui::BeginCombo("Component Type", m_selectedClassName, ImGuiComboFlags_HeightLarge))
                    {
                        for (uint i = 0; i < componentClassDescs.count(); ++i)
                        {
                            auto * classDesc = componentClassDescs[i];
                            auto * className = classDesc->GetClassName();
                            if (ImGui::Selectable(className))
                                m_selectedClassName = className;
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::BeginDisabled(m_selectedClassName == nullptr);
                    {
                        if (ImGui::Button("Create", style::button::SizeMedium))
                        {
                            status = Status::Failure;

                            if (m_selectedClassName)
                            {
                                IComponent * comp = (IComponent *)Kernel::getFactory()->createObject(m_selectedClassName, "New " + (string)m_selectedClassName, _object);
                                if (comp)
                                {
                                    gameObject->AddComponent(comp);
                                    comp->release();
                                    status = Status::Success;
                                }
                            }

                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndDisabled();

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", style::button::SizeMedium))
                    {
                        status = Status::Canceled;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }
            break;
        }

        return status;
    }
}