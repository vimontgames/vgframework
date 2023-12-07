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

    enum DragDropDest
    {
        Node = 0,
        Interline
    };

    //--------------------------------------------------------------------------------------
    bool doDragDrop(IGameObject * _from, IGameObject * _to, DragDropDest _dest)
    {
        // Moved not cannot became a child of its own child
        if (_to->HasAncestor(_from))
        {
            VG_WARNING("[Scene] Cannot move GameObject because it would become a child of its own children");
            return false;
        }

        switch(_dest)
        {
            case DragDropDest::Node:
            {
                // '_to' is the new parent of '_from'
                IGameObject * fromParent = dynamic_cast<IGameObject *>(_from->getParent());
                if (fromParent)
                {
                    VG_SAFE_INCREASE_REFCOUNT(_from);
                    fromParent->RemoveChild(_from);
                    _to->AddChild(_from, 0);
                    VG_SAFE_RELEASE(_from);
                    return true;
                }
            }
            break;

            case DragDropDest::Interline:
            {
                // '_from' is not the next following brother of '_to'
                IGameObject * fromParent = dynamic_cast<IGameObject *>(_from->getParent());
                if (fromParent)
                {
                    IGameObject * toParent = dynamic_cast<IGameObject *>(_to->getParent());
                    if (toParent)
                    {
                        VG_SAFE_INCREASE_REFCOUNT(_from);
                        fromParent->RemoveChild(_from);
                
                        uint childIndex = toParent->GetChildIndex(_to);
                        toParent->AddChild(_from, childIndex + 1);
                        
                        VG_SAFE_RELEASE(_from);
                
                        return true;
                    }
                }
            }
            break;
        }

        return false;
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
                const auto selColor = ImGui::GetStyleColorVec4(ImGuiCol_Header); 
                DrawRowsBackground(1, GetColorU32(selColor));
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (_gameObject == m_dragAndDropNodeTarget)
            {
                const auto selColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive);
                DrawRowsBackground(1, GetColorU32(selColor));
            }

            if (m_gameObjectMenu.m_RenamingGameObject == _gameObject)
            {
                char gameObjectLabel[256];
                sprintf_s(gameObjectLabel, "###%p", _gameObject);
                open = ImGui::TreeNodeEx(gameObjectLabel, flags);
                ImGui::SameLine();
                char temp[256];
                sprintf_s(temp, _gameObject->getName().c_str());
                sprintf_s(gameObjectLabel, "###Ren%p", _gameObject);

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
                {
                    if (ImGui::InputText(gameObjectLabel, temp, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        _gameObject->setName(temp);
                        m_gameObjectMenu.m_RenamingGameObject = nullptr;
                    }
                    else
                    {
                        if (!ImGui::IsAnyItemActive())
                            ImGui::SetKeyboardFocusHere(-1);
                    }
                }
                ImGui::PopStyleVar();
            }
            else
            {
                string gameObjectLabel = fmt::sprintf("%s###%p", _gameObject->getName(), (void*)_gameObject);
                open = ImGui::TreeNodeEx(gameObjectLabel.c_str(), flags | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth);                
            }

            bool startDragDrop = false;

            // Treenode receiving drag'n drop
            if (ImGui::BeginDragDropTarget())
            {
                ImGuiDragDropFlags target_flags = 0;
                target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
                if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
                {
                    IGameObject * from = *(IGameObject **)payload->Data;
                    IGameObject * to = _gameObject;

                    //VG_INFO("Moving from \"%s\" to node \"%s\"", from->getName().c_str(), to->getName().c_str());

                    if (payload->Delivery)
                    {
                        doDragDrop(from, to, DragDropDest::Node);
                        m_dragAndDropNodeTarget = nullptr;
                    }
                    else
                        m_dragAndDropNodeTarget = to;

                    //ImDrawList * draw_list = ImGui::GetWindowDrawList();
                    //float x0 = ImGui::GetCursorScreenPos().x;
                    //float y0 = ImGui::GetCursorScreenPos().y - 18;
                    //float x1 = x0 + 1024;
                    //float y1 = y0 + 18;
                    //
                    //const auto interlineColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive);
                    //u32 intColor = GetColorU32(float4(interlineColor.x, interlineColor.y, interlineColor.z, interlineColor.w));
                    //draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), intColor);

                    startDragDrop = true;
                    m_dragAndDropInterlineTarget = nullptr;
                }

                ImGui::EndDragDropTarget();
            }

            if (!startDragDrop)
            {
                if (!counting)
                    updateSelection(_gameObject);
            }

            m_gameObjectMenu.Display(_gameObject);

            ImGuiDragDropFlags src_flags = 0;
            src_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;     // Keep the source displayed as hovered
            src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging

            if (ImGui::BeginDragDropSource(src_flags))
            {
                //VG_INFO("BeginDragDropSource");
            
                //if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                //    ImGui::Text("Moving \"%s\"", _gameObject->getName().c_str());
                ImGui::SetDragDropPayload("GameObject", &_gameObject, sizeof(IGameObject*));
                ImGui::EndDragDropSource();
            }

            float interlineHeight = 8.0f;

            // Invisible selectable for interline
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - interlineHeight - 1);
                    string beforeGO = fmt::sprintf("###after %s", _gameObject->getName().c_str());
                    ImGui::InvisibleButton(beforeGO.c_str(), ImVec2(256.0f, interlineHeight));
                
                    // debug
                    //ImDrawList * draw_list = ImGui::GetWindowDrawList();
                    //float x0 = ImGui::GetCursorScreenPos().x;
                    //float y0 = ImGui::GetCursorScreenPos().y - 4 - 3;
                    //float x1 = x0 + 1024;
                    //float y1 = y0 + 4;
                    //draw_list->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), 0x7F0000FF);
                }
                ImGui::PopStyleVar(1);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
            }

            // "after" interline receiving drag'n drop
            if (ImGui::BeginDragDropTarget())
            {
                ImGuiDragDropFlags target_flags = 0;
                target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
                if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
                {
                    IGameObject * from = *(IGameObject**)payload->Data;
                    IGameObject * to = _gameObject;
            
                    //VG_INFO("Moving from \"%s\" to after \"%s\"", from->getName().c_str(), to->getName().c_str());
            
                    ImDrawList * draw_list = ImGui::GetWindowDrawList();
                    float x0 = ImGui::GetCursorScreenPos().x;
                    float y0 = ImGui::GetCursorScreenPos().y - interlineHeight/2;
                    float x1 = x0 + 1024;
                    float y1 = y0 + interlineHeight/2;
                    
                    const auto interlineColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive); 
                    u32 intColor = GetColorU32(interlineColor);
                    draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), intColor);

                    if (payload->Delivery)
                    {
                        doDragDrop(from, to, DragDropDest::Interline);
                        m_dragAndDropInterlineTarget = nullptr;
                    }
                    else
                        m_dragAndDropInterlineTarget = to;

                    m_dragAndDropNodeTarget = nullptr;
                }
            
                ImGui::EndDragDropTarget();
            }
        }

        if (open)
        {            
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