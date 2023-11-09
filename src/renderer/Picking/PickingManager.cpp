#include "renderer/Precomp.h"
#include "PickingManager.h"

#include "Shaders/system/toolmode.hlsl.h"

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
    void PickingManager::ProcessPickingData(const PickingData * _pickingData)
    {
        const uint4 id = _pickingData->m_id;
        const float4 pos = _pickingData->m_pos;

        VG_INFO("[Picking] id = %u, %u, %u, %u pos = %.2f, %.2f, %.2f, %.2f", (uint)id.x, (uint)id.y, (uint)id.z, (uint)id.w, (float)pos.x, (float)pos.y, (float)pos.z, (float)pos.w);

    }
}