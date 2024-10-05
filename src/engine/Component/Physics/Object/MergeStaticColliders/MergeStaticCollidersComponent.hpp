#include "MergeStaticCollidersComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(MergeStaticCollidersComponent, "Merge Static Colliders", "Physics", "Merge all static colliders in a single physics body.", editor::style::icon::PhysicsMergeStatic, -1);

    //--------------------------------------------------------------------------------------
    bool MergeStaticCollidersComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MergeStaticCollidersComponent::MergeStaticCollidersComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
    }

    //--------------------------------------------------------------------------------------
    MergeStaticCollidersComponent::~MergeStaticCollidersComponent()
    {

    }
}