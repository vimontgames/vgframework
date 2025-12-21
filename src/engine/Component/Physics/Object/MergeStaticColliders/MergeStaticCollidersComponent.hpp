#include "MergeStaticCollidersComponent.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // When the 'IsMergeStaticBodiesEnabled' option is enabled in physics options, component
    // is added to scene root and will hold all the static bodies to optimize physics when
    // engine start, and the original individual physics components are disabled.
    // TODO: This component should be hidden to user.
    //--------------------------------------------------------------------------------------

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