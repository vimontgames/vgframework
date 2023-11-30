#include "core/Precomp.h"
#include "Behaviour.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool Behaviour::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Behaviour::Behaviour(const core::string & _name, IObject * _parent) :
        IBehaviour(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Behaviour::~Behaviour()
    {

    }
}