#pragma once

#include "core/Instance/Instance.h"

namespace vg::core
{
    class BitMask;
}

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class IMaterialModel;
    class RenderContext;
    using PickingID = core::uint;

    vg_enum_class(vg::renderer, GraphicInstanceType, core::u8,
        Mesh = 0,
        ParticleSystem,
        Light,
        Camera
    );

    class IGraphicInstance : public core::Instance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IGraphicInstance, core::Instance);

        virtual void                Draw                    (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const = 0;

        virtual bool                SetMaterialCount        (core::uint _count) = 0;
        virtual bool                SetMaterial             (core::uint _index, IMaterialModel * _materialModel) = 0;
        virtual IMaterialModel *    GetMaterial             (core::uint _index) const = 0;

        virtual void                SetBatchMask            (const core::BitMask & _batchMask) = 0;

        virtual void                ClearPickingID          () = 0;
        virtual void                SetPickingID            (PickingID _id) = 0;
        virtual PickingID           GetPickingID            () const = 0;

        virtual GraphicInstanceType GetGraphicIntanceType   () const = 0;
        virtual bool                IsSkinned               () const = 0;
    };
}