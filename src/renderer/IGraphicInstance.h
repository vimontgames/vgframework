#pragma once

#include "core/Instance/Instance.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class IMaterialModel;
    class RenderContext;
    using PickingID = core::uint;

    class IGraphicInstance : public core::Instance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IGraphicInstance, core::Instance);

        virtual void                Draw            (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const = 0;

        virtual bool                SetMaterial     (core::uint _index, IMaterialModel * _materialModel) = 0;
        virtual IMaterialModel *    GetMaterial     (core::uint _index) const = 0;

        virtual void                ClearPickingID  () = 0;
        virtual void                SetPickingID    (PickingID _id) = 0;
        virtual PickingID           GetPickingID    () const = 0;

        virtual bool                IsSkinned       () const = 0;
    };
}