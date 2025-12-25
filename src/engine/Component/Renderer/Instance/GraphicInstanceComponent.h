#pragma once
#include "core/Component/Component.h"

namespace vg::renderer
{
    class IGraphicInstance;
}

namespace vg::engine
{
    class AnimationJob;
    class DrawSkeletonJob;

    class GraphicInstanceComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(GraphicInstanceComponent, core::Component);

    protected:
        virtual renderer::IGraphicInstance *    GetGraphicInstance          () = 0;

        bool                                    UpdateFlagsFromGameObject   () override;
    };
}