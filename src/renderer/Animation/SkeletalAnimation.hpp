#include "SkeletalAnimation.h"

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(SkeletalAnimation);

    //--------------------------------------------------------------------------------------
    bool SkeletalAnimation::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(SkeletalAnimation, "Skeletal Animation", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

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