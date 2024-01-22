#pragma once

#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "editor/ImGui/Menu/Inspector/GameObject/ImGuiGameObjectInspectorMenu.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "imgui/imgui_internal.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiGameObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            auto * go = dynamic_cast<core::IGameObject*>(_object);

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->getClassDescriptor(go->getClassName());

            // Sort top-level properties to display components at the end
            const char * curClassName = nullptr;
            bool visible = false;

            ImGui::PushID("DisplayObject");

            auto availableWidth = ImGui::GetContentRegionAvail().x;
            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();
            string gameObjectLabel = fmt::sprintf("%s %s", style::icon::Scene, "GameObject");
            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", gameObjectLabel, go).c_str(), ImGuiTreeNodeFlags_InvisibleArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);

            m_gameObjectInspectorMenu.Display(go);

            bool isGameobjectEnabled = asBool(IInstance::Flags::Enabled & go->GetFlags());

            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, gameObjectLabel.c_str(), isGameobjectEnabled);

            if (ImGui::CollapsingHeaderCheckbox(collapsingHeaderPos, isGameobjectEnabled, go, style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s GameObject %s", isGameobjectEnabled? "Disable" : "Enable", go->getName().c_str())))
            {
                go->SetFlags(IInstance::Flags::Enabled, !isGameobjectEnabled);
                //changed = true;
            }

            //if (ImGui::CollapsedHeaderIconButton(collapsingHeaderPos, availableWidth, go, style::icon::Trashcan, fmt::sprintf("Remove GameObject \"%s\"", go->getName().c_str())))
            //{
            //    //componentInspectorMenu.removeComponent(pComponent);
            //    //changed = true;
            //}

            if (open)
            {
                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                
                    if (strcmp(prop->getName(), "m_components"))
                    {
                        if (curClassName != prop->getClassName())
                        {
                            curClassName = prop->getClassName();
                            visible = true;
                        }
                
                        if (visible)
                            ImGuiWindow::displayProperty(go, prop);
                    }
                }
            }

            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
            
                if (!strcmp(prop->getName(), "m_components"))
                    ImGuiWindow::displayProperty(go, prop);
            }    

            // "Add Component" button
            {
                // align right
                //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                //ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.75f, 0.5f));
                //{
                //    auto availableWidth = ImGui::GetContentRegionAvail().x;
                //    ImGui::SetCursorPosX(availableWidth - style::button::SizeSmall.x + ImGui::GetStyle().WindowPadding.x + 4);
                //
                //    if (ImGui::Button(ImGui::getObjectLabel(fmt::sprintf("%s", style::icon::Plus).c_str(), go).c_str(), style::button::SizeSmall + ImVec2(0, ImGui::GetStyle().WindowPadding.x - 3)))
                //        m_gameObjectInspectorMenu.addComponentPopup();
                //}
                //ImGui::PopStyleVar(1);

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGuiStyle & style = ImGui::GetStyle();

                // Centered Button
                string label = fmt::sprintf(" %s Add Component ", style::icon::Plus);
                float alignment = 0.5f;
                float size = ImGui::CalcTextSize(label.c_str()).x + style.FramePadding.x * 2.0f;
                float avail = ImGui::GetContentRegionAvail().x;   

                float off = (avail - size) * alignment;
                if (off > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

                if (ImGui::Button(ImGui::getObjectLabel(label.c_str(), go).c_str()))
                    m_gameObjectInspectorMenu.addComponentPopup();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(fmt::sprintf("Add Component to \"%s\"", go->getName()).c_str());
            }

            ImGui::PopID();
        }

    private:
        ImGuiGameObjectInspectorMenu m_gameObjectInspectorMenu;
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(GameObject, ImGuiGameObjectHandler); 
}



