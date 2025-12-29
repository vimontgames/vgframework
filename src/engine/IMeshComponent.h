#pragma once

#include "engine/Component/Renderer/Instance/GraphicInstanceComponent.h"

namespace vg::engine
{
    using MaterialIndex = core::uint;
    class IMaterialResource;

    class IMeshComponent : public GraphicInstanceComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IMeshComponent, GraphicInstanceComponent);

        virtual MaterialIndex       GetMaterialIndex    (const core::string & _materialName) const = 0;
        virtual IMaterialResource * GetMaterialResource (MaterialIndex _materialIndex) const = 0;
    };
}