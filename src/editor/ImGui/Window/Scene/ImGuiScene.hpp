#include "ImguiScene.h"
#include "editor/ImGui/ImGui.h"
#include "core/IGameObject.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiScene::ImGuiScene() :
        ImGuiWindow(style::icon::Scene, "", "Scenes", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

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

                            ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

                            string newName = nameTmp;

                            if (ImGui::Button("Add", style::button::SizeMedium))
                            {
                                IScene * newScene = (IScene*)CreateFactoryObject(Scene, newName.c_str(), universe);
                                universe->AddScene(newScene);
                                IGameObject * rootSector = (IGameObject*)CreateFactoryObject(GameObject, "Root", newScene);
                                newScene->SetRoot(rootSector);
                                rootSector->release();
                                newScene->Release();
                                ImGui::CloseCurrentPopup();
                                nameTmp[0] = '\0';
                                m_selected = MenuOption::None;
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Cancel", style::button::SizeMedium))
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
                                universe->AddScene(scene);
                                scene->Release();
                            }
                            else
                            {
                                VG_SAFE_DELETE(scene);
                            }
                        }
                        break;
                }

                for (uint i = 0; i < universe->GetSceneCount(); ++i)
                {
                    const IScene * scene = universe->GetScene(i);
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
                            {
                                // First pass to get child count
                                uint count = 0;
                                displayGameObject(root, &count);

                                // Draw background
                                DrawRowsBackground(count, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f)), ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));

                                // Draw
                                displayGameObject(root);
                            }
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
    void ImGuiScene::displayGameObject(IGameObject * _gameObject, uint * _count)
    {
        const bool counting = (_count != nullptr);
        const auto children = _gameObject->GetChildren();

        bool open = counting;
        if (counting)
        {
            (*_count)++;
        }
        else
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
            if (children.size() > 0)
                flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            else
                flags |= ImGuiTreeNodeFlags_Leaf;

            if (getSelection()->IsSelectedObject(_gameObject))
            {
                //const auto selColor = ImGui::GetStyleColorVec4(ImGuiCol_Header); 
                //DrawRowsBackground(1, GetColorU32(selColor));
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            open = ImGui::TreeNodeEx(_gameObject->getName().c_str(), flags);
            m_gameObjectMenu.Display(_gameObject);
        }

        if (open)
        {
            if (!counting)
                updateSelection(_gameObject);
            
            for (uint j = 0; j < children.size(); ++j)
            {
                IGameObject * child = children[j];
                displayGameObject(child, _count);
            }

            if (!counting)
                ImGui::TreePop();
        }        
    }
}