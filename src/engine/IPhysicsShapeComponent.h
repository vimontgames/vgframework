#pragma once

#include "engine/IPhysicsObjectComponent.h"

namespace vg::engine
{
    class IPhysicsShapeComponent : public IPhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsShapeComponent, IPhysicsObjectComponent);
    };
}