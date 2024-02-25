#pragma once

#include "renderer/IPicking.h"

namespace vg::renderer
{
    class PickingManager : public IPicking
    {
    public:
        PickingManager();
        ~PickingManager();

        PickingID           CreatePickingID     (core::IObject * _object) final override;
        void                ReleasePickingID    (PickingID _id) final override;
        
        void                Update              (const gfx::IView * _view, bool & _showTooltip, core::string & _tooltipMsg, core::string & _tooltipDbg) final override;

    private:
        core::vector<core::IObject*>    m_pickingID;
    };
}