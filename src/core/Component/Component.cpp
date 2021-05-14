#include "core/Precomp.h"
#include "Component.h"
#include "core/IEntity.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Component::Component(const core::string & _name, IObject * _parent) :
        ObjectPointer(_name, _parent)
    {
        VG_ASSERT(dynamic_cast<IEntity*>(_parent));
    }

    //--------------------------------------------------------------------------------------
    Component::~Component()
    {
        
    }

    //--------------------------------------------------------------------------------------
    IEntity * Component:: getEntity() const 
    { 
        return (IEntity*)getParent(); 
    }
}