#pragma once

#include "core/IGameObject.h"
#include "engine/GameObject/Prefab/PrefabGameObject.h"
#include "core/IComponent.h"
#include "editor/ImGui/Menu/Inspector/GameObject/ImGuiGameObjectInspectorMenu.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/Options/EditorOptions.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiGameObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            bool changed = false;

            auto * go = dynamic_cast<core::IGameObject*>(_object);

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->GetClassDescriptor(go->GetClassName());

            // Sort top-level properties to display components at the end
            const char * curClassName = nullptr;
            bool visible = false;

            ImGui::PushID("DisplayObject");

            const bool isPrefab = go->IsPrefab();
            string goTypeName = isPrefab ? "Prefab" : "GameObject";
            auto goIcon = isPrefab ? style::icon::Prefab : style::icon::GameObject;

            IGameObject * prefab = nullptr;
            bool isPrefabInstanced = false;
            if (go)
            {
                prefab = go->GetParentPrefab();
                isPrefabInstanced = nullptr != prefab;
            }

            auto availableWidth = ImGui::GetContentRegionAvail().x;
            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();
            string gameObjectLabel = fmt::sprintf("%s %s", goIcon, goTypeName);

            if (isPrefab)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
            //else if (isPrefabInstanced)
            //    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", gameObjectLabel, go).c_str(), /*ImGuiTreeNodeFlags_InvisibleArrow |*/ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

            if (isPrefab)
                ImGui::PopStyleColor(1);
            //else if (isPrefabInstanced)
            //    ImGui::PopStyleColor(1);

            m_gameObjectInspectorMenu.Display(go);

            bool isGameobjectEnabled = asBool(InstanceFlags::Enabled & go->GetInstanceFlags());

            const bool debugInspector = EditorOptions::get()->IsDebugInspector();
            if (debugInspector)
                gameObjectLabel += fmt::sprintf(" (0x%016llX)", (u64)_object);

            if (isPrefab)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
            //else if (isPrefabInstanced)
            //    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, gameObjectLabel.c_str(), isGameobjectEnabled);

            if (isPrefab)
                ImGui::PopStyleColor(1);
            //else if (isPrefabInstanced)
            //    ImGui::PopStyleColor(1);

            if (ImGui::CollapsingHeaderCheckbox(collapsingHeaderPos, isGameobjectEnabled, go, style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s %s \"%s\"", isGameobjectEnabled? "Disable" : "Enable", goTypeName, go->GetName().c_str())))
                go->SetInstanceFlags(InstanceFlags::Enabled, !isGameobjectEnabled);

            if (open)
            {
                ImGui::Indent();
                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                
                    if (strcmp(prop->GetName(), "m_components"))
                    {
                        if (curClassName != prop->GetClassName())
                        {
                            curClassName = prop->GetClassName();
                            visible = true;
                        }
                
                        if (visible)
                            changed |= ImGuiWindow::displayProperty(go, prop);
                    }
                }
                ImGui::Unindent();
            }

            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
            
                if (!strcmp(prop->GetName(), "m_components"))
                    changed |= ImGuiWindow::displayProperty(go, prop);
            }    

            // "Add Component" button
            if (!isPrefab)
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
                float size = editor::style::button::SizeLarge.x; //ImGui::CalcTextSize(label.c_str()).x + style.FramePadding.x * 2.0f;
                float avail = ImGui::GetContentRegionAvail().x;   

                float off = (avail - size) * alignment;
                if (off > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

                if (ImGui::Button(ImGui::getObjectLabel(label.c_str(), go).c_str(), editor::style::button::SizeLarge))
                    m_gameObjectInspectorMenu.addComponentPopup();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(fmt::sprintf("Add Component to \"%s\"", go->GetName()).c_str());
            }

            ImGui::PopID();

            return changed;
        }

    private:
        ImGuiGameObjectInspectorMenu m_gameObjectInspectorMenu;
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(GameObject, ImGuiGameObjectHandler); 
    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(PrefabGameObject, ImGuiGameObjectHandler);
}



