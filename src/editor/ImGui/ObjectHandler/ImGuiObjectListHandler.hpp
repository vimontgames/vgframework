#pragma once
#include "core/File/File.h"
#include "core/IObject.h"
#include "core/IObjectList.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiObjectListHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext)
        {
            return displayObjectList(_object, _propContext->m_originalProp->GetDisplayName(), "m_objects", _propContext);
        }

        //--------------------------------------------------------------------------------------
        bool displayObjectList(IObject * _object, const core::string & _label, const core::string & _vectorPropName, const PropertyContext * _propContext)
        {
            string label = removeTrailingChar(_label, 's');
                   label = removeStartingIcon(label);

            ImGuiStyle & style = ImGui::GetStyle();
            bool changed = false;

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->GetClassDescriptor(_object->GetClassName());
            auto list = dynamic_cast<IObjectList *>(_object);
            VG_ASSERT(list, "[Editor] Object is not an Object list");
            if (nullptr == list)
                return false;
            auto availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x * 2 + 1;

            uint objectCount = 0;
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                if (!strcmp(prop->GetName(), _vectorPropName.c_str()))
                    objectCount = prop->GetPropertyObjectVectorCount(_object);
            }

            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

            ImGui::PushID("CollapsingHeader");
            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", _object).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
            ImGui::PopID();

            const bool readOnly = _propContext && _propContext->m_readOnly;

            if (readOnly)
                ImGui::PushDisabledStyle(true);

            const string headerLabel = fmt::sprintf("%ss (%u)", label.c_str(), objectCount);
            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, headerLabel.c_str(), true);

            if (open)
                ImGui::Indent();

            bool remove = false;

            ImGui::BeginDisabled(readOnly);
            {
                if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Plus, fmt::sprintf("Add %s", label), 0))
                {
                    list->Add(fmt::sprintf("new %s", label));
                    changed = true;
                }
                ImGui::BeginDisabled(list->Size() == 0);
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

                        objectCount = propContext.m_originalProp->GetPropertyObjectVectorCount(propContext.m_originalObject);

                        int removeAt = -1;
                        int moveUpAt = -1;
                        int moveDownAt = -1;

                        for (uint i = 0; i < objectCount; ++i)
                        {
                            collapsingHeaderPos.y = ImGui::GetCursorPos().y;
                            availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - style.FramePadding.x;

                            ImGui::PushID(i);
                            auto object = propContext.m_originalProp->GetPropertyObjectVectorElement(propContext.m_originalObject, i);

                            string itemLabel = io::getFileName(object->GetName());

                            if (itemLabel.empty())
                                ImGui::PushDisabledStyle(true);

                            bool open = ImGui::PersistentCollapsingHeader(itemLabel, object, prop);

                            if (itemLabel.empty())
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

                                ImGui::BeginDisabled(i == objectCount-1);
                                {
                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth - 1, _object, style::icon::MoveDown, fmt::sprintf("Move down element %u", i), 2))
                                        moveDownAt = i;
                                }
                                ImGui::EndDisabled();
                            }
                            ImGui::EndDisabled();

                            if (open)
                            {
                                changed |= ImGuiWindow::displayObject(object);
                                ImGui::Spacing();
                            }

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

                ImGui::Unindent();
            }

            if (remove)
                list->Pop();

            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(ObjectList, ImGuiObjectListHandler);
}



