#include "SkeletalAnimation.h"

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(SkeletalAnimation, "Skeletal Animation");

    //--------------------------------------------------------------------------------------
    bool SkeletalAnimation::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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
    float SkeletalAnimation::GetLength() const
    {
        return m_animData.time_end - m_animData.time_begin;
    }

    //--------------------------------------------------------------------------------------
    float SkeletalAnimation::GetFramerate() const
    {
        return m_animData.framerate;
    }

    //--------------------------------------------------------------------------------------
    bool SkeletalAnimation::SetTime(float _time)
    {
        if (m_time != _time)
        {
            m_time = _time;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IAnimation * SkeletalAnimation::createFromImporterData(const AnimImporterData & _data)
    {
        auto * anim = new SkeletalAnimation(_data.name, nullptr);
        anim->m_animData = _data;
        return anim;
    }
}