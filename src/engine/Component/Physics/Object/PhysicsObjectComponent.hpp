#include "PhysicsObjectComponent.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool PhysicsObjectComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsObjectComponent::PhysicsObjectComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    PhysicsObjectComponent::~PhysicsObjectComponent()
    {

    }
}