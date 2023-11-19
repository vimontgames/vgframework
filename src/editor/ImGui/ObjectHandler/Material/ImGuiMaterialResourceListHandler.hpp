#pragma once

#include "engine/Component/Mesh/MaterialResourceList.h"
#include "core/File/File.h"

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

                        for (uint i = 0; i < materialCount; ++i)
                        {
                            ImGui::PushID(i);
                            auto obj = prop->GetPropertyResourceVectorElement(_object, i);

                            // Edit material name
                            //ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);
                            //if (1)
                            //{
                            //    char buffer[1024];
                            //    strcpy(buffer, obj->getName().c_str());
                            //    if (ImGui::InputText("Name", buffer, countof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
                            //        obj->setName(buffer);
                            //}
                            //ImGui::PopItemWidth();                               

                            string materialLabel = (string)"ID " + to_string(i);

                            string materialName = obj->GetResourcePath();
                            if (!materialName.empty())
                                materialLabel += (string)" (" + io::getFileName(materialName) + (string)")";

                            materialLabel +="###" + to_string((uint_ptr)obj);
                            
                            //string materialLabel = materialName + "###" + to_string((uint_ptr)_object);
                            if (ImGui::TreeNodeEx(materialLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                ImGuiWindow::displayResource(obj, prop, i);
                                ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }                     
                    }
                    else
                    {
                        ImGuiWindow::displayProperty(_object, prop);
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



