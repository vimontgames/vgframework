#pragma once

#include "renderer/ISkeleton.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::editor
{
    class ImGuiSkeletonObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            auto availableWidth = ImGui::GetContentRegionAvail().x;
            ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

            ISkeleton * skeleton = dynamic_cast<ISkeleton *>(_object);
            VG_ASSERT(skeleton);
            if (skeleton)
            {
                const auto * factory = Kernel::getFactory();
                const auto * classDesc = factory->getClassDescriptor(_object->GetClassName());

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                    ImGuiWindow::displayProperty(_object, prop);
                }

                const uint nodeCount = skeleton->GetNodeCount();

                char treeNodeLabel[256];
                sprintf_s(treeNodeLabel, "Nodes (%u)", nodeCount);
                if (ImGui::TreeNode(treeNodeLabel))
                {
                    for (uint i = 0; i < nodeCount; ++i)
                    {
                        const string nodeName = skeleton->GetNodeName(i);

                        char nodeLabel[256];
                        const char * sub = strstr(nodeName.c_str(), "mixamorig:");

                        ImGui::PushID(i);

                        ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

                        if (sub)
                            sprintf_s(nodeLabel, "[%u] %s", i, nodeName.c_str() + strlen("mixamorig:"));
                        else
                            sprintf_s(nodeLabel, "[%u] %s", i, nodeName.c_str());
                    
                        bool isNodeSelected = skeleton->IsNodeSelected(i);
                        //ImGui::CollapsingHeaderLabel(collapsingHeaderPos, nodeLabel, isNodeSelected);
                        //
                        //if (ImGui::CollapsingHeaderCheckbox(collapsingHeaderPos, isNodeSelected, skeleton, style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s", isNodeSelected ? "Disable Node" : "Enable Node").c_str()))
                        //{
                        //    //pComponent->SetComponentFlags(ComponentFlags::Enabled, !isComponentEnabled);
                        //    //changed = true;
                        //}

                        if (ImGui::TreeNode(nodeLabel))
                        {
                            if (ImGui::BeginPopupContextItem("node_context_menu"))
                            {
                                if (ImGui::MenuItem("Copy node name to clipboard"))
                                    ImGui::SetClipboardText(nodeName.c_str());

                                ImGui::EndPopup();
                            }

                            bool b = skeleton->IsNodeSelected(i);
                            if (ImGui::Checkbox("Selected", &b))
                                skeleton->SelectNode(i, b);

                            ImGui::BeginDisabled(true);
                            {
                                int parent = skeleton->GetParentIndex(i);
                                ImGui::InputInt("ParentIndex", &parent);
                            }
                            ImGui::EndDisabled();

                            ImGui::TreePop();
                        }

                        ImGui::PopID();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::PopItemWidth();
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Skeleton, ImGuiSkeletonObjectHandler);
}



