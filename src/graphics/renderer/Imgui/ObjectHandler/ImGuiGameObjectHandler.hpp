#pragma once

#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "graphics/renderer/Pass/ImguiPass/Menus/ImguiMenu.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    class ImguiGameObjectInspectorMenu : public ImguiMenu
    {
    public:
        enum MenuOption
        {
            None = 0,
            AddComponent
        };

        //--------------------------------------------------------------------------------------
        Status Display(IObject * _object) final
        {
            auto status = Status::None;

            IGameObject * gameObject = dynamic_cast<IGameObject *>(_object);
            VG_ASSERT(nullptr != gameObject);

            bool openPopup = false;

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Add Component"))
                {
                    m_selected = MenuOption::AddComponent;
                    m_popup = "Add Component";
                    openPopup = true;
                    ImGui::OpenPopup("Add Component");
                }

                ImGui::EndPopup();
            }

            if (openPopup)
            {
                ImGui::OpenPopup(m_popup);
                openPopup = false;
            }

            switch (m_selected)
            {
                case MenuOption::AddComponent:
                    if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        // List the available components
                        auto factory = Kernel::getFactory();
                        auto componentClassDescs = factory->getClassDescriptors(IClassDesc::Flags::Component);

                        if (ImGui::BeginCombo("Components", m_selectedClassName, ImGuiComboFlags_HeightLarge))
                        {
                            for (uint i = 0; i < componentClassDescs.count(); ++i)
                            {
                                auto * classDesc = componentClassDescs[i];
                                auto * className = classDesc->getClassName();
                                if (ImGui::Selectable(className))
                                    m_selectedClassName = className;
                            }
                            ImGui::EndCombo();
                        }

                        if (ImGui::Button("Create", Editor::ButtonSize))
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

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel", Editor::ButtonSize))
                        {
                            status = Status::Canceled;
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }
                    break;
            }

            return status;
        }

    private:
        const char * m_selectedClassName = nullptr;
    };

    class ImguiGameObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->getClassDescriptor(_object->getClassName());

            // Sort top-level properties to display components at the end
            const char * curClassName = nullptr;
            bool visible = false;

            m_gameObjectInspectorMenu.Display(_object);

            if (ImGui::CollapsingHeader("GameObject", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);
                
                    if (strcmp(prop->getName(), "m_components"))
                    {
                        if (curClassName != prop->getClassName())
                        {
                            curClassName = prop->getClassName();
                            visible = true; // ImGui::CollapsingHeader(curClassName, nullptr, ImGuiTreeNodeFlags_DefaultOpen);
                        }
                
                        if (visible)
                            ImguiEditor::displayProperty(prop, _object);
                    }
                }
            }

            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->getPropertyByIndex(i);
            
                if (!strcmp(prop->getName(), "m_components"))
                    ImguiEditor::displayProperty(prop, _object);
            }
        }

    private:
        ImguiGameObjectInspectorMenu m_gameObjectInspectorMenu;
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(GameObject, ImguiGameObjectHandler); 
}



