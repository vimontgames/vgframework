#pragma once

#include "core/GameObject/GameObject.h"

namespace vg::graphics::renderer
{
    using namespace vg::core;

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
                {
                    ImguiEditor::displayProperty(prop, _object);
            
                    ImGui::Spacing();
                    if (ImGui::Button("Add Component"))
                    {
                        // TODO
                    }
                }
            }
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(GameObject, ImguiGameObjectHandler); 
}



