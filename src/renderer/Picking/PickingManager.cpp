#include "renderer/Precomp.h"
#include "PickingManager.h"
#include "core/IInput.h"
#include "core/ISelection.h"
#include "core/IInstance.h"
#include "core/IComponent.h"
#include "core/IGameObject.h"
#include "core/string/string.h"
#include "renderer/IView.h"
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
        VG_ASSERT(nullptr != dynamic_cast<core::IComponent *>(_object), "Picking is expecting a IComponent object");

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
        VG_ASSERT(id < 32768); // Only 15 bits because upper bit is used for "ZTest" in Outline
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
    bool PickingManager::decodePickingHit(const PickingHit & _hit, PickingHitInfo & _info) const
    {
        bool isValidPicking = false;

        const uint4 id = _hit.m_id;
        const float4 pos = _hit.m_pos;

        if (0 != id.x && id.x < m_pickingID.size())
        {
            _info.m_object = m_pickingID[id.x];
            if (nullptr != _info.m_object)
            {
                _info.m_component = dynamic_cast<IComponent *>(_info.m_object);

                if (nullptr != _info.m_component)
                {
                    IObject * parent = _info.m_component->GetParent();
                    _info.m_gameObject = dynamic_cast<IGameObject *>(parent);
                    if (nullptr != _info.m_gameObject)
                    {
                        auto * parentPrefab = _info.m_gameObject->GetParentPrefab();
                        if (parentPrefab)
                        {
                            auto * selection = Kernel::getSelection();
                            bool prefabSelected = selection->IsSelectedObject(parentPrefab);

                            if (!prefabSelected)
                            {
                                _info.m_gameObject = parentPrefab;
                                _info.m_isPrefab = true;
                            }
                        }
                    }
                }

                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void PickingManager::Update(const IView * _view, bool & _showTooltip, core::string & _tooltipMsg, core::string & _tooltipDbg)
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

                PickingHitInfo info;
                if (decodePickingHit(pickingHit, info))
                {
                    if (nullptr != info.m_gameObject)
                    {
                        if (info.m_isPrefab)
                        {
                            if (_showTooltip)
                                _tooltipMsg = fmt::sprintf("%s %s", editor::style::icon::Prefab, info.m_gameObject->GetName());
                        }
                        else
                        {
                            if (_showTooltip)
                            {
                                _tooltipMsg = fmt::sprintf("%s %s", editor::style::icon::GameObject, info.m_gameObject->GetName());

                                string subObjectName = info.m_component->GetSubObjectName((uint)id.y);
                                if (!subObjectName.empty())
                                    _tooltipMsg += fmt::sprintf(", %s", subObjectName);
                            }
                        }

                        if (_showTooltip)
                            _tooltipDbg = fmt::sprintf("\n\n%s \"%s\" (ID %u, %u)\nCounter = %u\nWorldPos = (%.2f, %.2f, %.2f) Depth = %f", info.m_component->GetClassName(), info.m_component->GetName().c_str(), (uint)id.x, (uint)id.y, _view->GetPickingRequestedHitCount(), (float)pos.x, (float)pos.y, (float)pos.z, (float)pos.w);

                        if (input->IsMouseButtonJustPressed(MouseButton::Left))
                        {
                            isValidPicking = true;

                            if (ctrl)
                            {
                                if (selection->IsSelectedObject(info.m_gameObject))
                                    selection->Remove(info.m_gameObject);
                                else
                                    selection->Add(info.m_gameObject);
                            }
                            else
                            {
                                selection->SetSelectedObject(info.m_gameObject);
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