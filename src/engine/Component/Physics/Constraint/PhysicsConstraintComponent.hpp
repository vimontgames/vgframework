#include "PhysicsConstraintComponent.h"

namespace vg::engine
{
    VG_REGISTER_CLASS(PhysicsConstraintComponent, "PhysicsConstraintComponent");

    //--------------------------------------------------------------------------------------
    bool PhysicsConstraintComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsConstraintComponent::PhysicsConstraintComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    PhysicsConstraintComponent::~PhysicsConstraintComponent()
    {

    }
}