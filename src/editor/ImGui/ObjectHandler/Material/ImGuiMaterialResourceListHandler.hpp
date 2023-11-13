#pragma once

#include "engine/Component/Mesh/MaterialResourceList.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiMaterialResourceListHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->getClassDescriptor(_object->getClassName());
            auto list = dynamic_cast<engine::IMaterialList *>(_object);

            ImGui::PushID(_object);
            string label = (string)"Materials###" + to_string((uint_ptr)_object);

            bool open = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Add Material"))
                {
                    list->AddMaterial();
                }
                if (ImGui::MenuItem("Remove Material"))
                {
                    list->RemoveMaterial();
                }
                ImGui::EndPopup();
            }

            if (open)
            {
                ImGui::Indent();
                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);

                    if (!strcmp(prop->getName(), "m_materialResources"))
                    {
                        uint count = prop->GetPropertyObjectVectorCount(_object);
                        //uint count = prop->getEnumCount();

                        for (uint i = 0; i < count; ++i)
                        {
                            ImGui::PushID(i);

                            IObject * obj = (IObject *)prop->GetPropertyObjectVectorData(_object);
                            //IObject * obj = prop->GetPropertyObject(_object, i);

                            string materialLabel = (string)"ID " + to_string(i) + "###" + to_string((uint_ptr)_object);
                            if (ImGui::TreeNodeEx(materialLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                ImGuiWindow::displayObject(obj);
                                ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }                     
                    }
                    else
                    {
                        ImGuiWindow::displayProperty(prop, _object);
                    }
                }

                ImGui::Spacing();

                string addMaterialLabel = "Add###" + to_string((uint_ptr)_object);
                string removeMaterialLabel = "Remove###" + to_string((uint_ptr)_object);
                if (ImGui::Button("Add Material"))
                {
                    list->AddMaterial();
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove Material"))
                {
                    list->RemoveMaterial();
                }

                ImGui::Unindent();
            }
            ImGui::PopID();

            /*
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

            ImGui::PopID();*/
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(MaterialResourceList, ImGuiMaterialResourceListHandler);
}



