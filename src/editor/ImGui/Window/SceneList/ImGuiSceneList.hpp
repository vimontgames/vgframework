#include "ImGuiSceneList.h"
#include "editor/ImGui/ImGui.h"
#include "engine/IWorldResource.h"
#include "core/IGameObject.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiSceneList::ImGuiSceneList(BaseSceneType _sceneType, const core::string& _icon, const core::string& _path, const core::string& _name, Flags _flags) :
        ImGuiWindow(_icon, _path, _name, _flags),
        m_sceneMenu(_sceneType)
    {
        getSelection()->RegisterSelectionChangedCallback(this, onSelectionChanged);
    }

    //--------------------------------------------------------------------------------------
    ImGuiSceneList ::~ImGuiSceneList()
    {
        getSelection()->UnregisterSelectionChangedCallback(this, onSelectionChanged);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiSceneList::onSelectionChanged(core::IObject * _this, core::SelectionChangeType _change)
    {
        ImGuiSceneList * sceneList = dynamic_cast<ImGuiSceneList *>(_this);
        VG_ASSERT(nullptr != sceneList);

        if (nullptr != sceneList)
        {
            auto & selectedObjects = getSelection()->GetSelectedObjects();
            core::vector<IGameObject *> selectedGameObjects;
            selectedGameObjects.reserve(selectedObjects.size());

            switch (_change)
            {
                case core::SelectionChangeType::Add:
                {
                    for (uint i = 0; i < selectedObjects.size(); ++i)
                    {
                        if (IGameObject * go = dynamic_cast<IGameObject *>(selectedObjects[i]))
                            selectedGameObjects.push_back(go);
                    }

                    if (selectedGameObjects.size() > 0)
                        sceneList->focus(selectedGameObjects);
                }
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool doDragDrop(IGameObject* _from, IGameObject* _to, DragAndDropType _dest)
    {
        if (_from == _to)
        {
            //VG_WARNING("[Scene] Cannot move GameObject onto himself");
            return false;
        }

        // Moved not cannot became a child of its own child
        if (_to->HasAncestor(_from))
        {
            //VG_WARNING("[Scene] Cannot move GameObject because it would become a child of its own children");
            return false;
        }

        switch (_dest)
        {
            case DragAndDropType::Node:
            {
                // '_to' is the new parent of '_from'
                IGameObject* fromParent = dynamic_cast<IGameObject*>(_from->GetParent());
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

            case DragAndDropType::AfterNode:
            {
                // '_from' is not the next following brother of '_to'
                IGameObject* fromParent = dynamic_cast<IGameObject*>(_from->GetParent());
                if (fromParent)
                {
                    IGameObject* toParent = dynamic_cast<IGameObject*>(_to->GetParent());
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
    bool isPassingFilterRecur(const ImGuiTextFilter & _filter, const IGameObject * _gameObject)
    {
        if (_filter.PassFilter(_gameObject->GetName().c_str()))
            return true;

        const auto & children = _gameObject->GetChildren();
        for (uint i = 0; i < children.size(); ++i)
        {
            if (isPassingFilterRecur(_filter, children[i]))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiSceneList::displayGameObject(IGameObject* _gameObject)
    {
        // Skip object if not matching filter and not of his children is passing filter
        if (!isPassingFilterRecur(m_filter, _gameObject))
            return;

        const auto children = _gameObject->GetChildren();
        const auto adapter = Editor::get()->getRenderer()->GetImGuiAdapter();

        // Focus 1st selected element
        if (vector_helper::exists(m_focusedGameObjects, _gameObject))
        {
            m_focusedGameObjects.clear();
            ImGui::SetScrollHereY(0.0f);
        }

        m_count++;

        const bool disabled = !_gameObject->IsEnabledInHierarchy();

        bool open = false;

        // This is actually full window width we want here
        #if 1 // IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        //auto availableWidth = GetContentRegionAvail().x;
        float availableWidth = ImGui::GetCurrentWindow()->WorkRect.GetWidth();
        #else
        auto availableWidth = ImGui::GetContentRegionMax().x;
        #endif

        const bool isPrefab = _gameObject->IsPrefab();
        const bool isPrefabChild = !isPrefab && nullptr != _gameObject->GetParentPrefab();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        if (children.size() > 0)
            flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        else
            flags |= ImGuiTreeNodeFlags_Leaf;

        // row background
        {
            u32 rowBgColor;

            if (m_count & 1)
                rowBgColor = GetColorU32(adapter->GetRowColorEven());
            else
                rowBgColor = GetColorU32(adapter->GetRowColorOdd());

            DrawRowsBackground(1, rowBgColor);
        }

        if (getSelection()->IsSelectedObject(_gameObject))
        {
            const auto selColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
            DrawRowsBackground(1, GetColorU32(selColor));
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        ImVec4 textColor = adapter->GetTextColor();

        if (_gameObject == m_dragAndDropNodeTarget)
        {
            const auto selColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive);
            DrawRowsBackground(1, GetColorU32(selColor));
        }

        auto pos = ImGui::GetCursorPos();

        if (isPrefab)
        {
            auto prefabColor = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
            textColor.x = prefabColor.x;
            textColor.y = prefabColor.y;
            textColor.z = prefabColor.z;
            ImGui::PushStyle(renderer::FontStyle::Bold);
        }
        else if (isPrefabChild)
        {
            ImGui::PushStyle(renderer::FontStyle::Italic);
        }

        const float disabledAlpha = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).w;
        
        if (disabled)
            textColor.w = disabledAlpha;

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);

        const bool isOpen = asBool(ObjectFlags::Opened & _gameObject->GetObjectFlags());
        ImGui::SetNextItemOpen(isOpen);

        const bool renaming = m_gameObjectMenu.m_RenamingGameObject == _gameObject;
        if (renaming)
        {
            char gameObjectLabel[256];
            sprintf_s(gameObjectLabel, "###%p", _gameObject);
            open = ImGui::TreeNodeEx(gameObjectLabel, flags);
            ImGui::SameLine();
            char temp[256];
            sprintf_s(temp, _gameObject->GetName().c_str());
            sprintf_s(gameObjectLabel, "###Ren%p", _gameObject);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
            {
                if (ImGui::InputText(gameObjectLabel, temp, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    _gameObject->SetName(temp);
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
            char gameObjectLabel[256];
            sprintf(gameObjectLabel, "%s###%p", _gameObject->GetName().c_str(), (void *)_gameObject);
            open = ImGui::TreeNodeEx(gameObjectLabel, flags | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth);
        }

        if (IsMouseDoubleClicked(ImGuiMouseButton_Left) && IsItemHovered())
            Editor::get()->focus(_gameObject);

        // Do not automatically open Leaf nodes when children are added
        if (0 == (flags & ImGuiTreeNodeFlags_Leaf) && open != isOpen)
            _gameObject->SetObjectFlags(ObjectFlags::Opened, open);

        if (isPrefab)
            ImGui::PopStyle();
        else if (isPrefabChild)
            ImGui::PopStyle();

        bool startDragDrop = false;

        // Treenode receiving drag'n drop
        if (ImGui::BeginDragDropTarget())
        {
            ImGuiDragDropFlags target_flags = 0;
            target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
            target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
            {
                IGameObject* from = *(IGameObject**)payload->Data;
                IGameObject* to = _gameObject;

                if (payload->Delivery)
                {
                    doDragDrop(from, to, DragAndDropType::Node);
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
            updateSelection(_gameObject);

        ImGui::PushStyleColor(ImGuiCol_Text, adapter->GetTextColor());
        {
            if (ImGuiGameObjectSceneEditorMenu::Status::Removed == m_gameObjectMenu.Display(_gameObject))
                _gameObject = nullptr;
        }
        ImGui::PopStyleColor();

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
        {
            //VG_INFO("BeginDragDropSource");
        
            //if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
            //    ImGui::Text("Moving \"%s\"", _gameObject->getName().c_str());
            ImGui::SetDragDropPayload("GameObject", &_gameObject, sizeof(IGameObject*));
            ImGui::EndDragDropSource();
        }
        
        // Invisible selectable for interline
        if (_gameObject)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style::draganddrop::interlineSize.y - 1);
                string beforeGO = fmt::sprintf("###after %s%p", _gameObject->GetName().c_str(), (void *)_gameObject);
                ImGui::InvisibleButton(beforeGO.c_str(), style::draganddrop::interlineSize);
        
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
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
            {
                IGameObject* from = *(IGameObject**)payload->Data;
                IGameObject* to = _gameObject;
        
                //VG_INFO("Moving from \"%s\" to after \"%s\"", from->getName().c_str(), to->getName().c_str());
        
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                float x0 = ImGui::GetCursorScreenPos().x;
                float y0 = ImGui::GetCursorScreenPos().y - style::draganddrop::interlineSize.y / 2;
                float x1 = x0 + 1024;
                float y1 = y0 + style::draganddrop::interlineSize.y / 2;
        
                const auto interlineColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive);
                u32 intColor = GetColorU32(interlineColor);
                draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), intColor);
        
                if (payload->Delivery)
                {
                    doDragDrop(from, to, DragAndDropType::AfterNode);
                    m_dragAndDropInterlineTarget = nullptr;
                }
                else
                    m_dragAndDropInterlineTarget = to;
        
                m_dragAndDropNodeTarget = nullptr;
            }
        
            ImGui::EndDragDropTarget();
        }
        
        // icons
        if (_gameObject)
        {
            const auto & components = _gameObject->GetComponents();
            auto bakePos = ImGui::GetCursorPos();
            ImGui::SameLine();

            auto drawIcon = [=](const char * _icon, uint _enabledCount, uint _disabledCount, float & _totalSize, float _availableWidth, const ImVec2 & _pos, const string & _tooltip)
            {
                VG_ASSERT(nullptr != _icon);

                const auto & style = ImGui::GetStyle();

                ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

                if (_enabledCount != 0 && _disabledCount != 0) // mixed
                {
                    textColor.w *= (0.5f + 0.5f * disabledAlpha);
                }
                else
                {
                    if (_enabledCount > 0)
                        textColor.w *= 1;
                    else
                        textColor.w *= disabledAlpha;
                }

                auto size = ImGui::CalcTextSize(_icon).x;
                _totalSize += size + style.FramePadding.x;

                ImGui::SetCursorPosX(_availableWidth - _totalSize);
                ImGui::SetCursorPosY(_pos.y);
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);

                ImGui::Text(_icon);

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    if ((_enabledCount + _disabledCount) > 1)
                    {
                        string tooltip = fmt::sprintf("%s (x%u)", _tooltip, _enabledCount + _disabledCount);
                        ImGui::SetTooltip(tooltip.c_str());
                    }
                    else
                    {
                        ImGui::SetTooltip(_tooltip.c_str());
                    }
                }

                ImGui::PopStyleColor();
            };

#if 1
            vector<const char*> componentIcons;
            for (uint i = 0; i < (uint)components.size(); ++i)
            {
                const auto * component = components[i];
                const auto * componentClassDesc = component->GetClassDesc();
                auto icon = componentClassDesc->GetIcon();
                if (nullptr != icon)
                {
                    if (!vector_helper::exists(componentIcons, icon))
                        componentIcons.push_back(icon);
                }
            }

            float totalSize = 0;
            for (int i = (int)componentIcons.size() - 1; i >= 0; --i)
            {
                auto compIcon = componentIcons[i];

                uint enabledCount = 0, disabledCount = 0;

                const IClassDesc * componentClassDesc = nullptr;

                for (uint j = 0; j < components.size(); ++j)
                {
                    const auto * component = components[j];
                    auto icon = component->GetClassDesc()->GetIcon();
                    if (nullptr != icon)
                    {
                        if (icon == compIcon)
                        {
                            const bool enabled = asBool(ComponentFlags::Enabled & component->GetComponentFlags());
                            componentClassDesc = component->GetClassDesc();

                            if (enabled)
                                enabledCount++;
                            else
                                disabledCount++;
                        }
                    }
                }

                if (enabledCount || disabledCount)
                {
                    drawIcon(compIcon, enabledCount, disabledCount, totalSize, availableWidth, pos, componentClassDesc->GetDescription());
                }
            }

#else
            float totalSize = 0;
            for (i64 i = components.size() - 1; i >= 0; --i)
            {
                const auto * component = components[i];
                const auto * componentClassDesc = component->GetClassDesc();
                auto icon = componentClassDesc->GetIcon();
                if (nullptr != icon)
                {
                    const bool enabled = asBool(ComponentFlags::Enabled & component->GetComponentFlags());
                    drawIcon(icon, enabled ? 1 : 0, enabled ? 0 : 1, totalSize, availableWidth, pos, componentClassDesc->GetDescription());
                }
            }
#endif
            if (isPrefab)
            {
                bool enabled = asBool(InstanceFlags::Enabled & _gameObject->GetInstanceFlags());
                drawIcon(editor::style::icon::Prefab, enabled ? 1 : 0, enabled ? 0 : 1, totalSize, availableWidth, pos, "Prefab");
            }

            ImGui::SetCursorPosX(bakePos.x);
            ImGui::SetCursorPosY(bakePos.y);

            if (open)
            {
                for (uint j = 0; j < children.size(); ++j)
                {
                    IGameObject * child = children[j];
                    displayGameObject(child);
                }
            }
        }

        if (open)
        {
            ImGui::TreePop();
        }

        ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------------------
    ImGuiSceneList::SceneTypeInfo ImGuiSceneList::getGameObjectTreeTypeInfo(core::BaseSceneType _sceneType)
    {
        SceneTypeInfo info;

        switch (_sceneType)
        {
            case BaseSceneType::Scene:
                info.icon = style::icon::Scene;
                info.windowName = "Scenes";
                info.newLabel = "New Scene";
                info.loadLabel = "Load Scene";
                info.dataFolder = "data/Scenes";
                info.fileExt = ".scene";
                break;

            case BaseSceneType::Prefab:
                info.icon = style::icon::Prefab;
                info.windowName = "Prefabs";
                info.newLabel = "New Prefab";
                info.loadLabel = "Load Prefab";
                info.dataFolder = "data/Prefabs";
                info.fileExt = ".prefab";
                break;
        }

        return info;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiSceneList::focus(const core::vector<core::IGameObject * > & _gameObjects)
    {
        if (_gameObjects.size() > 0)
        {
            engine::IEngine * engine = getEngine();
            const engine::IWorldResource * worldRes = engine->GetWorldResource();
            const IWorld * world = worldRes ? worldRes->GetWorld() : nullptr;
            const auto sceneType = GetSceneType();
            auto sceneResourceCount = worldRes->GetSceneResourceCount(sceneType);
            for (uint i = 0; i < sceneResourceCount; ++i)
            {
                const core::IResource * sceneRes = worldRes->GetSceneResource(i, sceneType);
                const IBaseScene * scene = (IBaseScene *)sceneRes->GetObject();
                if (nullptr != scene)
                {
                    if (_gameObjects[_gameObjects.size() - 1]->GetScene() == scene)
                    {
                        if (auto * window = FindWindowByName(getWindowTitle().c_str()))
                        {
                            if (window->Hidden)
                                m_stealFocus = true;
                        }
                        break;
                    }
                }
            }
        }

        m_focusedGameObjects = _gameObjects;
    }

    //--------------------------------------------------------------------------------------
    string ImGuiSceneList::getWindowTitle() const
    {
        auto worldRes = getEngine()->GetWorldResource();
        string label = worldRes && worldRes->GetObject() ? worldRes->GetObject()->GetName() : "<No World loaded>";
        const auto sceneType = GetSceneType();
        const auto typeInfo = getGameObjectTreeTypeInfo(sceneType);
        const string sceneTypeName = asString(sceneType);
        return fmt::sprintf("%s %s###%s", typeInfo.icon, typeInfo.windowName, sceneTypeName);
    }

    const char * g_previousFocusedWindowName = nullptr;

    //--------------------------------------------------------------------------------------
    void ImGuiSceneList::display()
    {
        const auto sceneType = GetSceneType();
        const string sceneTypeName = asString(sceneType);
        const auto typeInfo = getGameObjectTreeTypeInfo(sceneType);

        ImGui::PushID(sceneTypeName.c_str());

        auto worldRes = getEngine()->GetWorldResource();
        string label = worldRes && worldRes->GetObject() ? worldRes->GetObject()->GetName() : "<No World loaded>";

        if (m_stealFocus)
        {
            ImGuiContext & g = *ImGui::GetCurrentContext(); 

            g_previousFocusedWindowName = g.NavWindow->Name;

            if (auto * window = FindWindowByName(getWindowTitle().c_str()))
                ImGui::FocusWindow(window);

            m_stealFocus = false;
        }

        if (ImGui::Begin(getWindowTitle().c_str(), &m_isVisible))
        {
            bool newScene = false, loadScene = false;

            if (ImGui::TooltipButton(fmt::sprintf("%s %s", editor::style::icon::New, typeInfo.newLabel).c_str(), true, true, typeInfo.newLabel.c_str(), style::button::SizeLarge))
                newScene = true;

            ImGui::SameLine();

            if (ImGui::TooltipButton(fmt::sprintf("%s %s", editor::style::icon::Load, typeInfo.loadLabel).c_str(), true, true, typeInfo.loadLabel.c_str(), style::button::SizeLarge))
                loadScene = true;

            m_filter.Draw("Filter");

            ImGui::Separator();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                const auto* factory = Kernel::getFactory();
                engine::IEngine* engine = (engine::IEngine*)factory->GetSingleton("Engine");

                engine::IWorldResource* worldRes = engine->GetWorldResource();
                IWorld* world = worldRes ? worldRes->GetWorld() : nullptr;

                if (world)
                {
                    bool openPopup = false;
                    bool openFileDialog = false;

                    if (ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::MenuItem(typeInfo.newLabel.c_str()))
                            newScene = true;

                        if (ImGui::MenuItem(typeInfo.loadLabel.c_str()))
                            loadScene = true;

                        ImGui::EndPopup();
                    }

                    if (newScene)
                    {
                        m_selected = (uint)SceneListMenuOption::AddScene;
                        openFileDialog = true;
                        m_popup = typeInfo.newLabel;
                    }

                    if (loadScene)
                    {
                        m_selected = (uint)SceneListMenuOption::LoadScene;
                        openFileDialog = true;
                        m_popup = typeInfo.loadLabel;
                    }

                    if (openPopup)
                    {
                        ImGui::OpenPopup(m_popup.c_str());
                        openPopup = false;
                    }
                    else if (openFileDialog)
                    {
                        ImGui::OpenFileDialog(m_popup, typeInfo.fileExt, typeInfo.dataFolder);
                        openFileDialog = false;
                    }

                    string ext = ImGuiWindow::getFileBrowserExt(worldRes);

                    switch ((SceneListMenuOption)m_selected)
                    {
                    case SceneListMenuOption::AddScene:
                    {
                        if (ImGui::DisplayFileDialog(m_popup))
                        {
                            if (ImGui::IsFileDialogOK())
                                worldRes->CreateSceneResource(ImGui::GetFileDialogSelectedFile(), sceneType);

                            ImGui::CloseFileDialog();
                        }
                    }
                    break;

                    case SceneListMenuOption::LoadScene:
                        if (ImGui::DisplayFileDialog(m_popup))
                        {
                            if (ImGui::IsFileDialogOK())
                                worldRes->LoadSceneResource(ImGui::GetFileDialogSelectedFile(), sceneType);

                            ImGui::CloseFileDialog();
                        }
                        break;
                    }

                    auto availableWidth = ImGui::GetContentRegionAvail().x;

                    m_count = 0;

                    auto sceneResourceCount = worldRes->GetSceneResourceCount(sceneType);
                    for (uint i = 0; i < sceneResourceCount; ++i)
                    {
                        const core::IResource* sceneRes = worldRes->GetSceneResource(i, sceneType);
                        const IBaseScene* scene = (IBaseScene*)sceneRes->GetObject();
                        if (nullptr != scene)
                        {
                            ImGui::PushID("SceneTree");

                            IGameObject* root = scene->GetRoot();
                            auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen;
                            if (nullptr != root)
                                flags |= ImGuiTreeNodeFlags_None;
                            else
                                flags |= ImGuiTreeNodeFlags_Leaf;

                            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

                            const bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", scene->GetName(), scene).c_str(), flags);

                            m_count++;

                            bool enabled = asBool(InstanceFlags::Enabled & root->GetInstanceFlags());

                            auto status = m_sceneMenu.Display((IObject*)sceneRes);
                            
                            if (ImGuiMenu::Status::Removed == status)
                            {
                                sceneResourceCount--;
                                i--;
                            }
                            else
                            {
                                // Check in case menu just unloaded the scene (e.g., "Save & Update")
                                if (nullptr != sceneRes->GetObject())
                                {
                                    string sceneLabel = fmt::sprintf("%s %s", typeInfo.icon, scene->GetName());

                                    ImGui::CollapsingHeaderLabel(collapsingHeaderPos, sceneLabel.c_str(), enabled);

                                    if (ImGui::CollapsingHeaderCheckbox(collapsingHeaderPos, enabled, root, style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s Scene \"%s\"", enabled ? "Disable" : "Enable", scene->GetName().c_str())))
                                        root->SetInstanceFlags(InstanceFlags::Enabled, !enabled);

                                    if (open)
                                    {
                                        if (nullptr != root)
                                            displayGameObject(root);
                                    }
                                }
                            }

                            ImGui::PopID();
                        }
                    }

                    m_focusedGameObjects.clear();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopID();
    }
}