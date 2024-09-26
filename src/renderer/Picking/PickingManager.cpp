#include "renderer/Precomp.h"
#include "PickingManager.h"
#include "core/IInput.h"
#include "core/ISelection.h"
#include "core/IInstance.h"
#include "core/IComponent.h"
#include "core/IGameObject.h"
#include "core/string/string.h"
#include "gfx/IView.h"
#include "Shaders/system/toolmode.hlsl.h"
#include "renderer/IMeshInstance.h"
#include "editor/Editor_Consts.h" // should not include Editor from here

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    PickingManager::PickingManager()
    {
        m_pickingID.push_back(nullptr);
    }
    
    //--------------------------------------------------------------------------------------
    PickingManager::~PickingManager()
    {
    
    }

    //--------------------------------------------------------------------------------------
    PickingID PickingManager::CreatePickingID(IObject * _object)
    {
        PickingID id = -1;

        // Lookup available slots
        for (uint i = 1; i < m_pickingID.size(); ++i)
        {
            auto & slot = m_pickingID[i];
            if (slot == nullptr)
            {
                slot = _object;
                id = i;
                return id;
            }
        }

        // Alloc new slot
        id = (PickingID)m_pickingID.size();
        m_pickingID.push_back(_object);
        return id;
    }

    //--------------------------------------------------------------------------------------
    void PickingManager::ReleasePickingID(PickingID _id)
    {
        VG_ASSERT(0 != _id);
        VG_ASSERT(_id < m_pickingID.size());
        m_pickingID[_id] = nullptr;
    }

    //--------------------------------------------------------------------------------------
    void PickingManager::Update(const gfx::IView * _view, bool & _showTooltip, core::string & _tooltipMsg, core::string & _tooltipDbg)
    {
        if (_view->IsMouseOverView())
        {
            auto input = Kernel::getInput();
            const bool ctrl = input->IsKeyPressed(Key::LeftControl) || input->IsKeyPressed(Key::LeftControl);
            auto selection = Kernel::getSelection();
            
            bool isValidPicking = false;

            if (_view->GetPickingHitCount() > 0)
            {
                const PickingHit & pickingHit = _view->GetPickingHit(0);

                const uint4 id = pickingHit.m_id;
                const float4 pos = pickingHit.m_pos;

                if (0 != id.x && id.x < m_pickingID.size())
                {
                    IObject * object = m_pickingID[id.x];
                    if (nullptr != object)
                    {
                        IComponent * component = dynamic_cast<IComponent *>(object);

                        if (nullptr != component)
                        {
                            IObject * parent = component->GetParent();
                            IGameObject * go = dynamic_cast<IGameObject *>(parent);
                            if (nullptr != go)
                            {
                                bool autoSelectParentPrefab = false;
                                auto * parentPrefab = go->GetParentPrefab();
                                if (parentPrefab)
                                {
                                    bool prefabSelected = selection->IsSelectedObject(parentPrefab);

                                    if (!prefabSelected)
                                    {
                                        go = parentPrefab;
                                        autoSelectParentPrefab = true;
                                    }
                                }

                                if (autoSelectParentPrefab)
                                {
                                    if (_showTooltip)
                                        _tooltipMsg = fmt::sprintf("%s %s", editor::style::icon::Prefab, go->GetName());
                                }
                                else
                                {
                                    if (_showTooltip)
                                    {
                                        _tooltipMsg = fmt::sprintf("%s %s", editor::style::icon::GameObject, go->GetName());

                                        string subObjectName = component->GetSubObjectName((uint)id.y);
                                        if (!subObjectName.empty())
                                            _tooltipMsg += fmt::sprintf(", %s", subObjectName);
                                    }
                                }

                                if (_showTooltip)
                                    _tooltipDbg = fmt::sprintf("\n\n%s \"%s\" (ID %u, %u)\nCounter = %u\nWorldPos = (%.2f, %.2f, %.2f) Depth = %f", component->GetClassName(), component->GetName().c_str(), (uint)id.x, (uint)id.y, _view->GetPickingRequestedHitCount(), (float)pos.x, (float)pos.y, (float)pos.z, (float)pos.w);

                                if (input->IsMouseButtonJustPressed(MouseButton::Left))
                                {
                                    isValidPicking = true;

                                    if (ctrl)
                                    {
                                        if (selection->IsSelectedObject(go))
                                            selection->Remove(go);
                                        else
                                            selection->Add(go);
                                    }
                                    else
                                    {
                                        selection->SetSelectedObject(go);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ensure top-level selected objects are expanded
            if (isValidPicking)
            {
                const auto selectedObject = selection->GetSelectedObjects();
                const auto topSelectedObjects = selection->RemoveChildGameObjectsWithParents(selectedObject);
                for (uint i = 0; i < topSelectedObjects.size(); ++i)
                {
                    auto * obj = dynamic_cast<IGameObject *>(topSelectedObjects[i]);
                    if (obj)
                    {
                        // Open all parents but not the object itself
                        obj = dynamic_cast<IGameObject *>(obj->GetParent());
                        while (nullptr != obj)
                        {
                            obj->SetObjectFlags(ObjectFlags::Opened, true);
                            obj = dynamic_cast<IGameObject *>(obj->GetParent());
                        }
                    }
                }
            }
            else
            {
                if (input->IsMouseButtonJustPressed(MouseButton::Left))
                {
                    if (!ctrl)
                        selection->Clear();
                }
            }
        }
        else
        {
            _showTooltip = false;
        }
    }
}