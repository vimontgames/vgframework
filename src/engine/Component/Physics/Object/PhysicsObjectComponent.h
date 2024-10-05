#pragma once

#include "engine/IPhysicsObjectComponent.h"

namespace vg::engine
{
    class PhysicsObjectComponent : public IPhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL(PhysicsObjectComponent, IPhysicsObjectComponent);

        PhysicsObjectComponent(const core::string & _name, IObject * _parent);
        ~PhysicsObjectComponent();
    };
}