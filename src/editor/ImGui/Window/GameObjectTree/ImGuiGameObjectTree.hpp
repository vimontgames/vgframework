#include "ImGuiGameObjectTree.h"
#include "editor/ImGui/ImGui.h"
#include "engine/IWorldResource.h"
#include "core/IGameObject.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiGameObjectTree::ImGuiGameObjectTree(const core::string& _icon, const core::string& _path, const core::string& _name, Flags _flags) :
        ImGuiWindow(_icon, _path, _name, _flags)
    {
    }

    //--------------------------------------------------------------------------------------
    bool doDragDrop(IGameObject* _from, IGameObject* _to, style::draganddrop::Type _dest)
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
        case style::draganddrop::Type::Node:
        {
            // '_to' is the new parent of '_from'
            IGameObject* fromParent = dynamic_cast<IGameObject*>(_from->getParent());
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

        case style::draganddrop::Type::AfterNode:
        {
            // '_from' is not the next following brother of '_to'
            IGameObject* fromParent = dynamic_cast<IGameObject*>(_from->getParent());
            if (fromParent)
            {
                IGameObject* toParent = dynamic_cast<IGameObject*>(_to->getParent());
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
    void ImGuiGameObjectTree::displayGameObject(IGameObject* _gameObject, uint* _count)
    {
        //const bool counting = (_count != nullptr);
        const auto children = _gameObject->GetChildren();

        if (_count)
            (*_count)++;

        const bool disabled = !asBool(_gameObject->GetFlags() & IGameObject::Flags::Enabled);
        ImGui::BeginDisabled(disabled);

        bool open = false;
        auto availableWidth = ImGui::GetContentRegionMax().x;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        if (children.size() > 0)
            flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
        else
            flags |= ImGuiTreeNodeFlags_Leaf;

        // row background
        {
            u32 rowBgColor;

            if (*_count & 1)
                rowBgColor = GetColorU32(Editor::get()->getRenderer()->GetImGuiAdapter()->GetRowColorEven());
            else
                rowBgColor = GetColorU32(Editor::get()->getRenderer()->GetImGuiAdapter()->GetRowColorOdd());

            DrawRowsBackground(1, rowBgColor);
        }

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

        auto pos = ImGui::GetCursorPos();

        const bool renaming = m_gameObjectMenu.m_RenamingGameObject == _gameObject;
        if (renaming)
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
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
            {
                IGameObject* from = *(IGameObject**)payload->Data;
                IGameObject* to = _gameObject;

                if (payload->Delivery)
                {
                    doDragDrop(from, to, style::draganddrop::Type::Node);
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
            //if (!counting)
            updateSelection(_gameObject);
        }

        m_gameObjectMenu.Display(_gameObject);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
        {
            //VG_INFO("BeginDragDropSource");

            //if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
            //    ImGui::Text("Moving \"%s\"", _gameObject->getName().c_str());
            ImGui::SetDragDropPayload("GameObject", &_gameObject, sizeof(IGameObject*));
            ImGui::EndDragDropSource();
        }

        // Invisible selectable for interline
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style::draganddrop::interlineSize.y - 1);
                string beforeGO = fmt::sprintf("###after %s", _gameObject->getName().c_str());
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
                    doDragDrop(from, to, style::draganddrop::Type::AfterNode);
                    m_dragAndDropInterlineTarget = nullptr;
                }
                else
                    m_dragAndDropInterlineTarget = to;

                m_dragAndDropNodeTarget = nullptr;
            }

            ImGui::EndDragDropTarget();
        }

        // icons
        const auto& components = _gameObject->GetComponents();
        if (components.size() > 0)
        {
            auto bakePos = ImGui::GetCursorPos();
            ImGui::SameLine();

            float totalSize = 0;
            for (i64 i = components.size() - 1; i >= 0; --i)
            {
                const auto* component = components[i];
                const auto* componentClassDesc = component->getClassDesc();
                auto icon = componentClassDesc->GetIcon();
                auto size = ImGui::CalcTextSize(icon).x;
                totalSize += size + ImGui::GetStyle().FramePadding.x;

                ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

                if (asBool(IComponent::Flags::Enabled & component->GetFlags()))
                    textColor.w = 0.5f;
                else
                    textColor.w = 0.15f;

                ImGui::SetCursorPosX(availableWidth - totalSize);
                ImGui::SetCursorPosY(pos.y);
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                ImGui::Text(icon);

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    string tooltip = fmt::sprintf("%s:\n%s", componentClassDesc->GetClassDisplayName(), componentClassDesc->GetDescription());
                    ImGui::SetTooltip(tooltip.c_str());
                }

                ImGui::PopStyleColor();
            }

            ImGui::SetCursorPosX(bakePos.x);
            ImGui::SetCursorPosY(bakePos.y);
        }

        if (open)
        {
            for (uint j = 0; j < children.size(); ++j)
            {
                IGameObject* child = children[j];
                displayGameObject(child, _count);
            }

            //if (!counting)
            ImGui::TreePop();
        }

        ImGui::EndDisabled();
    }
}