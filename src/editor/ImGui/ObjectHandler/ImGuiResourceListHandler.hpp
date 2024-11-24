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
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) = 0;

        //--------------------------------------------------------------------------------------
        bool displayResourceList(IObject * _object, const core::string & _label, const core::string & _vectorPropName, const PropertyContext * _propContext)
        {
            ImGuiStyle & style = ImGui::GetStyle();
            bool changed = false;

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->GetClassDescriptor(_object->GetClassName());
            auto list = dynamic_cast<engine::IResourceList *>(_object);
            auto availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x *2 +1;

            uint resourceCount = 0;
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    resourceCount = prop->GetPropertyResourceVectorCount(_object);
            }

            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

            string label = fmt::sprintf("%ss (%u)", _label.c_str(), resourceCount);

            //ImGui::PushStyleColor(ImGuiCol_Header, style.Colors[ImGuiCol_WindowBg]);
            //ImGui::PushStyleColor(ImGuiCol_HeaderActive, style.Colors[ImGuiCol_ButtonActive]);
            //ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style.Colors[ImGuiCol_ButtonHovered]);
            //ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Header]);
            //ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_HeaderActive]);
            //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_HeaderHovered]);

            ImGui::PushID("CollapsingHeader");
            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", _object).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
            ImGui::PopID();

            const bool readOnly = _propContext && _propContext->m_readOnly;

            if (readOnly)
                ImGui::PushDisabledStyle(true);

            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, label.c_str(), true);            
            
            if (open)
                ImGui::Indent();

            bool remove = false;

            ImGui::BeginDisabled(readOnly);
            {
                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Plus, fmt::sprintf("Add %s", _label), 0))
                    list->Add();

                ImGui::BeginDisabled(list->Size() == 0);
                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Minus, fmt::sprintf("Remove %s", _label), 1))
                    remove = true;
                ImGui::EndDisabled();
            }
            ImGui::EndDisabled();

            if (readOnly)
                ImGui::PopDisabledStyle();

            //ImGui::PopStyleColor(6);

            if (open)
            {
                //ImGui::Indent();

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);

                    if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    {
                        PropertyContext propContext(_object, prop);

                        resourceCount = prop->GetPropertyResourceVectorCount(_object);

                        int removeAt = -1;

                        for (uint i = 0; i < resourceCount; ++i)
                        {
                            collapsingHeaderPos.y = ImGui::GetCursorPos().y;
                            availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x;

                            ImGui::PushID(i);
                            auto obj = prop->GetPropertyResourceVectorElement(_object, i);

                            string itemLabel;

                            string itemPath = io::getFileName(obj->GetResourcePath());

                            if (!obj->GetName().empty())
                                itemLabel = obj->GetName();
                            else
                                itemLabel = itemPath;

                            if (itemPath.empty())
                                ImGui::PushDisabledStyle(true);       

                            bool open = ImGui::PersistentCollapsingHeader(itemLabel, obj, prop);

                            if (itemPath.empty())
                                ImGui::PopDisabledStyle();

                            ImGui::BeginDisabled(readOnly);
                            {
                                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth - 1, _object, style::icon::Trashcan, fmt::sprintf("Remove element %u", i)))
                                    removeAt = i;
                            }
                            ImGui::EndDisabled();

                            if (open)
                            {
                                changed |= ImGuiWindow::displayResource(obj, prop, i, propContext);
                                //ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }

                        if (removeAt >= 0)
                        {
                            list->RemoveAt(removeAt);
                            changed = true;
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
                list->Pop();

            return changed;
        }        
    };
}



