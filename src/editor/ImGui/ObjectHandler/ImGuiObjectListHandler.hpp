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

            string label = fmt::sprintf("%s (%u)", _label.c_str(), objectCount);

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
                            }
                            ImGui::EndDisabled();

                            if (open)
                                changed |= ImGuiWindow::displayObject(object);
                                //changed |= ImGuiWindow::displayResource(resource, _object, prop, i, propContext);

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

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(ObjectList, ImGuiObjectListHandler);
}



