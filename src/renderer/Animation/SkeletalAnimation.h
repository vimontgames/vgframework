#pragma once

#include "renderer/ISkeletalAnimation.h"
#include "renderer/Importer/AnimImporterData.h"

namespace vg::renderer
{
    class SkeletalAnimation : public ISkeletalAnimation
    {
    public:
        VG_CLASS_DECL(SkeletalAnimation, ISkeletalAnimation)

        SkeletalAnimation(const core::string & _name, core::IObject * _parent);
        ~SkeletalAnimation();

        static IAnimation * createFromImporterData(const AnimImporterData & _data);

    //private:
        AnimImporterData m_animData;    
    };
}