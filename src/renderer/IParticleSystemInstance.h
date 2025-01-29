#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    class IParticleSystemInstance : public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IParticleSystemInstance, GraphicInstance);
    };
}