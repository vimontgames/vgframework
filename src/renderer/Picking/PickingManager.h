#pragma once

#include "renderer/IPicking.h"

namespace vg::renderer
{
    class PickingManager : public IPicking
    {
    public:
        PickingManager();
        ~PickingManager();

        PickingID           GetPickingID        (core::IObject * _object) final override;
        void                ReleasePickingID    (PickingID _id) final override;
        
        void                Update              (const gfx::IView * _view) final override;

    private:
        core::vector<core::IObject*>    m_pickingID;
    };
}