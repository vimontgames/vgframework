#include "ImGuiGameObjectSceneEditorMenu.h"
#include "core/IGameObject.h"
#include "core/IResource.h"
#include "core/ISelection.h"
#include "core/IInput.h"
#include "core/IResourceManager.h"
#include "core/IBaseScene.h"
#include "core/File/File.h"
#include "engine/IEngine.h"
#include "editor/ImGui/Window/ImGuiWindow.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h"
#include "editor/ImGui/Extensions/FileDialog/ImGuiFileDialog.h"
#include "editor/ImGui/Window/View/ImGuiView.h"
#include "editor/Editor.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "engine/IWorldResource.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiGameObjectSceneEditorMenu::Display(core::IObject * _object)
    {
        auto engine = ImGuiWindow::getEngine();

        auto status = Status::None;
        IGameObject * gameObject = dynamic_cast<IGameObject*>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;
        bool doDelete = false;
        bool doFocus = false;
        bool doRename = false;
        bool doDuplicate = false;
        bool doUnpack = false;

        ISelection * selection = Editor::get()->getSelection();

        // The copy here is *intentional*
        auto selectedObjects = selection->GetSelectedObjects(); 

        // Add the clicked object to "selection" if not already selected
        const bool isRoot = gameObject->IsRoot();
        if (!isRoot && !selection->IsSelectedObject(gameObject))
            selectedObjects.push_back(gameObject);

        auto * editor = Editor::get();
        vector<IGameObject *> topLevelGameObjects = editor->getSelection()->RemoveChildGameObjectsWithParents(selectedObjects);

        const bool isPrefab = gameObject->IsPrefab();
        const bool isPrefabChild = gameObject->GetParentPrefab();
        const bool isPrefabOrPartOfPrefab = isPrefab || isPrefabChild;

        // Root GameObject cannot be deleted
        const bool canAddChildGameObject = !isPrefabOrPartOfPrefab;
        const bool canAddChildPrefab = !isPrefabOrPartOfPrefab;
        const bool canDelete = topLevelGameObjects.size() > 0 && !isRoot;
        const bool canRename = true; // !isRoot;
        const bool canDuplicate = !isRoot;

        if (m_RenamingGameObject == nullptr)
        {
            if (selection->IsSelectedObject(gameObject)) 
            {
                // What if the Menu is closed? This code should run in Editor/Toolmode 3D View update instead
                if (!ImGui::IsAnyItemActive())
                {
                    /*if (ImGui::IsKeyPressed(ImGuiKey_F))
                        doFocus = true;
                    else if (ImGui::IsKeyPressed(ImGuiKey_Delete) && canDelete)
                        doDelete = true;
                    else */if (canRename && ImGui::IsKeyPressed(ImGuiKey_F2) && canRename)
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
                //AddGameObject
                ImGui::BeginDisabled(isRoot || isPrefabChild);
                if (ImGui::MenuItem("Add"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::AddGameObject;
                    m_popup = "Add GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add GameObject");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add new GameObject at the same level as the selected GameObject");

                // AddChildGameObject
                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Child"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::AddChildGameObject;
                    m_popup = "Add Child GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Child GameObject");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add new GameObject as child of the selected GameObject");

                // AddParentGameObject
                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Parent"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::AddParentGameObject;
                    m_popup = "Add Parent GameObject";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Parent GameObject");
                }
                ImGui::EndDisabled();

                if(ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add new GameObject as parent of the selected GameObject");

                ImGui::EndMenu();
            }
            //ImGui::EndDisabled();

            ImGui::Separator();

            //ImGui::BeginDisabled(isPrefab && (isRoot || isPrefabChild) && isPrefabOrPartOfPrefab);
            if (ImGui::BeginMenu("Prefabs"))
            {
                // EditPrefab
                ImGui::BeginDisabled(!isPrefab && !isPrefabChild);
                if (ImGui::MenuItem("Edit"))
                {
                    auto worldRes = engine->GetWorldResource();
                    VG_ASSERT(worldRes);

                    auto prefabGameObject = gameObject;
                    if (isPrefabChild)
                        prefabGameObject = gameObject->GetParentPrefab();

                    if (prefabGameObject && prefabGameObject->GetPrefabResource())
                    {
                        auto scene = VG_SAFE_STATIC_CAST(IBaseScene, prefabGameObject->GetPrefabResource()->GetObject());

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
                            editor->openPrefabView(sceneRes);
                    }
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Edit selected Prefab in a separate view");

                ImGui::Separator();

                // CreatePrefab
                ImGui::BeginDisabled(isPrefab || isPrefabChild);
                if (ImGui::MenuItem("Create"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::CreatePrefab;
                    m_popup = "Create Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Create Prefab");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Create a new Prefab file from existing GameObject");

                // UnpackPrefab
                ImGui::BeginDisabled(!isPrefab && !isPrefabChild);
                if (ImGui::MenuItem("Unpack"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::UnpackPrefab;
                    m_popup = "Unpack Prefab";
                    m_popupObject = _object;
                    doUnpack = true;
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Unpack Prefab into regular GameObject");

                ImGui::Separator();

                // AddPrefab
                ImGui::BeginDisabled(isRoot || isPrefabChild);
                if (ImGui::MenuItem("Add"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::AddPrefab;
                    m_popup = "Add Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Prefab");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add Prefab from file at the same level as the selected GameObject");

                // AddChildPrefab
                ImGui::BeginDisabled(isPrefabOrPartOfPrefab);
                if (ImGui::MenuItem("Add Child"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::AddChildPrefab;
                    m_popup = "Add Child Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Add Child Prefab");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add Prefab from file as child of the selected GameObject");

                // ReplaceByPrefab
                ImGui::BeginDisabled(isRoot || isPrefab || isPrefabChild);
                if (ImGui::MenuItem("Replace"))
                {
                    m_selected = (uint)GameObjectSceneEditorMenuOption::ReplaceByPrefab;
                    m_popup = "Replace by Prefab";
                    m_popupObject = _object;
                    openPopup = true;
                    ImGui::OpenPopup("Replace by Prefab");
                }
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Replace selected GameObject with Prefab");

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

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Rename selected GameObject");

            ImGui::BeginDisabled(!canDuplicate);
            {
                if (ImGui::MenuItem("Duplicate", "Ctrl-D"))
                    doDuplicate = true;
            }
            ImGui::EndDisabled();

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Create a copy of selected GameObject");

            ImGui::Separator();

            if (ImGui::MenuItem("Focus", "F"))
                editor->focus(gameObject);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Focus GameObject in Editor View");

            ImGui::BeginDisabled(!canDelete);
            {
                if (ImGui::MenuItem("Delete", "Del"))
                    doDelete = true;
            }
            ImGui::EndDisabled();

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Delete selected GameObject");

            ImGui::EndPopup();            
        }

        if (doDuplicate)
            editor->getSelection()->DuplicateGameObjects(topLevelGameObjects);

        if (doUnpack)
        {
            IGameObject * parentPrefabGO = gameObject->GetParentPrefab();
            IGameObject * prefabGO;
            if (parentPrefabGO)
                prefabGO = parentPrefabGO;
            else
                prefabGO = gameObject;

            if (nullptr != prefabGO)
            {
                VG_ASSERT(prefabGO->IsPrefab());

                ImGui::OnMsgBoxClickedFunc unpackPrefab = [=]() mutable
                {
                    auto * prefabRes = prefabGO->GetPrefabResource();
                    IGameObject * prefabParent = VG_SAFE_STATIC_CAST(IGameObject, prefabGO->GetParent());
                    
                    auto srcChildren = prefabGO->GetChildren();

                    if (srcChildren.size() > 0)
                    {
                        IGameObject * root = srcChildren[0];
                        VG_SAFE_INCREASE_REFCOUNT(root);
                        root->SetParent(nullptr);
                        root->SetName(prefabGO->GetName());
                        root->SetLocalMatrix(prefabGO->GetLocalMatrix());
                        prefabParent->AddChild(root);
                        VG_SAFE_RELEASE(root);
                    }
                    prefabGO->RemoveAllChildren();
                    prefabParent->RemoveChild(prefabGO);

                    return true;
                };

                string msg = fmt::sprintf("Are you sure you want to unpack Prefab instance \"%s\" of \"%s\"?", prefabGO->GetName(), prefabGO->GetPrefabResource()->GetResourcePath());
                ImGui::MessageBox(MessageBoxType::YesNo, "Unpack Prefab", msg.c_str(), unpackPrefab);
            }
        }

        if (doFocus)
            editor->focus(gameObject);

        if (doDelete)
        {
            editor->deleteGameObjects(topLevelGameObjects);
            status = Status::Removed;
        }

        if (doRename)
            m_RenamingGameObject = gameObject;

        if (openPopup)
        {
            ImGui::OpenPopup(m_popup.c_str());
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            openPopup = false;
        }
        
        if (m_popupObject == _object)
        {
            auto selected = (GameObjectSceneEditorMenuOption)m_selected;
            switch (selected)
            {
                case GameObjectSceneEditorMenuOption::AddChildPrefab:
                case GameObjectSceneEditorMenuOption::AddPrefab:
                case GameObjectSceneEditorMenuOption::CreatePrefab:
                case GameObjectSceneEditorMenuOption::ReplaceByPrefab:
                {
                    const string ext = ".prefab";

                    static char prefabPath[1024] = { '\0' };
                    bool pickPrefabFile = false;
                    bool addPrefab = false;

                    if (selected == GameObjectSceneEditorMenuOption::CreatePrefab && prefabPath[0] == '\0')
                        sprintf_s(prefabPath, "%s/%s.prefab", io::getRelativePath(ImGuiWindow::getDefaultFolder("Prefabs")).c_str(), _object->GetName().c_str());

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
                            m_selected = (uint)GameObjectSceneEditorMenuOption::None;
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel", style::button::SizeMedium))
                        {
                            ImGui::CloseCurrentPopup();
                            prefabPath[0] = '\0';
                            m_selected = (uint)GameObjectSceneEditorMenuOption::None;
                        }

                        ImGui::EndPopup();
                    }

                    const string selectPrefabString = "Select Prefab";

                    if (pickPrefabFile)
                    {
                        ImGui::OpenFileDialog(selectPrefabString, ext, ImGuiWindow::getDefaultFolder("Prefabs"));
                    }
                    
                    if (ImGui::DisplayFileDialog(selectPrefabString))
                    {
                        if (ImGui::IsFileDialogOK())
                        {
                            const string newFile = io::getRelativePath(ImGui::GetFileDialogSelectedFile());

                            if (strcmp(prefabPath, newFile.c_str()))
                                strcpy(prefabPath, newFile.c_str());

                            ImGui::OpenPopup(m_popup.c_str());
                        }

                        ImGui::CloseFileDialog();
                    }

                    if (addPrefab)
                    {
                        //VG_INFO("[Prefab] Add Prefab \"%s\"", prefabPath);

                        auto newName = io::getFileNameWithoutExt(prefabPath);
                        IGameObject * newPrefabObject = (IGameObject *)CreateFactoryObject(PrefabGameObject, newName.c_str(), gameObject);
                        newPrefabObject->RegisterUID();
                        auto * prefabRes = newPrefabObject->GetPrefabResource();

                        switch (selected)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(selected);
                                break;

                            case GameObjectSceneEditorMenuOption::ReplaceByPrefab:
                            {
                                prefabRes->SetResourcePath(prefabPath);
                                auto parent = dynamic_cast<IGameObject *>(gameObject->GetParent());
                                VG_ASSERT(parent);
                                if (parent)
                                {
                                    const auto index = parent->GetChildIndex(gameObject);
                                    parent->AddChild(newPrefabObject, index+1);
                                    parent->RemoveChild(gameObject);
                                    status = Status::Removed;
                                }
                            }
                            break;

                            case GameObjectSceneEditorMenuOption::AddPrefab:
                            {
                                prefabRes->SetResourcePath(prefabPath);
                                auto parent = dynamic_cast<IGameObject *>(gameObject->GetParent());
                                VG_ASSERT(parent);
                                if (parent)
                                    parent->AddChild(newPrefabObject);
                            }
                            break;

                            case GameObjectSceneEditorMenuOption::AddChildPrefab:
                            {
                                prefabRes->SetResourcePath(prefabPath);
                                gameObject->SetObjectFlags(ObjectFlags::Opened, true);
                                gameObject->AddChild(newPrefabObject);
                            }
                            break;

                            case GameObjectSceneEditorMenuOption::CreatePrefab:
                            {
                                auto parent = dynamic_cast<IGameObject *>(gameObject->GetParent());
                                VG_ASSERT(parent);
                                if (parent)
                                {
                                    VG_SAFE_INCREASE_REFCOUNT(gameObject);
                                    uint index = parent->GetChildIndex(gameObject);
                                    parent->RemoveChild(gameObject);
                                    newPrefabObject->SetLocalMatrix(gameObject->GetLocalMatrix());
                                    gameObject->SetLocalMatrix(float4x4::identity());

                                    parent->SetObjectFlags(ObjectFlags::Opened, true);
                                    parent->AddChild(newPrefabObject, index);

                                    prefabRes->CreateFile(prefabPath, gameObject);
                                    VG_SAFE_RELEASE(gameObject); 

                                    prefabRes->ClearResourcePath();
                                    prefabRes->SetResourcePath(prefabPath);
                                    //engine->GetResourceManager()->Reimport(prefabRes);

                                    prefabPath[0] = '\0';
                                    status = Status::Removed;
                                }
                            }
                            break;
                        }  

                        selection->SetSelectedObject(newPrefabObject);

                        VG_SAFE_RELEASE(newPrefabObject);
                    }
                }
                break;

            case GameObjectSceneEditorMenuOption::AddGameObject:
            case GameObjectSceneEditorMenuOption::AddChildGameObject:
            case GameObjectSceneEditorMenuOption::AddParentGameObject:
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

                        case GameObjectSceneEditorMenuOption::AddGameObject:
                        {
                            auto parent = dynamic_cast<IGameObject *>(gameObject->GetParent());
                            if (parent)
                                parent->AddChild(newGameObject);
                        }
                        break;

                        case GameObjectSceneEditorMenuOption::AddChildGameObject:
                            gameObject->SetObjectFlags(ObjectFlags::Opened, true);
                            gameObject->AddChild(newGameObject);
                            break;

                        case GameObjectSceneEditorMenuOption::AddParentGameObject:
                            auto parent = dynamic_cast<IGameObject *>(gameObject->GetParent());
                            if (parent)
                            {
                                parent->AddChild(newGameObject);
                                VG_SAFE_INCREASE_REFCOUNT(gameObject);
                                parent->RemoveChild(gameObject);
                                newGameObject->AddChild(gameObject);
                                VG_SAFE_RELEASE(gameObject);
                            }
                            else if (core::IBaseScene * scene = dynamic_cast<core::IBaseScene *>(gameObject->GetParent()))
                            {
                                // Object is root node, create a new root node and add this a its child
                                VG_SAFE_INCREASE_REFCOUNT(gameObject);
                                newGameObject->SetParent(scene);
                                scene->SetRoot(newGameObject);
                                newGameObject->AddChild(gameObject);
                                VG_SAFE_RELEASE(gameObject);
                            }
                            break;
                        }

                        selection->SetSelectedObject(newGameObject);
                        newGameObject->Release();
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = (uint)GameObjectSceneEditorMenuOption::None;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", style::button::SizeMedium))
                    {
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        m_selected = (uint)GameObjectSceneEditorMenuOption::None;
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