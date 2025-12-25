#pragma once

#include "engine/Component/Renderer/Instance/GraphicInstanceComponent.h"

namespace vg::engine
{
    class IMeshComponent : public GraphicInstanceComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IMeshComponent, GraphicInstanceComponent);

    };
}