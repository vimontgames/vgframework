#pragma once

#include "engine/IPhysicsConstraintComponent.h"

namespace vg::engine
{
    class PhysicsConstraintComponent : public IPhysicsConstraintComponent
    {
    public:
        VG_CLASS_DECL(PhysicsConstraintComponent, IPhysicsConstraintComponent);   
        
        PhysicsConstraintComponent(const core::string & _name, IObject * _parent);
        ~PhysicsConstraintComponent();
    };
}