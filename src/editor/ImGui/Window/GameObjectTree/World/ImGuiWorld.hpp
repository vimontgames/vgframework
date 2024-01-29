#include "ImGuiWorld.h"
#include "editor/ImGui/ImGui.h"
#include "engine/IWorldResource.h"
#include "core/IGameObject.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiWorld::ImGuiWorld() :
        ImGuiGameObjectTree(style::icon::World, "", "World", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWorld::DrawGUI()
    {
        ImGui::PushID("ImguiScene");

        auto worldRes = getEngine()->GetWorldResource();
        string label = worldRes && worldRes->getObject() ? worldRes->getObject()->getName() : "<No World loaded>";

        if (ImGui::IconBegin(style::icon::World, fmt::sprintf("%s###WorldScenes", "World").c_str(), &m_isVisible))
        {
            bool newScene = false, loadScene = false;

            if (ImGui::TooltipButton(editor::style::icon::New, true, true, "New Scene"))
                newScene = true;

            ImGui::SameLine();

            if (ImGui::TooltipButton(editor::style::icon::Load, true, true, "Load Scene"))
                loadScene = true;

            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {

                const auto* factory = Kernel::getFactory();
                engine::IEngine* engine = (engine::IEngine*)factory->getSingleton("Engine");
                auto& fileBrowser = ImGuiWindow::getFileBrowser();

                engine::IWorldResource* worldRes = engine->GetWorldResource();
                IWorld* world = worldRes ? worldRes->GetWorld() : nullptr;

                if (world)
                {
                    bool openPopup = false;

                    if (ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::MenuItem("New Scene"))
                            newScene = true;

                        if (ImGui::MenuItem("Load Scene"))
                            loadScene = true;

                        ImGui::EndPopup();
                    }

                    if (newScene)
                    {
                        m_selected = MenuOption::AddScene;
                        openPopup = true;
                        m_popup = "New Scene";
                        fileBrowser.setFolder(io::getRootDirectory() + "/data/Scenes");
                    }

                    if (loadScene)
                    {
                        m_selected = MenuOption::LoadScene;
                        openPopup = true;
                        m_popup = "Load Scene";
                        fileBrowser.setFolder(io::getRootDirectory() + "/data/Scenes");
                    }

                    if (openPopup)
                    {
                        ImGui::OpenPopup(m_popup);
                        openPopup = false;
                    }

                    string ext = ImGuiWindow::getFileBrowserExt(worldRes);

                    switch (m_selected)
                    {
                    case MenuOption::AddScene:
                    {
                        if (fileBrowser.showFileDialog(m_popup, imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ".scene"))
                        {
                            const string path = io::addExtensionIfNotPresent(fileBrowser.selected_path, ".scene");
                            worldRes->CreateSceneResource(path);
                        }
                    }
                    break;

                    case MenuOption::LoadScene:
                        if (fileBrowser.showFileDialog(m_popup, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, style::dialog::Size, ".scene"))
                        {
                            const string path = fileBrowser.selected_path;
                            worldRes->LoadSceneResource(path);
                        }
                        break;
                    }

                    auto availableWidth = ImGui::GetContentRegionAvail().x;

                    for (uint i = 0; i < worldRes->GetSceneResourceCount(); ++i)
                    {
                        const core::IResource* sceneRes = worldRes->GetSceneResource(i);
                        const IScene* scene = (IScene*)sceneRes->getObject();
                        if (nullptr != scene)
                        {
                            ImGui::PushID("SceneTree");

                            IGameObject* root = scene->GetRoot();
                            auto flags = ImGuiTreeNodeFlags_InvisibleArrow | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen;
                            if (nullptr != root)
                                flags |= ImGuiTreeNodeFlags_None;
                            else
                                flags |= ImGuiTreeNodeFlags_Leaf;

                            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

                            const bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", scene->getName(), scene).c_str(), flags);
                            bool enabled = asBool(IInstance::Flags::Enabled & root->GetFlags());

                            auto status = m_sceneMenu.Display((IObject*)scene);

                            string sceneLabel = fmt::sprintf("%s %s", style::icon::Scene, scene->getName());

                            ImGui::CollapsingHeaderLabel(collapsingHeaderPos, sceneLabel.c_str(), enabled);

                            if (ImGui::CollapsingHeaderCheckbox(collapsingHeaderPos, enabled, root, style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s Scene \"%s\"", enabled ? "Disable" : "Enable", scene->getName().c_str())))
                                root->SetFlags(IInstance::Flags::Enabled, !enabled);

                            if (status == ImGuiMenu::Status::Removed)
                                root = nullptr;

                            if (open)
                            {
                                if (nullptr != root)
                                {
                                    // Draw
                                    uint counter = 0;
                                    displayGameObject(root, &counter);
                                }
                                //ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopID();
    }
}