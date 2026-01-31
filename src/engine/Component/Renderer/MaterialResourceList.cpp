#include "engine/Precomp.h"
#include "MaterialResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_CLASS(MaterialResourceList, "Material Resource List");

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceList::MaterialResourceList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialResourceList::~MaterialResourceList()
    {

    }    
}