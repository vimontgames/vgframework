#pragma once

#include "core/Singleton/Singleton.h"
#include "renderer/IPicking.h"

struct PickingHit;

namespace vg::renderer
{
    struct PickingHitInfo
    {
        core::IObject *     m_object     = nullptr;
        core::IComponent *  m_component  = nullptr;
        core::IGameObject * m_gameObject = nullptr;
        bool                m_isPrefab   = false;
    };

    class PickingManager : public IPicking, public core::Singleton<PickingManager>
    {
    public:
        PickingManager();
        ~PickingManager();

        PickingID           CreatePickingID     (core::IObject * _object) final override;
        void                ReleasePickingID    (PickingID _id) final override;
        
        void                Update              (const IView * _view, bool & _showTooltip, core::string & _tooltipMsg, core::string & _tooltipDbg) final override;

        bool                decodePickingHit    (const PickingHit & _hit, PickingHitInfo & _info) const;

    private:
        core::vector<core::IObject*>    m_pickingID;
    };
}