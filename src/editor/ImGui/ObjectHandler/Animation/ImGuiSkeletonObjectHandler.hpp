#pragma once

#include "renderer/ISkeleton.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "renderer/Importer/BodyPartFlags.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::editor
{
    class ImGuiSkeletonObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            bool changed = false;

            auto availableWidth = ImGui::GetContentRegionAvail().x;
            ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

            ISkeleton * skeleton = dynamic_cast<ISkeleton *>(_object);
            VG_ASSERT(skeleton);
            if (skeleton)
            {
                const auto * factory = Kernel::getFactory();
                const auto * classDesc = factory->GetClassDescriptor(_object->GetClassName());

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                    changed |= ImGuiWindow::displayProperty(_object, prop);
                }

                const uint nodeCount = skeleton->GetNodeCount();

                char treeNodeLabel[256];
                sprintf_s(treeNodeLabel, "Bones [%u]", nodeCount);
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
                            {
                                skeleton->SelectNode(i, b);
                                changed = true;
                            }

                            {
                                ImGui::BeginDisabled(true);
                                int parent = skeleton->GetParentIndex(i);
                                changed |= ImGui::InputInt("ParentIndex", &parent);
                                ImGui::EndDisabled();

                                string enumLabel = ImGui::getObjectLabel("BodyParts", _object);
                                auto enumVal = skeleton->GetNodeFlags(i);
                                string preview = "";
                                bool first = true;
                                for (uint e = 0; e < enumCount<BodyPartFlags>(); ++e)
                                {
                                    BodyPartFlags enumBit = (BodyPartFlags)getEnumValue<BodyPartFlags>(e);

                                    if (asBool(enumVal & enumBit))
                                    {
                                        if (!first)
                                            preview += " | ";
                                        preview += getEnumString((BodyPartFlags)enumBit);
                                        first = false;
                                    }
                                }

                                ImGui::PushDisabledStyle(true);

                                if (ImGui::BeginCombo(enumLabel.c_str(), preview.c_str(), ImGuiComboFlags_None))
                                {
                                    for (uint e = 0; e < enumCount<BodyPartFlags>(); ++e)
                                    {
                                        BodyPartFlags enumBit = (BodyPartFlags)getEnumValue<BodyPartFlags>(e);

                                        bool value = asBool(enumVal & enumBit) ? true : false;
                                        const string name = getEnumString((BodyPartFlags)enumBit);

                                        if (ImGui::Checkbox(name.c_str(), &value))
                                        {
                                            if (value)
                                                enumVal |= enumBit;
                                            else
                                                enumVal &= ~enumBit;
                                        }
                                    }
                                    ImGui::EndCombo();
                                }

                                ImGui::PopDisabledStyle();
                            }

                            ImGui::TreePop();
                        }

                        ImGui::PopID();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::PopItemWidth();

            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Skeleton, ImGuiSkeletonObjectHandler);
}



