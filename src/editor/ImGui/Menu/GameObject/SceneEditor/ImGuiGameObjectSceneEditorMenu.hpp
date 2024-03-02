#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "core/IResource.h"
#include "core/ISelection.h"
#include "core/IInput.h"
#include "engine/IEngine.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/Editor.h"
#include "core/IBaseScene.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"
#include "core/File/File.h"
#include "engine/IWorldResource.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiGameObjectSceneEditorMenu::Display(core::IObject * _object)
    {
        auto status = Status::None;
        IGameObject * gameObject = dynamic_cast<IGameObject*>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;
        bool doDelete = false;
        bool doRename = false;
        bool doDuplicate = false;

        ISelection * selection = Editor::get()->getSelection();

        // The copy here is *intentional*
        auto selectedObjects = selection->GetSelectedObjects(); 

        // Add the clicked object to "selection" if not already selected
        const bool isRoot = gameObject->IsRoot();
        if (!isRoot && !selection->IsSelectedObject(gameObject))
            selectedObjects.push_back(gameObject);

        vector<IGameObject *> gameObjectsToDelete = Editor::get()->getSelection()->RemoveChildGameObjectsWithParents(selectedObjects);

        const bool isPrefab = gameObject->IsPrefab();
        const bool isPrefabChild = gameObject->GetParentPrefab();
        const bool isPrefabOrPartOfPrefab = isPrefab || isPrefabChild;

        // Root GameObject cannot be deleted
        const bool canAddChildGameObject = !isPrefabOrPartOfPrefab;
        const bool canAddChildPrefab = !isPrefabOrPartOfPrefab;
        const bool canDelete = gameObjectsToDelete.size() > 0 && !isRoot;
        const bool canRename = true; // !isRoot;
        const bool canDuplicate = !isRoot;

        if (m_RenamingGameObject == nullptr)
        {
            if (selection->IsSelectedObject(gameObject)) 
            {
                if (!ImGui::IsAnyItemActive())
                {
                    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && canDelete)
                        doDelete = true;
                    else if (canRename && ImGui::IsKeyPressed(ImGuiKey_F2) && canRename)
                        doRename = true;
                    else if (ImGui::IsKeyPressed(ImGuiKey_D) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && canDuplicate)
                        doDuplicate = true;
                }
            }
        }        

        if (ImGui::BeginPopupContextItem())
        {
            // if menu called on non-selected object then it becomes the selection
            if (!selection->IsSelectedObject(_object))
                selection->SetSelectedObject(_object);

            //ImGui::BeginDisabled((isRoot || isPrefabChild) && isPrefabOrPartOfPrefab);
            if (ImGui::BeginMenu("GameObjects"))
            {
                ImGui::BeginDisabled(isRoot || isPrefabChild);
                if (ImGui::MenuItem("Add"))
                {
                    m_selected = MenuOption::AddGameObject;
                    m_popup = "Add GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add");
                }
                ImGui::EndDisabled();

                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Child"))
                {
                    m_selected = MenuOption::AddChildGameObject;
                    m_popup = "Add Child GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Child GameObject");
                }
                ImGui::EndDisabled();

                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Parent"))
                {
                    m_selected = MenuOption::AddParentGameObject;
                    m_popup = "Add Parent GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Parent GameObject");
                }
                ImGui::EndDisabled();

                ImGui::EndMenu();
            }
            //ImGui::EndDisabled();

            ImGui::Separator();

            //ImGui::BeginDisabled(isPrefab && (isRoot || isPrefabChild) && isPrefabOrPartOfPrefab);
            if (ImGui::BeginMenu("Prefabs"))
            {
                ImGui::BeginDisabled(!isPrefab && !isPrefabChild);

                if (ImGui::MenuItem("Edit"))
                {
                    auto engine = ImGuiWindow::getEngine();
                    auto worldRes = engine->GetWorldResource();
                    VG_ASSERT(worldRes);

                    auto prefabGameObject = gameObject;
                    if (isPrefabChild)
                        prefabGameObject = gameObject->GetParentPrefab();

                    if (prefabGameObject && prefabGameObject->GetPrefabResource())
                    {
                        auto scene = VG_SAFE_STATIC_CAST(IBaseScene, prefabGameObject->GetPrefabResource()->getObject());

                        IResource * sceneRes = worldRes->FindSceneResource(scene, BaseSceneType::Prefab);
                        
                        if (!sceneRes)
                        {
                            const string path = prefabGameObject->GetPrefabResource()->GetResourcePath();
                            worldRes->LoadSceneResource(path, BaseSceneType::Prefab);
                            engine->FlushLoading();
                        }

                        // open immediately
                        sceneRes = worldRes->FindSceneResource(scene, BaseSceneType::Prefab);
                        VG_ASSERT(sceneRes);
                        if (sceneRes)
                            Editor::get()->openPrefabView(sceneRes);
                    }
                }
                ImGui::EndDisabled();

                ImGui::Separator();

                ImGui::BeginDisabled(isRoot || isPrefabChild);
                if (ImGui::MenuItem("Add"))
                {
                    m_selected = MenuOption::AddPrefab;
                    m_popup = "Add Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add");
                }
                ImGui::EndDisabled();

                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Child"))
                {
                    m_selected = MenuOption::AddChildPrefab;
                    m_popup = "Add Child Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Child Prefab");
                }
                ImGui::EndDisabled();

                ImGui::EndMenu();
            }
            //ImGui::EndDisabled();

            ImGui::Separator();

            ImGui::BeginDisabled(!canRename);
            {
                if (ImGui::MenuItem("Rename", "Ctrl-R"))
                    doRename = true;
            }
            ImGui::EndDisabled();

            ImGui::BeginDisabled(!canDuplicate);
            {
                if (ImGui::MenuItem("Duplicate", "Ctrl-D"))
                    doDuplicate = true;
            }
            ImGui::EndDisabled();

            ImGui::Separator();

            ImGui::BeginDisabled(!canDelete);
            {
                if (ImGui::MenuItem("Delete", "Del"))
                    doDelete = true;
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();            
        }

        if (doDuplicate)
        {
            Editor::get()->getSelection()->DuplicateGameObjects(gameObjectsToDelete);
        }

        if (doDelete)
        {
            ImGui::OnMsgBoxClickedFunc deleteGameObject = [=]() mutable
            {
                for (uint i = 0; i < gameObjectsToDelete.size(); ++i)
                {
                    IGameObject * gameObjectToDelete = gameObjectsToDelete[i];
                    IGameObject * parentGameObject = dynamic_cast<IGameObject *>(gameObjectToDelete->getParent());
                    if (nullptr != parentGameObject)
                    {
                        parentGameObject->RemoveChild(gameObjectToDelete);
                        status = Status::Removed;
                    }
                }
                return true;
            };

            string msg;
            if (gameObjectsToDelete.size() > 1)
            {
                msg = "Are you sure you want to delete " + to_string(gameObjectsToDelete.size()) + " GameObjects and their children?";
            }
            else
            { 
                msg = "Are you sure you want to delete " + (string)gameObject->GetClassName() + " \"" + gameObject->getName() + "\"?";
            }
            ImGui::MessageBox(MessageBoxType::YesNo, "Delete GameObject", msg.c_str(), deleteGameObject);
        }

        if (doRename)
            m_RenamingGameObject = gameObject;

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup.c_str());
            openPopup = false;
        }
        
        if (m_popupObject == _object)
        {
            auto selected = (MenuOption)m_selected;
            switch (selected)
            {
                case MenuOption::AddChildPrefab:
                case MenuOption::AddPrefab:
                {
                    auto & fileBrowser = ImGuiWindow::getFileBrowser();
                    static char prefabPath[1024] = { '\0' };
                    bool pickPrefabFile = false;
                    bool addPrefab = false;

                    if (ImGui::BeginPopupModal(m_popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                            ImGui::SetKeyboardFocusHere(0);

                        ImGui::Text("Select prefab");

                        ImGui::PushItemWidth(384 - style::button::SizeSmall.x);

                        bool enterPressed = ImGui::InputText("###Path", prefabPath, countof(prefabPath), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

                        ImGui::PopItemWidth();

                        ImGui::SameLine();
                        auto x = ImGui::GetCursorPosX();
                        ImGui::SetCursorPosX(x + style::button::SizeSmall.x);
                        ImGui::Text("Path");
                        auto x2 = ImGui::GetCursorPosX();
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(x - 4);

                        if (ImGui::Button(style::icon::File, style::button::SizeSmall))
                            pickPrefabFile = true;

                        if (ImGui::Button("Add", style::button::SizeMedium) || enterPressed)
                        {
                            addPrefab = true;
                            ImGui::CloseCurrentPopup();
                            m_selected = MenuOption::None;
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel", style::button::SizeMedium))
                        {
                            ImGui::CloseCurrentPopup();
                            prefabPath[0] = '\0';
                            m_selected = MenuOption::None;
                        }

                        ImGui::EndPopup();
                    }

                    if (pickPrefabFile)
                    {
                        const auto defaultFolder = ImGuiWindow::getDefaultFolder("Prefabs");
                        fileBrowser.setFolder(defaultFolder);
                        ImGui::OpenPopup("Select Prefab");
                    }

                    const string ext = ".prefab";

                    if (fileBrowser.showFileDialog("Select Prefab", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, style::dialog::Size, ext))
                    {
                        const string newFile = io::getRelativePath(fileBrowser.selected_path);

                        if (strcmp(prefabPath, newFile.c_str()))
                            strcpy(prefabPath, newFile.c_str());

                        ImGui::OpenPopup(m_popup.c_str());
                    }

                    if (addPrefab)
                    {
                        //VG_INFO("[Prefab] Add Prefab \"%s\"", prefabPath);

                        auto newName = io::getFileNameWithoutExt(prefabPath);
                        IGameObject * newPrefabObject = (IGameObject *)CreateFactoryObject(PrefabGameObject, newName.c_str(), gameObject);
                        newPrefabObject->GetPrefabResource()->SetResourcePath(prefabPath);

                        switch (selected)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(selected);
                                break;

                            case MenuOption::AddPrefab:
                            {
                                auto parent = dynamic_cast<IGameObject *>(gameObject->getParent());
                                if (parent)
                                    parent->AddChild(newPrefabObject);
                            }
                            break;

                            case MenuOption::AddChildPrefab:
                                gameObject->SetObjectFlags(ObjectFlags::Opened, true);
                                gameObject->AddChild(newPrefabObject);
                                break;
                        }  

                        VG_SAFE_RELEASE(newPrefabObject);
                    }
                }
                break;

            case MenuOption::AddGameObject:
            case MenuOption::AddChildGameObject:
            case MenuOption::AddParentGameObject:
            {
                if (ImGui::BeginPopupModal(m_popup.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char nameTmp[1024] = { '\0' };

                    if (nameTmp[0] == '\0')
                        strcpy(nameTmp, "New GameObject");

                    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                        ImGui::SetKeyboardFocusHere(0);

                    bool enterPressed = ImGui::InputText("Name", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

                    string newName = nameTmp;

                    if (ImGui::Button("Add", style::button::SizeMedium) || enterPressed)
                    {
                        IGameObject * newGameObject = (IGameObject *)CreateFactoryObject(GameObject, newName.c_str(), gameObject);

                        switch (selected)
                        {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(selected);
                            break;

                        case MenuOption::AddGameObject:
                        {
                            auto parent = dynamic_cast<IGameObject *>(gameObject->getParent());
                            if (parent)
                                parent->AddChild(newGameObject);
                        }
                        break;

                        case MenuOption::AddChildGameObject:
                            gameObject->SetObjectFlags(ObjectFlags::Opened, true);
                            gameObject->AddChild(newGameObject);
                            break;

                        case MenuOption::AddParentGameObject:
                            auto parent = dynamic_cast<IGameObject *>(gameObject->getParent());
                            if (parent)
                            {
                                parent->AddChild(newGameObject);
                                VG_SAFE_INCREASE_REFCOUNT(gameObject);
                                parent->RemoveChild(gameObject);
                                newGameObject->AddChild(gameObject);
                                VG_SAFE_RELEASE(gameObject);
                            }
                            else if (core::IBaseScene * scene = dynamic_cast<core::IBaseScene *>(gameObject->getParent()))
                            {
                                // Object is root node, create a new root node and add this a its child
                                VG_SAFE_INCREASE_REFCOUNT(gameObject);
                                newGameObject->setParent(scene);
                                scene->SetRoot(newGameObject);
                                newGameObject->AddChild(gameObject);
                                VG_SAFE_RELEASE(gameObject);
                            }
                            break;
                        }

                        newGameObject->Release();
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
            }
            break;
            }
        }
        return status;
    }
}