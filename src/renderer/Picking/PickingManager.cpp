#include "renderer/Precomp.h"
#include "PickingManager.h"
#include "core/IInput.h"
#include "core/ISelection.h"
#include "core/IInstance.h"
#include "core/IComponent.h"
#include "core/IGameObject.h"
#include "gfx/IView.h"
#include "Shaders/system/toolmode.hlsl.h"
#include "renderer/IMeshInstance.h"

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
    PickingID PickingManager::GetPickingID(IObject * _object)
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
    void PickingManager::Update(const gfx::IView * _view, bool & _showTooltip, core::string & _tooltipMsg)
    {
        const auto mousePos = _view->GetRelativeMousePos();
        const auto viewSize = _view->GetSize();

        if (all(mousePos.xy >= 0) && all(mousePos.xy < viewSize.xy))
        {
            auto input = Kernel::getInput();
            const bool ctrl = input->IsKeyPressed(Key::LCONTROL) || input->IsKeyPressed(Key::LCONTROL);
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
                            IObject * parent = component->getParent();
                            IGameObject * go = dynamic_cast<IGameObject *>(parent);
                            if (nullptr != go)
                            {
                                if (_showTooltip)
                                {
                                    char temp[256];
                                    sprintf_s(temp, "GameObject \"%s\"\n%s \"%s\" (ID %u)\nSubID = %u\nCounter = %u\nWorldPos = (%.2f, %.2f, %.2f) Depth = %f", go->getName().c_str(), component->getClassName(), component->getName().c_str(), (uint)id.x, (uint)id.y, _view->GetPickingRequestedHitCount(), (float)pos.x, (float)pos.y, (float)pos.z, (float)pos.w);
                                    _tooltipMsg = temp;
                                }

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
                                        selection->Clear();
                                        selection->Add(go);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!isValidPicking)
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