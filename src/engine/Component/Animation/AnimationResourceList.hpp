#include "AnimationResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(AnimationResourceList, "Animation Resource List");

    //--------------------------------------------------------------------------------------
    AnimationResourceList::AnimationResourceList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    AnimationResourceList::~AnimationResourceList()
    {

    }

    //--------------------------------------------------------------------------------------
    bool AnimationResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }   
}