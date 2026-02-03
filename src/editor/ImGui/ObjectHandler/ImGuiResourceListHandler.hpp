#pragma once
#include "core/File/File.h"
#include "core/IResource.h"
#include "core/IResourceList.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiResourceListHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext)
        {
            return displayResourceList(_object, _propContext->m_originalProp->GetDisplayName(), "m_resources", _propContext);
        }

        //--------------------------------------------------------------------------------------
        bool displayResourceList(IObject * _object, const core::string & _label, const core::string & _vectorPropName, const PropertyContext * _propContext)
        {
            string label = removeTrailingChar(_label, 's');
                   label = removeStartingIcon(label);

            ImGuiStyle & style = ImGui::GetStyle();
            bool changed = false;

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->GetClassDescriptor(_object->GetClassName());
            auto list = dynamic_cast<IResourceList *>(_object);
            VG_ASSERT(list, "[Editor] Object is not a Resource list");
            if (nullptr == list)
                return false;

            auto availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x *2 +1;

            uint resourceCount = 0;
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    resourceCount = prop->GetPropertyResourceVectorCount(_object);
            }

            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

            string headerLabel = fmt::sprintf("%s (%u)", _label.c_str(), resourceCount);

            ImGui::PushID("CollapsingHeader");
            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", _object).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
            ImGui::PopID();

            const bool readOnly = _propContext && _propContext->m_readOnly;

            if (readOnly)
                ImGui::PushDisabledStyle(true);

            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, headerLabel.c_str(), true);
            
            if (open)
                ImGui::Indent();

            bool remove = false;

            ImGui::BeginDisabled(readOnly || !list->CanAdd());
            {
                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Plus, fmt::sprintf("Add %s", label), 0))
                {
                    list->Add(fmt::sprintf("new %s", label));
                    changed = true;
                }

                ImGui::BeginDisabled(readOnly || list->Size() == 0 || !list->CanRemove());
                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Minus, fmt::sprintf("Remove %s", label), 1))
                    remove = true;
                ImGui::EndDisabled();
            }
            ImGui::EndDisabled();

            if (readOnly)
                ImGui::PopDisabledStyle();

            if (open)
            {
                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);

                    if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    {
                        PropertyContext propContext(_object, prop);

                        resourceCount = propContext.m_originalProp->GetPropertyResourceVectorCount(propContext.m_originalObject);

                        int removeAt = -1;
                        int moveUpAt = -1;
                        int moveDownAt = -1;

                        for (uint i = 0; i < resourceCount; ++i)
                        {
                            collapsingHeaderPos.y = ImGui::GetCursorPos().y;
                            availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x;

                            ImGui::PushID(i);
                            auto resource = propContext.m_originalProp->GetPropertyResourceVectorElement(propContext.m_originalObject, i);

                            string itemLabel;

                            string itemPath = io::getFileName(resource->GetResourcePath());

                            if (!resource->GetName().empty())
                                itemLabel = resource->GetName();
                            else
                                itemLabel = itemPath;

                            if (itemPath.empty())
                                ImGui::PushDisabledStyle(true);       

                            bool open = ImGui::PersistentCollapsingHeader(itemLabel, resource, prop);

                            if (itemPath.empty())
                                ImGui::PopDisabledStyle();

                            ImGui::BeginDisabled(readOnly);
                            {
                                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth - 1, _object, style::icon::Trashcan, fmt::sprintf("Remove element %u", i)))
                                    removeAt = i;

                                ImGui::BeginDisabled(i == 0);
                                {
                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth - 1, _object, style::icon::MoveUp, fmt::sprintf("Move up element %u", i), 1))
                                        moveUpAt = i;
                                }
                                ImGui::EndDisabled();

                                ImGui::BeginDisabled(i == resourceCount - 1);
                                {
                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth - 1, _object, style::icon::MoveDown, fmt::sprintf("Move down element %u", i), 2))
                                        moveDownAt = i;
                                }
                                ImGui::EndDisabled();
                            }
                            ImGui::EndDisabled();

                            if (open)
                                changed |= ImGuiWindow::displayResource(resource, _object, prop, i, propContext);

                            ImGui::PopID();
                        }

                        if (-1 != moveUpAt)
                            changed = list->MoveUp(moveUpAt);

                        if (-1 != moveDownAt)
                            changed = list->MoveDown(moveDownAt);

                        if (-1 != removeAt)
                            changed = list->Remove(removeAt);
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

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(ResourceList, ImGuiResourceListHandler);
}



