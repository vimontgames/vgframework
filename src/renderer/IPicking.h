#pragma once

#include "core/Object/Object.h"

struct PickingData;

namespace vg::gfx
{
    class IView;
}

namespace vg::renderer
{
    using PickingID = core::uint;

    class IPicking : public core::Object
    {
    public:
        virtual ~IPicking() {}

        virtual PickingID           GetPickingID        (core::IObject * _object) = 0;
        virtual void                ReleasePickingID    (PickingID _id) = 0;

        virtual void                Update              (const gfx::IView * _view, bool & _showTooltip, core::string & _tooltipMsg) = 0;
    };
}