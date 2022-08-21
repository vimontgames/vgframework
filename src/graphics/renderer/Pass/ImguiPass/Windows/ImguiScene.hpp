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
                            ImGui::PushID("SceneMenu");
                            if (ImGui::MenuItem("Save"))
                            {
                                openPopup = true;
                                m_sceneMenu = SceneMenu::Save;
                                //fileBrowser.setPath(io::getCurrentWorkingDirectory() + "/data/Scenes");
                                fileBrowser.setFilename(scene->getName() + ".scene");
                            }
                            if (ImGui::MenuItem("Close"))
                            {
                                openPopup = true;
                                m_sceneMenu = SceneMenu::Close;
                            }
                            ImGui::PopID();
                            ImGui::EndPopup();
                        }

                        if (open)
                        {
                            if (nullptr != root)
                                displayGameObject(root);
                            ImGui::TreePop();
                        }

                        if (openPopup)
                        {
                            ImGui::OpenPopup(asString(m_sceneMenu).c_str());
                            openPopup = false;
                        }

                        switch (m_sceneMenu)
                        {
                        case SceneMenu::Save:
                            if (fileBrowser.showFileDialog("Save", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(860, 400), ".scene"))
                            {
                                const string newFilePath = fileBrowser.selected_path;
                                factory->saveToXML(scene, newFilePath);
                            }
                            break;
                        case SceneMenu::Close:
                            {
                                IUniverse * universe = (IUniverse*)scene->getParent();
                                universe->removeScene((IScene*)scene);
                            }
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

        bool open = ImGui::TreeNodeEx(_gameObject->getName().c_str(), children.size() > 0 ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf);
        bool openPopup = false;

        // Menu
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushID("GameObjectMenu");
            if (ImGui::MenuItem("Add Child"))
            {
                m_gameObjectMenu = GameObjectMenu::AddGameObject;
                openPopup = true;
                //IGameObject * gameObject = (IGameObject*)CreateFactoryObject(GameObject, "new GameObject", _gameObject);
                //_gameObject->AddChild(gameObject);
                //gameObject->Release();
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }

        if (openPopup)
            ImGui::OpenPopup(asString(m_gameObjectMenu).c_str());

        if (ImGui::BeginPopupModal(asString(m_gameObjectMenu).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char nameTmp[1024] = { '\0' };

            if (nameTmp[0] == '\0')
                strcpy(nameTmp, "New GameObject");

            ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll);

            string newName = nameTmp;

            if (ImGui::Button("Add", ImVec2(80, 0)))
            {
                IGameObject * gameObject = (IGameObject*)CreateFactoryObject(GameObject, newName.c_str(), _gameObject);
                _gameObject->AddChild(gameObject);
                gameObject->Release();
                ImGui::CloseCurrentPopup();
                nameTmp[0] = '\0';
                m_gameObjectMenu = GameObjectMenu::None;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel", ImVec2(80, 0)))
            {
                ImGui::CloseCurrentPopup();
                nameTmp[0] = '\0';
                m_gameObjectMenu = GameObjectMenu::None;
            }

            ImGui::EndPopup();
        }

        if (open)
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