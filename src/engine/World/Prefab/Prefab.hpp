#include "engine/Precomp.h"
#include "Prefab.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(Prefab, "Prefab", IClassDesc::Flags::SceneNode);

    //--------------------------------------------------------------------------------------
    Prefab::Prefab(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Prefab::~Prefab()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Prefab::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }
}