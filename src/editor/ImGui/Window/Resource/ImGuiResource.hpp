#include "ImGuiResource.h"
#include "core/IResourceManager.h"

using namespace vg::core;
using namespace vg::engine;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiResource::ImGuiResource() :
        ImGuiWindow(style::icon::Resource, "", "Resources", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiResource::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Resource, GetName().c_str(), &m_isVisible))
        {
            IResourceManager * rm = Editor::get()->getEngine()->GetResourceManager();

            bool enabled = rm != nullptr && !rm->HasResourceLoading();

            if (ImGui::TooltipButton(fmt::sprintf("%s Reload Resources", style::icon::Reload).c_str(), enabled, enabled, "Reload Resources (F7)", style::button::SizeLarge))
                rm->UpdateResources();

            ImGui::Separator();

            rm->Lock();

            const auto resCount = rm->GetResourceCount();

            // Sort Resources by resource type
            unordered_map<string, vector<const IResourceInfo *>> resourcesByType;

            for (uint i = 0; i < resCount; ++i)
            {
                const IResourceInfo & resInfo = rm->GetResourceInfo(i);
                const string resType = resInfo.GetResourceType();

                auto it = resourcesByType.find(resType);
                if (resourcesByType.end() == it)
                {
                    vector<const IResourceInfo *> resList;
                    it = resourcesByType.insert(std::pair<string, vector<const IResourceInfo *>>(resType, resList)).first;
                }

                vector<const IResourceInfo *> & resList = it->second;
                resList.push_back(&resInfo);
            }

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                char allResLabel[256];
                sprintf_s(allResLabel, "Resources (%u)", resCount);
                if (ImGui::TreeNodeEx(allResLabel, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto & pair : resourcesByType)
                    {
                        string name = pair.first;
                        const auto & list = pair.second;

                        // Remove "Resource" at the end of the resource type name
                        const string resSuffix = "Resource";
                        auto lastResString = name.find_last_of(resSuffix);
                        if (-1 != lastResString)
                            name = name.substr(0, name.length() - resSuffix.length());

                        char resCatLabel[256];
                        sprintf_s(resCatLabel, "%s (%u)", name.c_str(), (uint)list.size());
                        if (ImGui::TreeNode(resCatLabel))
                        {
                            for (uint i = 0; i < list.size(); ++i)
                            {
                                ImGui::PushID(i);

                                const IResourceInfo * resInfo = list[i];
                                string resPath = resInfo->GetResourcePath().c_str();
                                string fileName = io::getFileName(resPath);
                                char resLabel[256];
                                auto resTreeFlags = ImGuiTreeNodeFlags_None;
                                const uint clientCount = resInfo->GetClientCount();
                                if (clientCount > 1)
                                    sprintf_s(resLabel, "%s (%u)", fileName.c_str(), clientCount);
                                else
                                    sprintf_s(resLabel, "%s", fileName.c_str());

                                bool resOpen = ImGui::TreeNodeEx(resLabel, resTreeFlags);

                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip(resPath.c_str());

                                if (resOpen)
                                {
                                    for (uint j = 0; j < clientCount; ++j)
                                    {
                                        const IResource * client = resInfo->GetClient(j);

                                        char clientLabel[256];
                                        clientLabel[0] = '\0';

                                        // for textures, display the material if found
                                        if (resInfo->GetResourceType() == "TextureResource")
                                        {
                                            IObject * parent = client->GetParent();
                                            IObject * material = nullptr;
                                            while (parent)
                                            {
                                                if ((string)parent->GetClassName() == "MaterialResourceData")
                                                {
                                                    material = parent;
                                                    break;
                                                }

                                                parent = parent->GetParent();
                                            }

                                            if (nullptr != material)
                                            {
                                                sprintf_s(clientLabel, "%s###%u %u", io::getFileName(material->GetFile()).c_str(), i, j);
                                                bool openResGameObject = ImGui::TreeNodeEx(clientLabel, ImGuiTreeNodeFlags_Leaf);

                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip(material->GetFile().c_str());

                                                if (openResGameObject)
                                                {
                                                    ImGui::TreePop();
                                                }

                                                continue;
                                            }
                                        }

                                        // Find parent IComponent
                                        IObject * parent = client->GetParent();
                                        IComponent * component = nullptr;
                                        while (parent)
                                        {
                                            component = dynamic_cast<IComponent *>(parent);
                                            if (component)
                                                break;
                                            parent = parent->GetParent();
                                        }

                                        //VG_ASSERT(component);
                                        if (component)
                                        {
                                            // Find parent GameObject
                                            IGameObject * gameobject = nullptr;
                                            while (parent)
                                            {
                                                gameobject = dynamic_cast<IGameObject *>(parent);
                                                if (gameobject)
                                                    break;
                                                parent = parent->GetParent();
                                            }
                                            VG_ASSERT(gameobject);

                                            if (gameobject)
                                            {
                                                sprintf_s(clientLabel, "%s###%u %u", gameobject->GetName().c_str(), i, j);
                                                bool openResGameObject = ImGui::TreeNodeEx(clientLabel, ImGuiTreeNodeFlags_Leaf);

                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip(component->GetClassName());

                                                if (openResGameObject)
                                                {
                                                    ImGui::TreePop();
                                                }
                                            }
                                        }


                                    }

                                    ImGui::TreePop();
                                }

                                ImGui::PopID();
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

                //if (nullptr != rm)
                //    displayObject(rm); 
            }
            ImGui::EndChild();

            rm->Unlock();
        }  
        ImGui::End();          
    }
}