#pragma once

#include "renderer/ISkeleton.h"

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

            const ISkeleton * skeleton = dynamic_cast<ISkeleton *>(_object);
            VG_ASSERT(skeleton);
            if (skeleton)
            {
                const auto * factory = Kernel::getFactory();
                const auto * classDesc = factory->getClassDescriptor(_object->getClassName());

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
                        //const float4x4 nodeToParentMatrix =  

                        char nodeLabel[256];
                        const char * sub = strstr(nodeName.c_str(), "mixamorig:");

                        if (sub)
                            sprintf_s(nodeLabel, "[%u] %s", i, nodeName.c_str() + strlen("mixamorig:"));
                        else
                            sprintf_s(nodeLabel, "[%u] %s", i, nodeName.c_str());

                        if (ImGui::TreeNode(nodeLabel))
                        {
                            ImGui::BeginDisabled(true);
                            {
                                int parent = skeleton->GetParentIndex(i);
                                ImGui::InputInt("ParentIndex", &parent);
                                //ImGuiWindow::displayFloat4x4("Bone", (core::float4x4 *)&node.node_to_parent);
                            }
                            ImGui::EndDisabled();

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::PopItemWidth();
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Skeleton, ImGuiSkeletonObjectHandler);
}



