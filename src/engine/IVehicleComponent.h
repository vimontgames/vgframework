#pragma once

#include "engine/Component/Physics/Constraint/PhysicsConstraintComponent.h"

namespace vg::engine
{
    class IVehicleComponent : public PhysicsConstraintComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IVehicleComponent, PhysicsConstraintComponent);

    };
}