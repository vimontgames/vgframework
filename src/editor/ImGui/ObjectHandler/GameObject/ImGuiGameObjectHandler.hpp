#pragma once

#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "editor/ImGui/Menu/Inspector/GameObject/ImGuiGameObjectInspectorMenu.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiGameObjectHandler : public ImGuiObjectHandler
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

            ImGui::PushID("DisplayObject");

            bool open = ImGui::CollapsingHeader("GameObject", nullptr, ImGuiTreeNodeFlags_DefaultOpen);

            m_gameObjectInspectorMenu.Display(_object);

            if (open)
            {
                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);
                
                    if (strcmp(prop->getName(), "m_components"))
                    {
                        if (curClassName != prop->getClassName())
                        {
                            curClassName = prop->getClassName();
                            visible = true;
                        }
                
                        if (visible)
                        {
                            ImGuiWindow::displayProperty(prop, _object);
                        }
                    }
                }
            }

            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->getPropertyByIndex(i);
            
                if (!strcmp(prop->getName(), "m_components"))
                {
                    ImGuiWindow::displayProperty(prop, _object);
                }
            }

            ImGui::PopID();
        }

    private:
        ImGuiGameObjectInspectorMenu m_gameObjectInspectorMenu;
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(GameObject, ImGuiGameObjectHandler); 
}



