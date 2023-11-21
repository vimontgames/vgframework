#pragma once

#include "core/File/File.h"
#include "core/IResource.h"
#include "engine/IResourceList.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiResourceListHandler : public ImGuiObjectHandler
    {
    public:
        void displayObject(IObject * _object) = 0;

        //--------------------------------------------------------------------------------------
        void displayResourceList(IObject * _object, const core::string & _label, const core::string & _vectorPropName)
        {
            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->getClassDescriptor(_object->getClassName());
            auto list = dynamic_cast<engine::IResourceList *>(_object);

            uint materialCount = 0;
            for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->getPropertyByIndex(i);
                if (!strcmp(prop->getName(), _vectorPropName.c_str()))
                    materialCount = prop->GetPropertyResourceVectorCount(_object);
            }

            ImGui::PushID(_object);
            string label = _label + (string)"s (" + to_string(materialCount) + (string)")###" + to_string((uint_ptr)_object);

            bool open = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            bool remove = false;

            if (open)
            {
                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);

                    if (!strcmp(prop->getName(), _vectorPropName.c_str()))
                    {
                        materialCount = prop->GetPropertyResourceVectorCount(_object);

                        for (uint i = 0; i < materialCount; ++i)
                        {
                            ImGui::PushID(i);
                            auto obj = prop->GetPropertyResourceVectorElement(_object, i);

                            string itemLabel = _label + (string)" " + to_string(i);
                            string itemName = obj->GetResourcePath();
                            if (!itemName.empty())
                                itemLabel += (string)" (" + io::getFileName(itemName) + (string)")";
                            itemLabel += "###" + to_string((uint_ptr)obj);

                            if (ImGui::TreeNodeEx(itemLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
                if (ImGui::Button(((string)"Add " + _label).c_str()))
                    list->Add();

                ImGui::SameLine();

                ImGui::BeginDisabled(materialCount == 0);
                {
                    if (ImGui::Button(((string)"Remove " + _label).c_str()))
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
                list->Remove();
        }
    };
}



