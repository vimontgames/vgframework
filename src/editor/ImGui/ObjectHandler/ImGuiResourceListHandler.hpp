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
        bool displayObject(IObject * _object, ObjectContext & _objectContext) = 0;

        //--------------------------------------------------------------------------------------
        bool displayResourceList(IObject * _object, const core::string & _label, const core::string & _vectorPropName)
        {
            bool changed = false;

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->getClassDescriptor(_object->GetClassName());
            auto list = dynamic_cast<engine::IResourceList *>(_object);
            auto availableWidth = ImGui::GetContentRegionAvail().x;

            uint resourceCount = 0;
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    resourceCount = prop->GetPropertyResourceVectorCount(_object);
            }

            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

            string label = fmt::sprintf("%ss (%u)", _label.c_str(), resourceCount);

            ImGuiStyle & style = ImGui::GetStyle();

            ImGui::PushStyleColor(ImGuiCol_Header, style.Colors[ImGuiCol_WindowBg]);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, style.Colors[ImGuiCol_ButtonActive]);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style.Colors[ImGuiCol_ButtonHovered]);
            ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Header]);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_HeaderActive]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_HeaderHovered]);

            ImGui::PushID("CollapsingHeader");
            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", _object).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
            ImGui::PopID();

            ImGui::BeginDisabled(false);
            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, label.c_str(), true);            
            ImGui::EndDisabled();

            bool remove = false;

            if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Plus, fmt::sprintf("Add %s", _label), 0))
                list->Add();

            ImGui::BeginDisabled(list->Size() == 0);
            if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Minus, fmt::sprintf("Remove %s", _label), 1))
                remove = true;    
            ImGui::EndDisabled();

            ImGui::PopStyleColor(6);

            if (open)
            {
                ImGui::Indent();

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);

                    if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    {
                        PropertyContext propContext(_object, prop);

                        resourceCount = prop->GetPropertyResourceVectorCount(_object);

                        for (uint i = 0; i < resourceCount; ++i)
                        {
                            ImGui::PushID(i);
                            auto obj = prop->GetPropertyResourceVectorElement(_object, i);

                            string itemLabel = fmt::sprintf("[%u] %s", i, obj->GetResourcePath().empty() ? "Empty" : io::getFileName(obj->GetResourcePath()));
                            itemLabel += "###" + to_string((uint_ptr)obj);

                            if (ImGui::TreeNodeEx(itemLabel.c_str(), ImGuiTreeNodeFlags_None))
                            {
                                changed |= ImGuiWindow::displayResource(obj, prop, i, propContext);
                                ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }
                    }
                    else
                    {
                        changed |= ImGuiWindow::displayProperty(_object, prop);
                    }
                }

                ImGui::Spacing();

                ImGui::Unindent();
            }

            if (remove)
                list->Remove();

            return changed;
        }        
    };
}



