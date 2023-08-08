#include "ImguiScene.h"
#include "imgui/imgui.h"
#include "core/IGameObject.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiScene::DrawGUI()
    {
        ImGui::PushID("ImguiScene");

        if (ImGui::IconBegin(style::icon::Scene, "Scenes", &m_isVisible))
        {
            const auto * factory = Kernel::getFactory();
            engine::IEngine * engine = (engine::IEngine *)factory->getSingleton("Engine");
            auto & fileBrowser = ImGuiWindow::getFileBrowser();
            IUniverse * universe = engine->getCurrentUniverse();
            if (universe)
            {
                bool openPopup = false;

                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Add Scene"))
                    {
                        m_selected = MenuOption::AddScene;
                        openPopup = true;
                        m_popup = "Add Scene";
                    }

                    if (ImGui::MenuItem("Load Scene"))
                    {
                        m_selected = MenuOption::LoadScene;
                        openPopup = true;
                        m_popup = "Load Scene";
                    }

                    ImGui::EndPopup();
                }

                if (openPopup)
                {
                    ImGui::OpenPopup(m_popup);
                    openPopup = false;
                }
                
                switch (m_selected)
                {
                    case MenuOption::AddScene:
                        if (ImGui::BeginPopupModal(m_popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                        {
                            static char nameTmp[1024] = { '\0' };

                            if (nameTmp[0] == '\0')
                                strcpy(nameTmp, "New Scene");

                            ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll);

                            string newName = nameTmp;

                            if (ImGui::Button("Add", style::button::Size))
                            {
                                IScene * newScene = (IScene*)CreateFactoryObject(Scene, newName.c_str(), universe);
                                universe->addScene(newScene);
                                IGameObject * rootSector = (IGameObject*)CreateFactoryObject(GameObject, "Root", newScene);
                                newScene->SetRoot(rootSector);
                                rootSector->release();
                                newScene->Release();
                                ImGui::CloseCurrentPopup();
                                nameTmp[0] = '\0';
                                m_selected = MenuOption::None;
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Cancel", style::button::Size))
                            {
                                ImGui::CloseCurrentPopup();
                                nameTmp[0] = '\0';
                                m_selected = MenuOption::None;
                            }
                            ImGui::EndPopup();
                        }
                        break;

                    case MenuOption::LoadScene:
                        if (fileBrowser.showFileDialog(m_popup, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, style::dialog::Size, ".scene"))
                        {
                            const string path = fileBrowser.selected_path;
                            IScene* scene = (IScene*)CreateFactoryObject(Scene, "", universe);
                            if (factory->loadFromXML(scene, path))
                            {
                                universe->addScene(scene);
                                scene->Release();

                                //status = Status::Loaded;
                            }
                            else
                            {
                                VG_SAFE_DELETE(scene);
                            }
                        }
                        break;
                }

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

                        auto status = m_sceneMenu.Display((IObject*)scene);

                        if (status == ImGuiMenu::Status::Removed)
                            root = nullptr;

                        if (open)
                        {
                            if (nullptr != root)
                                displayGameObject(root);
                            ImGui::TreePop();
                        }
                    }
                }
            }
        }
        ImGui::End();
        ImGui::PopID();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiScene::displayGameObject(IGameObject * _gameObject)
    {
        const auto children = _gameObject->GetChildren();

        ImGuiTreeNodeFlags flags;
        if (children.size() > 0)
            flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
        else
            flags = ImGuiTreeNodeFlags_Leaf;

        if (isSelectedObject(_gameObject))
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(_gameObject->getName().c_str(), flags);
        bool openPopup = false;

        m_gameObjectMenu.Display(_gameObject);

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