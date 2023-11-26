#include "SkeletalAnimation.h"

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(SkeletalAnimation, "Skeletal Animation");

    //--------------------------------------------------------------------------------------
    bool SkeletalAnimation::registerProperties(IClassDesc & _desc)
    {
        //super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    SkeletalAnimation::SkeletalAnimation(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    SkeletalAnimation::~SkeletalAnimation()
    {

    }

    //--------------------------------------------------------------------------------------
    IAnimation * SkeletalAnimation::createFromImporterData(const AnimImporterData & _data)
    {
        auto * anim = new SkeletalAnimation(_data.name, nullptr);
        anim->m_animData = _data;
        return anim;
    }
}