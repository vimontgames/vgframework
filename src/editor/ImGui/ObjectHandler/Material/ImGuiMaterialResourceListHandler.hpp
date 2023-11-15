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

            uint materialCount = 0;
            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->getPropertyByIndex(i);
                if (!strcmp(prop->getName(), "m_materialResources"))
                    materialCount = prop->GetPropertyResourceVectorCount(_object);
            }

            ImGui::PushID(_object);
            string label = (string)"Materials (" + to_string(materialCount) + (string)")###" + to_string((uint_ptr)_object);

            bool open = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            bool remove = false;

            if (open)
            {
                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);

                    if (!strcmp(prop->getName(), "m_materialResources"))
                    {
                        materialCount = prop->GetPropertyResourceVectorCount(_object);
                        const uint sizeOf = prop->getSizeOf();

                        for (uint i = 0; i < materialCount; ++i)
                        {
                            ImGui::PushID(i);
                            auto obj = (IResource *)(prop->GetPropertyResourceVectorData(_object) + i * sizeOf);

                            string materialLabel = (string)"MatID " + to_string(i) + "###" + to_string((uint_ptr)_object);
                            if (ImGui::TreeNodeEx(materialLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                ImGuiWindow::displayResource(obj);
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

                ImGui::BeginDisabled(materialCount == 0);
                {
                    if (ImGui::Button("Remove Material"))
                    {
                        // Can't remove while iterating the list ;)
                        remove = true;
                    }
                }
                ImGui::EndDisabled();

                ImGui::TreePop();
            }
            ImGui::PopID();

            if (remove)
                list->RemoveMaterial();
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(MaterialResourceList, ImGuiMaterialResourceListHandler);
}



