#include "ImguiScene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiScene::display()
    {
        if (ImGui::Begin("Scenes", &m_isVisible))
        {
            const auto * factory = Kernel::getFactory();
            engine::IEngine * engine = (engine::IEngine *)factory->getSingleton("Engine");

            IUniverse * universe = engine->getCurrentUniverse();
            if (universe)
            {
                for (uint i = 0; i < universe->getSceneCount(); ++i)
                {
                    const IScene * scene = universe->getScene(i);
                    if (nullptr != scene)
                    {
                        IGameObject * root = scene->GetRoot();
                        auto flags = ImGuiTreeNodeFlags_Framed;
                        if (nullptr != root)
                            flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
                        else
                            flags |= ImGuiTreeNodeFlags_Leaf;

                        const bool open = ImGui::TreeNodeEx(scene->getName().c_str(), flags);
                        bool openPopup = false;

                        auto & fileBrowser = getFileBrowser();

                        // Menu
                        if (ImGui::BeginPopupContextItem())
                        {
                            ImGui::PushID("ImguiSceneContextMenu");
                            if (ImGui::MenuItem("Save"))
                            {
                                openPopup = true;
                                m_selectedContextMenu = ContextMenu::Save;
                                //fileBrowser.setPath(io::getCurrentWorkingDirectory() + "/data/Scenes");
                                fileBrowser.setFilename(scene->getName() + ".scene");
                            }
                            if (ImGui::MenuItem("Close"))
                            {
                                openPopup = true;
                                m_selectedContextMenu = ContextMenu::Close;
                            }
                            ImGui::PopID();
                            ImGui::EndPopup();
                        }

                        if (open)
                        {
                            displayGameObject(root);
                            ImGui::TreePop();
                        }

                        if (openPopup)
                        {
                            ImGui::OpenPopup(asString(m_selectedContextMenu).c_str());
                            openPopup = false;
                        }

                        switch (m_selectedContextMenu)
                        {
                        case Save:
                            if (fileBrowser.showFileDialog("Save", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(860, 400), ".scene"))
                            {
                                const string newFilePath = fileBrowser.selected_path;
                                scene->saveToFile(newFilePath);
                            }
                            break;
                        case Close:
                            // TODO
                            break;
                        }
                    }
                }
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiScene::displayGameObject(IGameObject * _gameObject)
    {
        const auto children = _gameObject->GetChildren();

        if (ImGui::TreeNodeEx(_gameObject->getName().c_str(), children.size() > 0 ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf))
        {
            updateSelection(_gameObject);
            
            for (uint j = 0; j < children.size(); ++j)
            {
                IGameObject * child = children[j];
                displayGameObject(child);
            }
            ImGui::TreePop();
        }        
    }
}