#pragma once

#include "renderer/Animation/Skeleton.h"

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

            const Skeleton * skeleton = dynamic_cast<Skeleton *>(_object);
            VG_ASSERT(skeleton);
            if (skeleton)
            {
                const auto indices = skeleton->getBoneIndices(); 
                const auto matrices = skeleton->getBoneMatrices();
                const uint boneCount = (uint)indices.size();
                VG_ASSERT(indices.size() == matrices.size());
                char treeNodeLabel[256];
                sprintf_s(treeNodeLabel, "Bones (%u)", boneCount);
                if (ImGui::TreeNode(treeNodeLabel))
                {
                    for (uint i = 0; i < boneCount; ++i)
                    {
                        char boneLabel[256];
                        sprintf_s(boneLabel, "Bone %u (index %u)", i, indices[i]);
                        if (ImGui::TreeNode(boneLabel))
                        {
                            ImGui::BeginDisabled(true);
                            {
                                ImGuiWindow::displayFloat4x4("Bone", (core::float4x4 *)&matrices[i]);
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



