#include "SoundResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(SoundResourceList, "Sound Resource List");

    //--------------------------------------------------------------------------------------
    SoundResourceList::SoundResourceList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    SoundResourceList::~SoundResourceList()
    {

    }

    //--------------------------------------------------------------------------------------
    bool SoundResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}