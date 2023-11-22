#pragma once

#include "renderer/Animation/Skeleton.h"
//#include "renderer/Importer/MeshImporterData.h"
//#include "renderer/Model/Mesh/MeshModel.h"

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

            //const MeshModel * meshModel = dynamic_cast<MeshModel *>(_object->getParent());
            //const MeshGeometry * geometry = meshModel->GetGeometry();

            const Skeleton * skeleton = dynamic_cast<Skeleton *>(_object);
            VG_ASSERT(skeleton);
            if (skeleton)
            {
                const auto * factory = Kernel::getFactory();
                const auto * classDesc = factory->getClassDescriptor(_object->getClassName());

                for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->getPropertyByIndex(i);
                    ImGuiWindow::displayProperty(_object, prop);
                }

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
                        //MeshImporterNode * bone = 

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



