#pragma once
#include "core/Object/Object.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysicsOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsOptions, core::Object);

        virtual bool    IsRigidBodyVisible(ShapeType _shape) const = 0;
    };
}