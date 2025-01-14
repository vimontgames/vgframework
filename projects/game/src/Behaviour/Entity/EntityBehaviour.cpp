#include "Precomp.h"
#include "EntityBehaviour.h"

// Component base class is not being exposed but still needs to be registered for custom RTTI
VG_REGISTER_OBJECT_CLASS(EntityBehaviour, "Entity");

using namespace vg::core;

//--------------------------------------------------------------------------------------
EntityBehaviour::EntityBehaviour(const vg::core::string & _name, vg::core::IObject * _parent) :
    super(_name, _parent)
{

}

//--------------------------------------------------------------------------------------
EntityBehaviour::~EntityBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool EntityBehaviour::registerProperties(vg::core::IClassDesc & _desc)
{
    super::registerProperties(_desc);

    return true;
}
